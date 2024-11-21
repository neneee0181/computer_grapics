#pragma once

#include <limits>  // std::numeric_limits ����� ���� ��� �߰�
#include <gl/glm/glm/glm.hpp>

#include "CustomContactResultCallback.h"

#include"include/btBulletCollisionCommon.h"
#include"include/btBulletDynamicsCommon.h"
#include "Model.h"

void removeRigidBodyFromModel(Model& model);

// Bullet Physics ���� ������
btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;

// Bullet Physics �ʱ�ȭ �Լ�
void initPhysics() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

// Model�� AABB�� ����Ͽ� ũ�⸦ ��ȯ�ϴ� �Լ�
glm::vec3 calculateModelSize(const Model& model) {
    // �ʱ� �ּ�, �ִ밪�� ����
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(std::numeric_limits<float>::lowest());

    // ��� ���� ��ȸ
    for (const Vertex& vertex : model.vertices) {
        // ���� ��ġ�� ���� �������� ��ȯ
        glm::vec4 transformedVertex = model.modelMatrix * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);

        // �ּҰ��� �ִ밪 ����
        min.x = std::min(min.x, transformedVertex.x);
        min.y = std::min(min.y, transformedVertex.y);
        min.z = std::min(min.z, transformedVertex.z);

        max.x = std::max(max.x, transformedVertex.x);
        max.y = std::max(max.y, transformedVertex.y);
        max.z = std::max(max.z, transformedVertex.z);
    }

    // ��, ����, ���̸� ���
    return max - min;
}

void alignModelToOrigin(Model& model) {
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(std::numeric_limits<float>::lowest());

    // 1. AABB ��� (���� �ּ�/�ִ� ���� ã��)
    for (const Vertex& vertex : model.vertices) {
        min.x = std::min(min.x, vertex.x);
        min.y = std::min(min.y, vertex.y);
        min.z = std::min(min.z, vertex.z);

        max.x = std::max(max.x, vertex.x);
        max.y = std::max(max.y, vertex.y);
        max.z = std::max(max.z, vertex.z);
    }

    glm::vec3 center = (min + max) * 0.5f; // AABB�� �߽� ���

    // 2. ��� ������ �߽� �������� �̵�
    for (auto& vertex : model.vertices) {
        vertex.x -= center.x;
        vertex.y -= center.y;
        vertex.z -= center.z;
    }

    // 3. �� ��Ŀ� �߽� �̵� �߰�
    model.modelMatrix = glm::translate(model.modelMatrix, center);
}

// �𵨿� ���� �浹 ��ü�� ��ü ���� �� ���� ���迡 �߰�
void addModelToPhysicsWorld(Model& model) {
    // ���� ũ�� ��� (AABB)
    glm::vec3 size = calculateModelSize(model);

    // �浹 �ڽ� ����
    btCollisionShape* shape = nullptr;

    // Box ������ �浹 ��� ���� (0.5��� ũ�� ����)
    shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));

    if (!shape) {
        std::cerr << "Failed to create collision shape for model: " << model.name << std::endl;
        return;
    }

    // ���� ��ġ ����
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(
        model.modelMatrix[3].x,
        model.modelMatrix[3].y,
        model.modelMatrix[3].z
    ));

    // ���� �� ���� ����
    btScalar mass = 1.0f;
    btVector3 localInertia(0, 0, 0);
    if (mass != 0.0f) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    // RigidBody ���� �� ���� ���迡 �߰�
    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    dynamicsWorld->addRigidBody(body);
    model.rigidBody = body;
}

// ��� �𵨿� ���� ���� ���� �浹 ��ü �ʱ�ȭ
void initializeModelsWithPhysics(std::vector<Model>& models) {
    for (auto& model : models) {
        addModelToPhysicsWorld(model);
    }
}

void updatePhysics(std::vector<Model>& models, Model& model_basket) {
    CustomContactResultCallback resultCallback;

    // �� �𵨿� ���� �ִϸ��̼ǵ� ��ġ�� ȸ�� ���¸� Bullet Physics�� ����ȭ
    for (auto& model : models) {
        if (model.rigidBody) {
            btTransform transform;
            transform.setFromOpenGLMatrix(glm::value_ptr(model.modelMatrix)); // modelMatrix�� ȸ�� ����
            model.rigidBody->setWorldTransform(transform);
            model.rigidBody->getMotionState()->setWorldTransform(transform);
        }
    }

    // �ٱ��� �� ��ġ �� ȸ�� ����ȭ
    if (model_basket.rigidBody) {
        btTransform basketTransform;
        basketTransform.setFromOpenGLMatrix(glm::value_ptr(model_basket.modelMatrix)); // modelMatrix�� ȸ�� ����
        model_basket.rigidBody->setWorldTransform(basketTransform);
        model_basket.rigidBody->getMotionState()->setWorldTransform(basketTransform);
    }

    // �𵨵� ������ �ٱ��� �� ������ �浹 �˻� ����
    for (auto& model : models) {
        if (model.rigidBody && model_basket.rigidBody) {
            resultCallback.reset();  // ���� �浹 ���¸� �ʱ�ȭ
            dynamicsWorld->contactPairTest(model.rigidBody, model_basket.rigidBody, resultCallback);

            // �浹�� �����Ǿ����� Ȯ��
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

// Ư�� ���� �����ϴ� �Լ�
void removeModelFromWorld(std::vector<Model>& models, Model& modelToDelete) {
    // ���� ���迡�� ���� RigidBody ����
    if (modelToDelete.rigidBody) {
        dynamicsWorld->removeRigidBody(modelToDelete.rigidBody);

        // RigidBody�� �Ҵ�� ���ҽ� ����
        delete modelToDelete.rigidBody->getMotionState();
        delete modelToDelete.rigidBody;
        modelToDelete.rigidBody = nullptr;
    }

    // models ���Ϳ��� �ش� �� ����
    auto it = std::find_if(models.begin(), models.end(), [&](const Model& model) {
        return &model == &modelToDelete;
        });

    if (it != models.end()) {
        models.erase(it);
    }
}

// Ư�� ���� RigidBody�� �����ϴ� �Լ�
void removeRigidBodyFromModel(Model& model) {
    if (model.rigidBody) {
        // ���� ���迡�� RigidBody ����
        dynamicsWorld->removeRigidBody(model.rigidBody);

        // RigidBody�� �Ҵ�� ���ҽ� ����
        delete model.rigidBody->getMotionState();
        delete model.rigidBody;
        model.rigidBody = nullptr;
    }
}

void UpdateRigidBodyTransform(Model& model) {
    if (!model.rigidBody) return; // ���� ��ü�� ������ �ǳʶ�
    glm::mat4 modelMatrix;
  
    modelMatrix = model.modelMatrix;
   
    btTransform transform;

    // glm::mat4 -> btTransform ��ȯ
    transform.setFromOpenGLMatrix(glm::value_ptr(modelMatrix));
    model.rigidBody->setWorldTransform(transform); // Bullet ���� ��ü ����
}

void RenderCollisionBox(const Model& model) {
    if (!model.rigidBody) return; // ���� ��ü�� ������ �ǳʶ�

    // OpenGL ���� ����
    glPushAttrib(GL_CURRENT_BIT);

    // AABB ���
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
    glColor3f(1.0f, 0.0f, 0.0f); // ������

    glBegin(GL_LINES);
    for (int i = 0; i < 24; i += 2) {
        glVertex3fv(glm::value_ptr(corners[indices[i]]));
        glVertex3fv(glm::value_ptr(corners[indices[i + 1]]));
    }
    glEnd();

    // OpenGL ���� ����
    glPopAttrib();
}

