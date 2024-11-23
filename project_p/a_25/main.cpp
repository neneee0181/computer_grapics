#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector> 
#include <unordered_map>
#include <random>

#include "shaderMaker.h"
#include "LoadWall.h"
#include "LoadBody.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

//카메라
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 70);
glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

//조명
glm::vec3 lightPos = glm::vec3(15.0, 0.0, 0.0);

vector<Model> models;

//키
unordered_map<char, bool> keyState;

void keyDown_s(const char& key) {
    keyState[key] = !keyState[key];
}

void keyUp_s(const char& key) {
    keyState[key] = false;
}

bool isKeyPressed_s(const char& key) {
    return keyState[key];
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    width = w;
    height = h;
}

void keyUp(unsigned char key, int x, int y) {
    //keyUp_s(key);
    glutPostRedisplay();
}

void timer(int value) {

    //y -> 자전
    if (keyState['y']) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::rotate(matrix, glm::radians(0.3f), glm::vec3(0.0, 1.0, 0.0));
        SQU::models[0].modelMatrix = matrix * SQU::models[0].modelMatrix;
        PIRA::models[0].modelMatrix = matrix * PIRA::models[0].modelMatrix;
    }

    if (keyState['r']) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(-SQU::models[0].modelMatrix[3].x, 0.0, 0.0));
        matrix = glm::rotate(matrix, glm::radians(0.3f), glm::vec3(0.0, 1.0, 0.0));
        matrix = glm::translate(matrix, glm::vec3(SQU::models[0].modelMatrix[3].x, 0.0, 0.0));

        // 조명 위치 업데이트
        lightPos = glm::vec3(matrix * glm::vec4(lightPos, 1.0));

        SQU::models[1].modelMatrix = matrix * SQU::models[1].modelMatrix;
        PIRA::models[1].modelMatrix = matrix * PIRA::models[1].modelMatrix;
    }


    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyDown(unsigned char key, int x, int y) {

    keyDown_s(key);

    switch (key)
    {
    case 'z':
    {
        keyState['r'] = false;
        float x = SQU::models[1].modelMatrix[3].x;
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(x - 0.05, 0.0, 0.0));
        SQU::models[1].modelMatrix = matrix;
        PIRA::models[1].modelMatrix = matrix;
        lightPos = glm::vec3(glm::vec4(glm::vec3(PIRA::models[1].modelMatrix[3]), 1.0));
        break;
    }
    case 'Z':
    {
        keyState['r'] = false;
        float x = SQU::models[1].modelMatrix[3].x;
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(x + 0.05, 0.0, 0.0));
        SQU::models[1].modelMatrix = matrix;
        PIRA::models[1].modelMatrix = matrix;
        lightPos = glm::vec3(glm::vec4(glm::vec3(PIRA::models[1].modelMatrix[3]), 1.0));

        break;
    }
    case 'q':
        cout << " 프로그램 종료 " << endl;
        exit(0);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void keySpecial(int key, int x, int y) {

    glm::mat4 rotationMatrix;
    if (key == GLUT_KEY_LEFT) {
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (key == GLUT_KEY_RIGHT) {
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (key == GLUT_KEY_UP) {
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    else if (key == GLUT_KEY_DOWN) {
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    else {
        return;
    }

    cameraPos = glm::vec3(rotationMatrix * glm::vec4(cameraPos, 1.0f));

    glutPostRedisplay();
}

int main(int argc, char** argv) {

    width = 800;
    height = 600;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("24번실습");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    make_shaderProgram();

    initPhysics(); // Bullet 초기화 함수 호출

   
    PIRA::load_obj(); // 몸 obj 불러옴
    SQU::load_obj(); // 벽 obj 불러옴

    initializeModelsWithPhysics(PIRA::models);

    InitBuffer();

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

    if (keyState['m']) {
        GLint lightEnabledLoc = glGetUniformLocation(shaderProgramID, "lightEnabled");
        glUniform1i(lightEnabledLoc, 1); // true: 1, false: 0으로 전달

    }
    else {
        GLint lightEnabledLoc = glGetUniformLocation(shaderProgramID, "lightEnabled");
        glUniform1i(lightEnabledLoc, 0); // true: 1, false: 0으로 전달
    }
    GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
    GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    GLint objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(0.6f, 0.65f, 0.6f)));
    glUniform3f(objColorLocation, 1.0, 0.5, 0.3);
   
    glEnable(GL_DEPTH_TEST);
    if (keyState['n'])
        PIRA::draw(shaderProgramID, isKeyPressed_s);
    else
        SQU::draw(shaderProgramID, isKeyPressed_s);

    glDisable(GL_DEPTH_TEST);

    //Body::draw_rigidBody(shaderProgramID);
    //PIRA::draw_rigidBody(shaderProgramID);

    glutSwapBuffers();

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;
    }
}

// 버퍼 초기화 함수
void InitBuffer() {
    //-----------------------------------------------------------------------------------------------------------
    SQU::initBuffer();
    PIRA::initBuffer();
}