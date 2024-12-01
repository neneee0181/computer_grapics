#include <iostream>
#include <vector> 

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


void timer(int value){

    glm::mat4 matrix = glm::mat4(1.0f);
    for (auto& model : models) {
        if (keyState['x']) {
            matrix = glm::rotate(matrix, glm::radians(0.5f), glm::vec3(1.0, 0.0, 0.0));
            model->matrix = matrix * model->matrix;
        }
        else if (keyState['X']) {
            matrix = glm::rotate(matrix, glm::radians(-0.5f), glm::vec3(1.0, 0.0, 0.0));
            model->matrix = matrix * model->matrix;
        }
        else if (keyState['y']) {
            matrix = glm::rotate(matrix, glm::radians(0.5f), glm::vec3(0.0, 1.0, 0.0));
            model->matrix = matrix * model->matrix;
        }
        else if (keyState['Y']) {
            matrix = glm::rotate(matrix, glm::radians(-0.5f), glm::vec3(0.0, 1.0, 0.0));
            model->matrix = matrix * model->matrix;
        }
    }
   
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
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

    loadModelWithProgress <DefaultModel>("pira.obj", "obj/", "pira", "sphere", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), models);
    loadModelWithProgress <DefaultModel>("squ.obj", "obj/", "squ", "sphere", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), models);

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
    glutTimerFunc(0, timer, 0);
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
   
    glEnable(GL_DEPTH_TEST);
    for (const auto& model : models) { // 실제 모델 draw
        if (keyState['c']) {
            if (model->name == "pira") {
                model->draw(shaderProgramID, isKeyPressed_s);
            }
        }
        else if (keyState['p']) {
            if (model->name == "squ") {
                model->draw(shaderProgramID, isKeyPressed_s);
            }
        }
    }
    glDisable(GL_DEPTH_TEST);

    for (const auto& model : models) { // 모델 bb draw
        model->draw_rigidBody(shaderProgramID);
    }

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