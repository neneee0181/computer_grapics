#pragma once

#include <limits>  // std::numeric_limits 사용을 위한 헤더 추가
#include <gl/glm/glm/glm.hpp>

#include "CustomContactResultCallback.h"

#include"include/btBulletCollisionCommon.h"
#include"include/btBulletDynamicsCommon.h"
#include "Model.h"

void removeRigidBodyFromModel(Model& model);

// Bullet Physics 관련 변수들
btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;

// Bullet Physics 초기화 함수
void initPhysics() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

// Model의 AABB를 계산하여 크기를 반환하는 함수
glm::vec3 calculateModelSize(const Model& model) {
    // 초기 최소, 최대값을 설정
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(std::numeric_limits<float>::lowest());

    // 모든 정점 순회
    for (const Vertex& vertex : model.vertices) {
        // 정점 위치를 월드 공간으로 변환
        glm::vec4 transformedVertex = model.modelMatrix * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);

        // 최소값과 최대값 갱신
        min.x = std::min(min.x, transformedVertex.x);
        min.y = std::min(min.y, transformedVertex.y);
        min.z = std::min(min.z, transformedVertex.z);

        max.x = std::max(max.x, transformedVertex.x);
        max.y = std::max(max.y, transformedVertex.y);
        max.z = std::max(max.z, transformedVertex.z);
    }

    // 폭, 높이, 깊이를 계산
    return max - min;
}

void alignModelToOrigin(Model& model) {
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(std::numeric_limits<float>::lowest());

    // 1. AABB 계산 (모델의 최소/최대 정점 찾기)
    for (const Vertex& vertex : model.vertices) {
        min.x = std::min(min.x, vertex.x);
        min.y = std::min(min.y, vertex.y);
        min.z = std::min(min.z, vertex.z);

        max.x = std::max(max.x, vertex.x);
        max.y = std::max(max.y, vertex.y);
        max.z = std::max(max.z, vertex.z);
    }

    glm::vec3 center = (min + max) * 0.5f; // AABB의 중심 계산

    // 2. 모든 정점을 중심 기준으로 이동
    for (auto& vertex : model.vertices) {
        vertex.x -= center.x;
        vertex.y -= center.y;
        vertex.z -= center.z;
    }

    // 3. 모델 행렬에 중심 이동 추가
    model.modelMatrix = glm::translate(model.modelMatrix, center);
}

// 모델에 대한 충돌 객체와 강체 생성 및 물리 세계에 추가
void addModelToPhysicsWorld(Model& model) {
    // 모델의 크기 계산 (AABB)
    glm::vec3 size = calculateModelSize(model);

    // 충돌 박스 생성
    btCollisionShape* shape = nullptr;

    // Box 형태의 충돌 경계 생성 (0.5배로 크기 설정)
    shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));

    if (!shape) {
        std::cerr << "Failed to create collision shape for model: " << model.name << std::endl;
        return;
    }

    // 시작 위치 설정
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(
        model.modelMatrix[3].x,
        model.modelMatrix[3].y,
        model.modelMatrix[3].z
    ));

    // 질량 및 관성 설정
    btScalar mass = 1.0f;
    btVector3 localInertia(0, 0, 0);
    if (mass != 0.0f) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    // RigidBody 생성 및 물리 세계에 추가
    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    dynamicsWorld->addRigidBody(body);
    model.rigidBody = body;
}

// 모든 모델에 대한 물리 세계 충돌 객체 초기화
void initializeModelsWithPhysics(std::vector<Model>& models) {
    for (auto& model : models) {
        addModelToPhysicsWorld(model);
    }
}

void updatePhysics(std::vector<Model>& models, Model& model_basket) {
    CustomContactResultCallback resultCallback;

    // 각 모델에 대해 애니메이션된 위치와 회전 상태를 Bullet Physics에 동기화
    for (auto& model : models) {
        if (model.rigidBody) {
            btTransform transform;
            transform.setFromOpenGLMatrix(glm::value_ptr(model.modelMatrix)); // modelMatrix에 회전 포함
            model.rigidBody->setWorldTransform(transform);
            model.rigidBody->getMotionState()->setWorldTransform(transform);
        }
    }

    // 바구니 모델 위치 및 회전 동기화
    if (model_basket.rigidBody) {
        btTransform basketTransform;
        basketTransform.setFromOpenGLMatrix(glm::value_ptr(model_basket.modelMatrix)); // modelMatrix에 회전 포함
        model_basket.rigidBody->setWorldTransform(basketTransform);
        model_basket.rigidBody->getMotionState()->setWorldTransform(basketTransform);
    }

    // 모델들 각각과 바구니 모델 사이의 충돌 검사 수행
    for (auto& model : models) {
        if (model.rigidBody && model_basket.rigidBody) {
            resultCallback.reset();  // 이전 충돌 상태를 초기화
            dynamicsWorld->contactPairTest(model.rigidBody, model_basket.rigidBody, resultCallback);

            // 충돌이 감지되었는지 확인
            if (resultCallback.hitDetected) {
                //
            }
        }
    }
}

void cleanupPhysics() {
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}

// 특정 모델을 삭제하는 함수
void removeModelFromWorld(std::vector<Model>& models, Model& modelToDelete) {
    // 물리 세계에서 모델의 RigidBody 제거
    if (modelToDelete.rigidBody) {
        dynamicsWorld->removeRigidBody(modelToDelete.rigidBody);

        // RigidBody에 할당된 리소스 해제
        delete modelToDelete.rigidBody->getMotionState();
        delete modelToDelete.rigidBody;
        modelToDelete.rigidBody = nullptr;
    }

    // models 벡터에서 해당 모델 제거
    auto it = std::find_if(models.begin(), models.end(), [&](const Model& model) {
        return &model == &modelToDelete;
        });

    if (it != models.end()) {
        models.erase(it);
    }
}

// 특정 모델의 RigidBody만 제거하는 함수
void removeRigidBodyFromModel(Model& model) {
    if (model.rigidBody) {
        // 물리 세계에서 RigidBody 제거
        dynamicsWorld->removeRigidBody(model.rigidBody);

        // RigidBody에 할당된 리소스 해제
        delete model.rigidBody->getMotionState();
        delete model.rigidBody;
        model.rigidBody = nullptr;
    }
}

void UpdateRigidBodyTransform(Model& model) {
    if (!model.rigidBody) return; // 물리 객체가 없으면 건너뜀
    glm::mat4 modelMatrix;
  
    modelMatrix = model.modelMatrix;
   
    btTransform transform;

    // glm::mat4 -> btTransform 변환
    transform.setFromOpenGLMatrix(glm::value_ptr(modelMatrix));
    model.rigidBody->setWorldTransform(transform); // Bullet 물리 객체 갱신
}

void RenderCollisionBox(const Model& model) {
    if (!model.rigidBody) return; // 물리 객체가 없으면 건너뜀

    // OpenGL 상태 저장
    glPushAttrib(GL_CURRENT_BIT);

    // AABB 계산
    btVector3 aabbMin, aabbMax;
    model.rigidBody->getCollisionShape()->getAabb(model.rigidBody->getWorldTransform(), aabbMin, aabbMax);

    glm::vec3 min = glm::vec3(aabbMin.getX(), aabbMin.getY(), aabbMin.getZ());
    glm::vec3 max = glm::vec3(aabbMax.getX(), aabbMax.getY(), aabbMax.getZ());

    glm::vec3 corners[8] = {
        glm::vec3(min.x, min.y, min.z),
        glm::vec3(max.x, min.y, min.z),
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(max.x, max.y, max.z),
        glm::vec3(min.x, max.y, max.z)
    };

    GLuint indices[24] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    glLineWidth(2.0f);
    glColor3f(1.0f, 0.0f, 0.0f); // 빨간색

    glBegin(GL_LINES);
    for (int i = 0; i < 24; i += 2) {
        glVertex3fv(glm::value_ptr(corners[indices[i]]));
        glVertex3fv(glm::value_ptr(corners[indices[i + 1]]));
    }
    glEnd();

    // OpenGL 상태 복원
    glPopAttrib();
}

