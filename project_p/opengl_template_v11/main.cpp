#include <iostream>
#include <vector> 

#include "shaderMaker.h"
#include "DefaultModel.h"
#include "KeyBoard.h"
#include "Camera.h"
#include "Light.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

//��
vector<Model*> models;

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

    initPhysics(); // Bullet �ʱ�ȭ �Լ� ȣ��

    DefaultModel* default_model = new DefaultModel("obj/straight_road.obj", "straight_road", "sphere", glm::scale(glm::mat4(1.0f), glm::vec3(10.0, 10.0, 10.0))); // ���� �� ��������
    models.push_back(default_model);

    initializeModelsWithPhysics(models); // ��� �� Bullet world�� �߰�

    InitBuffer();   

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
   
    glEnable(GL_DEPTH_TEST);
    for (const auto& model : models) { // ���� �� draw
        model->draw(shaderProgramID, isKeyPressed_s);
    }
    glDisable(GL_DEPTH_TEST);

    for (const auto& model : models) { // �� bb draw
        model->draw_rigidBody(shaderProgramID);
    }

    glutSwapBuffers();

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;
    }
}

// ���� �ʱ�ȭ �Լ�
void InitBuffer() {
    //-----------------------------------------------------------------------------------------------------------
    for (const auto& model : models) { // ��� �� initBuffer
        model->initBuffer();
    }
}