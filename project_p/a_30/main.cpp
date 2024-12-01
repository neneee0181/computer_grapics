#include <iostream>
#include <vector> 
#include <algorithm>  // std::sort ����� ���� ���

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

//��
vector<Model*> models;
std::vector<Model*> opaqueModels;
std::vector<Model*> transparentModels;

int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);  // ������� ���� �ػ�
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT); // ������� ���� �ػ�
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 600); // â ũ�⸦ ȭ�� ũ��� ����
    glutCreateWindow("opengl template v16");
    //glutFullScreen(); // ��ü ȭ������ ��ȯ

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    make_shaderProgram();

    initPhysics(); // Bullet �ʱ�ȭ �Լ� ȣ��

    loadModelWithProgress <DefaultModel>("squ.obj", "obj/", "squ", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), models);
    loadModelWithProgress <DefaultModel>("pira.obj", "obj/", "pira", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 15.0)), glm::vec3(1.0, 1.0, 1.0)), models);

    for (const auto& model : models) {
        bool isTransparent = false;
        for (const auto& [materialName, material] : model->materials) {
            if (material.d < 1.0f) { // ���� ���� ���� Ȯ��
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

    // ����� ���
    /*debug_model(models.back());
    debug_materials(models.back()->materials);*/

    initializeModelsWithPhysics(models); // ��� �� Bullet world�� �߰�

    InitBuffer();

    // �ʱ� ������ ���� ������
    drawScene();  // ���÷��� �ݹ� �Լ� ���� ȣ��

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

    // **���� �� ���� (ī�޶���� �Ÿ� ����)**
    std::sort(transparentModels.begin(), transparentModels.end(), [](Model* a, Model* b) {
        float distanceA = calculateDistanceToCamera(a, cameraPos);
        float distanceB = calculateDistanceToCamera(b, cameraPos);
        return distanceA > distanceB; // �ָ� �ִ� ���� ���� ������
        });

    // ���� �׽�Ʈ Ȱ��ȭ
    glEnable(GL_DEPTH_TEST);

    // 1. ������ �� ������
    for (const auto& model : opaqueModels) {
        model->draw(shaderProgramID, isKeyPressed_s);
    }

    // 2. ���� ���� ���� ��Ȱ��ȭ
    glDepthMask(GL_FALSE);

    // ���� Ȱ��ȭ
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 3. ���� �� ������ (���ĵ� ������)
    for (const auto& model : transparentModels) {
        model->draw(shaderProgramID, isKeyPressed_s);
    }

    // ���� ��Ȱ��ȭ
    glDisable(GL_BLEND);

    // ���� ���� ���� �ٽ� Ȱ��ȭ
    glDepthMask(GL_TRUE);

    // ���� �׽�Ʈ ��Ȱ��ȭ
    glDisable(GL_DEPTH_TEST);

    //for (const auto& model : models) { // �� bb draw
    //    model->draw_rigidBody(shaderProgramID);
    //}

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

float calculateDistanceToCamera(const Model* model, const glm::vec3& cameraPosition) {
    // �� �߽��� ��� (���⼭�� ���� ��ǥ�踦 �������� ���)
    glm::vec3 modelPosition = glm::vec3(model->matrix[3]); // �� ����� ���� [3]�� ��ġ
    return glm::distance(cameraPosition, modelPosition);  // ī�޶���� �Ÿ�
}