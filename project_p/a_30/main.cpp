#include <iostream>
#include <vector> 
#include <algorithm>  // std::sort 사용을 위한 헤더
#include <random>

#include "shaderMaker.h"
#include "DefaultModel.h"
#include "KeyBoard.h"
#include "Camera.h"
#include "Light.h"
#include "Debug.h"
#include "LoadProgress.h"
#include "SierpinskiModel.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
float calculateDistanceToCamera(const Model* model, const glm::vec3& cameraPosition);

//모델
vector<Model*> models;
std::vector<Model*> opaqueModels;
std::vector<Model*> transparentModels;

std::random_device rd;
std::mt19937 gen(rd());
uniform_real_distribution<> snow_location_dis_x(-0.65, 0.65);


void checkSnowBoard() {
    for (const auto& model_1 : models) {
        if (model_1->name != "board") continue;
        for (auto& model_2 : models) {
            if (model_2->name != "snow") continue;

            CustomContactResultCallback resultCallback;
            dynamicsWorld->contactPairTest(model_1->rigidBody, model_2->rigidBody, resultCallback);
            if (resultCallback.hitDetected) {
                model_2->matrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(snow_location_dis_x(gen), 1.5, snow_location_dis_x(gen))), glm::vec3(0.1, 0.1, 0.1));
            }
        }
    }
}


void timer(int value) {

    if (keyState['s']) { // 눈 내리기
        for (auto& model : models) {
            if (model->name != "snow") continue;
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, glm::vec3(0.0, -(model->speed), 0.0));

            // 자전(회전)
            glm::vec3 center = glm::vec3(0.0, 0.0, 0.0); // 회전 중심 설정 (모델의 로컬 중심)
            glm::mat4 rotation = glm::mat4(1.0f);
            rotation = glm::translate(rotation, center); // 중심으로 이동
            rotation = glm::rotate(rotation, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0)); // Y축 기준 회전
            rotation = glm::translate(rotation, -center); // 원래 위치로 복귀

            model->matrix = matrix * rotation * model->matrix; // 이동 + 회전 적용

        }
        checkSnowBoard();
    }

    if (keyState['r']) { // 조명이 화면 중앙을 기준 공전
        //물체 이동
        for (auto& model : models) {
            if (model->name != "light_m") continue;

            glm::vec3 center = glm::vec3(0.0, 0.0, 0.0); // 회전 중심 설정 (모델의 로컬 중심)
            glm::mat4 rotation = glm::mat4(1.0f);
            rotation = glm::translate(rotation, center); // 중심으로 이동
            rotation = glm::rotate(rotation, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0)); // Y축 기준 회전
            rotation = glm::translate(rotation, -center); // 원래 위치로 복귀
            model->matrix = rotation * model->matrix; // 이동 + 회전 적용

            // `lightPos`에 회전 적용
            glm::vec4 lightPos4 = glm::vec4(lightPos, 1.0f); // `vec3`를 `vec4`로 확장
            lightPos4 = rotation * lightPos4; // 회전 적용
            lightPos = glm::vec3(lightPos4); // 다시 `vec3`로 변환
        }
    }

    if (keyState['y']) { // Y키를 누른 상태면 카메라 회전
        static float angle = 0.0f; // 회전 각도 (초기값 0)
        angle += 0.1f; // 회전 속도 (1도씩 회전)

        // 회전 중심: (0, 0, 0)
        glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f); // Y축 기준 회전

        // 카메라 위치 회전 (0, 0, 0을 중심으로)
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), rotationAxis);
        glm::vec4 rotatedPos = rotationMatrix * glm::vec4(cameraPos, 1.0f);
        cameraPos = glm::vec3(rotatedPos);

        // 카메라 방향 업데이트 (항상 (0, 0, 0)을 바라보게 설정)
        cameraDirection = glm::normalize(-cameraPos); // (0, 0, 0) - cameraPos

        // 카메라 위쪽 방향 유지
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    // Planet 공전
    for (auto& model : models) {
        glm::vec3 center = glm::vec3(0.0, 0.0, 0.0); // 공전 중심
        glm::mat4 rotation = glm::mat4(1.0f);

        if (model->name == "planet1") {
            // 공전을 위한 변환
            rotation = glm::translate(rotation, center);
            rotation = glm::rotate(rotation, glm::radians(0.5f), glm::vec3(1.0, 0.0, 0.0));
            rotation = glm::rotate(rotation, glm::radians(0.5f), glm::vec3(0.0, 1.0, 0.0));

            rotation = glm::translate(rotation, -center);
        }

        if (model->name == "planet2") {
            // 공전을 위한 변환
            rotation = glm::translate(rotation, center);
            rotation = glm::rotate(rotation, glm::radians(0.5f), glm::vec3(1.0, 0.0, 0.0));
            rotation = glm::rotate(rotation, glm::radians(-0.5f), glm::vec3(0.0, 1.0, 0.0));

            rotation = glm::translate(rotation, -center);
        }

        if (model->name == "planet3") {
            // planet3 공전 (반대 방향)
            rotation = glm::translate(rotation, center);
            rotation = glm::rotate(rotation, glm::radians(-0.5f), glm::vec3(1.0, 0.0, 0.0)); // X축 반대 회전
            rotation = glm::rotate(rotation, glm::radians(-0.5f), glm::vec3(0.0, 1.0, 0.0)); // Y축 반대 회전
            rotation = glm::translate(rotation, -center);
        }

        // 모델의 변환 매트릭스 업데이트
        model->matrix = rotation * model->matrix;
    }

    UpdateRigidBodyTransforms(models);
    glutPostRedisplay();
    glutTimerFunc(16, timer, ++value);
}



int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);  // 모니터의 가로 해상도
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT); // 모니터의 세로 해상도
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 600); // 창 크기를 화면 크기로 설정
    glutCreateWindow("opengl template v16");
    //glutFullScreen(); // 전체 화면으로 전환

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    make_shaderProgram();

    initPhysics(); // Bullet 초기화 함수 호출

    //loadModelWithProgress <DefaultModel>("squ.obj", "obj/", "squ", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), models);
    //loadModelWithProgress <DefaultModel>("pira.obj", "obj/", "pira", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 15.0)), glm::vec3(1.0, 1.0, 1.0)), models);
    loadModelWithProgress <DefaultModel>("board.obj", "obj/", "board", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), models);
    loadModelWithProgress <DefaultModel>("piramid.obj", "obj/", "pira", "box", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.05, 0.043, 0.05)), glm::vec3(0.0, 5.0, 0.0)), models);
    loadModelWithProgress <DefaultModel>("sphere.obj", "obj/", "light_m", "box", glm::scale(glm::translate(glm::mat4(1.0f), lightPos), glm::vec3(0.003, 0.003, 0.003)), models);

    for (int i = 0; i < 5; ++i) {
        loadModelWithProgress <DefaultModel>("Snowflake.obj", "obj/", "snow", "sphere", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(snow_location_dis_x(gen), 1.5, snow_location_dis_x(gen))), glm::vec3(0.1, 0.1, 0.1)), models);
    }

    loadModelWithProgress <DefaultModel>("squ.obj", "obj/", "squ", "box", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.01, 0.06, 0.01)), glm::vec3(60.0, 5.0, 0.0)), models);
    loadModelWithProgress <DefaultModel>("squ.obj", "obj/", "squ", "box", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.01, 0.06, 0.01)), glm::vec3(-60.0, 5.0, 0.0)), models);
    loadModelWithProgress <DefaultModel>("squ.obj", "obj/", "squ", "box", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.01, 0.06, 0.01)), glm::vec3(0.0, 5.0, 60.0)), models);
    loadModelWithProgress <DefaultModel>("squ.obj", "obj/", "squ", "box", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.01, 0.06, 0.01)), glm::vec3(0.0, 5.0, -60.0)), models);

    //SierpinskiModel* sp_model = new SierpinskiModel(1);
    //glm::mat4 sp_m = glm::mat4(1.0f);
    //sp_m = glm::scale(sp_m, glm::vec3(0.5, 0.5, 0.5));
    //sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.0, 0.5));
    //sp_m = glm::rotate(sp_m, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
    //sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.5, 0.0));
    //sp_model->matrix = sp_m * sp_model->matrix;
    //models.push_back(sp_model);

    for (const auto& model : models) {
        if (model->name == "sierpinski") {
            opaqueModels.push_back(model);
            continue;
        }
        bool isTransparent = false;
        for (const auto& [materialName, material] : model->materials) {
            if (material.d < 1.0f) { // 투명 재질 여부 확인
                isTransparent = true;
                break;
            }
        }
        if (isTransparent) {
            transparentModels.push_back(model);
        }
        else {
            opaqueModels.push_back(model);
        }
    }

    // 디버깅 출력
    /*debug_model(models.back());
    debug_materials(models.back()->materials);*/

    initializeModelsWithPhysics(models); // 모든 모델 Bullet world에 추가

    InitBuffer();

    // 초기 프레임 강제 렌더링
    drawScene();  // 디스플레이 콜백 함수 직접 호출

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(keySpecial);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();

    return 0;
}

GLvoid drawScene() {

    glClearColor(0.0, 0.0, 0.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glUseProgram(shaderProgramID);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error in glUseProgram: " << error << std::endl;
    }

    view = glm::mat4(1.0f);
    view = glm::lookAt(
        cameraPos,
        cameraDirection,
        cameraUp
    );
    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 300.0f);
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
    GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    if (keyState['m']) {
        GLint lightEnabledLoc = glGetUniformLocation(shaderProgramID, "lightEnabled");
        glUniform1i(lightEnabledLoc, 1); // true: 1, false: 0으로 전달

    }
    else {
        GLint lightEnabledLoc = glGetUniformLocation(shaderProgramID, "lightEnabled");
        glUniform1i(lightEnabledLoc, 0); // true: 1, false: 0으로 전달
    }


    // **투명 모델 정렬 (카메라와의 거리 기준)**
    std::sort(transparentModels.begin(), transparentModels.end(), [](Model* a, Model* b) {
        float distanceA = calculateDistanceToCamera(a, cameraPos);
        float distanceB = calculateDistanceToCamera(b, cameraPos);
        return distanceA > distanceB; // 멀리 있는 모델을 먼저 렌더링
        });

    // 깊이 테스트 활성화
    glEnable(GL_DEPTH_TEST);

    // 1. 불투명 모델 렌더링
    for (const auto& model : opaqueModels) {
        model->draw(shaderProgramID, isKeyPressed_s);
    }

    // 2. 깊이 버퍼 쓰기 비활성화
    glDepthMask(GL_FALSE);

    // 블렌딩 활성화
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 3. 투명 모델 렌더링 (정렬된 순서로)
    for (const auto& model : transparentModels) {
        model->draw(shaderProgramID, isKeyPressed_s);
    }

    // 블렌딩 비활성화
    glDisable(GL_BLEND);

    // 깊이 버퍼 쓰기 다시 활성화
    glDepthMask(GL_TRUE);

    // 깊이 테스트 비활성화
    glDisable(GL_DEPTH_TEST);

    //for (const auto& model : models) { // 모델 bb draw
    //    model->draw_rigidBody(shaderProgramID);
    //}

    glutSwapBuffers();

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;
    }
}

// 버퍼 초기화 함수
void InitBuffer() {
    //-----------------------------------------------------------------------------------------------------------
    for (const auto& model : models) { // 모든 모델 initBuffer
        model->initBuffer();
    }
}

float calculateDistanceToCamera(const Model* model, const glm::vec3& cameraPosition) {
    // 모델 중심을 계산 (여기서는 월드 좌표계를 기준으로 계산)
    glm::vec3 modelPosition = glm::vec3(model->matrix[3]); // 모델 행렬의 번지 [3]은 위치
    return glm::distance(cameraPosition, modelPosition);  // 카메라와의 거리
}