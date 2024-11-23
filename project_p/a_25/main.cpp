#include <iostream>
#include <vector> 
#include <random>

#include "shaderMaker.h"
#include "DefaultModel.h"
#include "KeyBoard.h"
#include "Camera.h"
#include "Light.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

//모델
vector<Model*> models;

void timer(int value) {
    
    if (keyState['r']) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(-30.0, 0.0, -40.0));
        matrix = glm::rotate(matrix, glm::radians(0.5f), glm::vec3(0.0, 1.0, 0.0));
        matrix = glm::translate(matrix, glm::vec3(30.0, 0.0, 40.0));
        lightPos = matrix * glm::vec4(lightPos, 1.0f);
        models[3]->matrix = matrix * models[3]->matrix;
    }
    else if (keyState['R']) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(-30.0, 0.0, -40.0));
        matrix = glm::rotate(matrix, glm::radians(-0.5f), glm::vec3(0.0, 1.0, 0.0));
        matrix = glm::translate(matrix, glm::vec3(30.0, 0.0, 40.0));
        lightPos = matrix * glm::vec4(lightPos, 1.0f);
        models[3]->matrix = matrix * models[3]->matrix;
    }

    UpdateRigidBodyTransforms(models);

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv) {

    width = 800;
    height = 600;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 200);
    glutInitWindowSize(width, height);
    glutCreateWindow("opengl template v8");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    make_shaderProgram();

    initPhysics(); // Bullet 초기화 함수 호출

    DefaultModel* default_model1 = new DefaultModel("obj/sphere.obj", "sphere1", "sphere"); // 실제 모델 가져오기
    default_model1->material.Ka = glm::vec3(1.0, 0.0, 0.0);
    DefaultModel* default_model2 = new DefaultModel("obj/sphere.obj", "sphere1", "sphere", glm::translate(glm::mat4(1.0f), glm::vec3(-30.0, 0.0, -40.0)));
    default_model2->material.Ka = glm::vec3(0.0, 1.0, 0.0);
    DefaultModel* default_model3 = new DefaultModel("obj/sphere.obj", "sphere1", "sphere", glm::translate(glm::mat4(1.0f), glm::vec3(-60.0, 0.0, -80.0)));
    default_model3->material.Ka = glm::vec3(0.0, 0.0, 1.0);

    DefaultModel* default_model4 = new DefaultModel("obj/sphere.obj", "sphere1", "sphere", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2)), lightPos));
    default_model4->material.Ka = lightColor;

    models.push_back(default_model1);
    models.push_back(default_model2);
    models.push_back(default_model3);
    models.push_back(default_model4);

    initializeModelsWithPhysics(models); // 모든 모델 Bullet world에 추가

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
   
    glEnable(GL_DEPTH_TEST);
    for (const auto& model : models) { // 실제 모델 draw
        model->draw(shaderProgramID, isKeyPressed_s);
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