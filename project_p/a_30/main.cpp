#include <iostream>
#include <vector> 
#include <algorithm>  // std::sort 사용을 위한 헤더

#include "shaderMaker.h"
#include "DefaultModel.h"
#include "KeyBoard.h"
#include "Camera.h"
#include "Light.h"
#include "Debug.h"
#include "LoadProgress.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
float calculateDistanceToCamera(const Model* model, const glm::vec3& cameraPosition);

//모델
vector<Model*> models;

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

    loadModelWithProgress <DefaultModel>("squ.obj", "obj/", "squ", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), models);
    loadModelWithProgress <DefaultModel>("pira.obj", "obj/", "pira", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 15.0)), glm::vec3(1.0, 1.0, 1.0)), models);

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
    glutMainLoop(); 

    return 0;
}

GLvoid drawScene() {

    glClearColor(1.0, 1.0, 1.0, 1.0f);
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

    // **투명 모델 정렬 (카메라와의 거리 기준)**
    std::sort(models.begin(), models.end(), [](Model* a, Model* b) {
        float distanceA = calculateDistanceToCamera(a, cameraPos); // 글로벌 변수 접근
        float distanceB = calculateDistanceToCamera(b, cameraPos); // 글로벌 변수 접근
        return distanceA > distanceB;
        });

    glEnable(GL_DEPTH_TEST);
    for (const auto& model : models) { // 실제 모델 draw
        model->draw(shaderProgramID, isKeyPressed_s);
    }
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