#include <iostream>   // ǥ�� ����� ���̺귯�� ����
#include <gl/glew.h>  // GLEW ���̺귯�� ���� (OpenGL Ȯ�� ����� ����ϱ� ���� �ʿ�)
#include <gl/freeglut.h>  // GLUT ���̺귯�� ���� (������ â �� ����� �Է� ó���� ���� ���)
#include <gl/freeglut_ext.h>  // GLUT Ȯ�� ��� ����
#include <gl/glm/glm/glm.hpp>  // GLM ���̺귯�� ���� (�������� ����� ���� ���� ���̺귯��)
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector>  // ǥ�� ���� ���̺귯�� ���� (���� �迭�� ����ϱ� ���� �ʿ�)

#include "LoadObj.h"
#include "LoadTexture.h"
#include "shaderMaker.h"

using namespace std;  // ���ӽ����̽� std ������� �ڵ� ������ std:: ���� ����

// �Լ� �����
void InitBuffer();  // ���� �ʱ�ȭ �Լ� ����
GLvoid drawScene(GLvoid);  // ȭ���� �׸��� �Լ� ����
GLvoid Reshape(int w, int h);  // ȭ�� ũ�Ⱑ ����� �� ȣ��Ǵ� �Լ� ����

// ���� 3D ���� �����ϴ� ����
vector<Model> models;
vector<GLuint> vaos;
vector<vector<GLuint>> vbos;

const Vertex x[2] = {
    {-0.8f,0.0f, 0.0f},
    {0.8f,0.0f, 0.0f}
};

const Vertex y[2] = {
    {0.0f,-0.8f, 0.0f},
    {0.0f, 0.8f, 0.0f}
};

const Vertex z[2] = {
    {0.0f, 0.0f, -0.8f},
    {0.0f, 0.0f, 0.8f}
};

// ȭ�� ũ�Ⱑ ����� �� ȣ��Ǵ� �Լ�
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);  // ����Ʈ ũ�� ����
}

int currentSpiralIndex1 = 0;  // 1�� ������ ���� �����̷� �ε���
int currentSpiralIndex2 = 0;  // 2�� ������ ���� �����̷� �ε���

int Direction1 = 1;  // 1�� ������ �����̷� ���� ���� (1: ������, -1: �ڷ�)
int Direction2 = -1;  // 2�� ������ �����̷� ���� ���� (1: ������, -1: �ڷ�)

bool startModel2 = false;  // 2�� ������ �����̷��� ���� �����ߴ��� ����

vector<Vertex> spirals_v;

char key_result = ' ';
bool isTimerRunning = false;  // Ÿ�̸Ӱ� ���� ������ ���θ� �����ϴ� �÷���

glm::vec3 f_f1;
glm::vec3 f_f2;

void timer(int value) {

    if (!isTimerRunning) {
        return;  // Ÿ�̸Ӱ� ���߸� �� �̻� �������� ����
    }


    if (key_result == '1') {
        // 1�� ���� �����̷� �̵� ó��
        currentSpiralIndex1 += Direction1;

        // 1�� ������ �����̷� ���� �������� �� ���� ��ȯ
        if (currentSpiralIndex1 >= spirals_v.size() || currentSpiralIndex1 < 0) {
            Direction1 *= -1;
            currentSpiralIndex1 += Direction1;
        }

        // �����̷� ����� 1�� ���� ��ġ
        Vertex currentPos1 = spirals_v[currentSpiralIndex1];
        float scaleFactor = 3.5f;  // �����̷� ������ Ȯ��
        glm::vec3 scaledPos1 = glm::vec3(currentPos1.x * scaleFactor, currentPos1.y * scaleFactor, currentPos1.z * scaleFactor);

        // 1�� ������ ��ȯ ����
        glm::mat4 transformationMatrix1 = models[0].initialRotation;  // �ʱ� ȸ�� ����
        transformationMatrix1 = glm::scale(transformationMatrix1, glm::vec3(0.2f, 0.2f, 0.2f));  // ������
        transformationMatrix1 = glm::translate(transformationMatrix1, glm::vec3(spirals_v[0].x, spirals_v[0].y, spirals_v[0].z));
        transformationMatrix1 = glm::translate(transformationMatrix1, scaledPos1);  // ��ġ ����
        models[0].modelMatrix = transformationMatrix1;

        // 2�� ���� ���� ���� Ȯ�� (2�� �� ����)
        if (startModel2) {
            currentSpiralIndex2 += Direction2;

            // 2�� ������ �����̷� ���� �������� �� ���� ��ȯ
            if (currentSpiralIndex2 >= spirals_v.size() || currentSpiralIndex2 < 0) {
                Direction2 *= -1;
                currentSpiralIndex2 += Direction2;
            }

            // �����̷� ����� 2�� ���� ��ġ
            Vertex currentPos2 = spirals_v[currentSpiralIndex2];
            glm::vec3 scaledPos2 = glm::vec3(currentPos2.x * scaleFactor, currentPos2.y * scaleFactor, currentPos2.z * scaleFactor);

            // 2�� ������ ��ȯ ����
            glm::mat4 transformationMatrix2 = models[1].initialRotation;  // �ʱ� ȸ�� ����
            transformationMatrix2 = glm::scale(transformationMatrix2, glm::vec3(0.2f, 0.2f, 0.2f));  // ������
            transformationMatrix2 = glm::translate(transformationMatrix2, glm::vec3(spirals_v[0].x, spirals_v[0].y, spirals_v[0].z));
            transformationMatrix2 = glm::translate(transformationMatrix2, scaledPos2);  // ��ġ ����
            models[1].modelMatrix = transformationMatrix2;
        }
    }
    else if (key_result == '2') {
        float speed = 0.1f;

       

    }
    else if (key_result == '3') {
        // 1�� ������ ��ġ�� 0�� ������ �̵�
        
        glm::vec3 targetPosition1 = f_f2 - glm::vec3(models[0].modelMatrix[3]);  // 1�� ������ ��ġ ����
        models[0].modelMatrix = glm::translate(models[0].modelMatrix, targetPosition1);

        glm::vec3 targetPosition2 = f_f1 - glm::vec3(models[1].modelMatrix[3]);  // 1�� ������ ��ġ ����
        models[1].modelMatrix = glm::translate(models[1].modelMatrix, targetPosition2);
        
    }

    glutPostRedisplay();  // ȭ�� �ٽ� �׸��� ��û
    glutTimerFunc(16, timer, value);  // Ÿ�̸� �缳�� (�� 60 FPS)
}

// 2�� ���� ���� Ÿ�̸� (2�� �� ����)
void startModel2Timer(int value) {
    startModel2 = true;  // 2�� ���� �����̷� ����
}

void makeSpiral() {
    spirals_v.clear();  // ���� �����͸� �ʱ�ȭ

    float radiusIncreaseRate = 0.001f;  // ������ ������
    float angleStep = 0.1f;  // ���� ������ (����)
    int numTurns = 6;  // �����̷��� ȸ�� �� (�� ���� �� ������)
    int numPointsPerTurn = 100;  // �� ���� �� ������ ���� ��
    float initialRadius = 0.0f;  // �ʱ� ������

    for (int i = 0; i < numTurns * numPointsPerTurn; ++i) {
        float angle = i * angleStep;  // ���� ���
        float radius = initialRadius + i * radiusIncreaseRate;  // ���� ������ ���

        float x = radius * cos(angle);  // x ��ǥ
        float z = radius * sin(angle);  // z ��ǥ

        spirals_v.push_back({ x, 0.0f, z });  // XZ ��鿡 �� �߰�
    }
}

void keyBoard(unsigned char key, int x, int y) {
    switch (key)
    {
    case '1':
    {
        isTimerRunning = !isTimerRunning;

        key_result = '1';
        makeSpiral();
        Model modelSpiral;
        modelSpiral.name = "spiral";
        modelSpiral.vertices = spirals_v;
        modelSpiral.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
        modelSpiral.modelMatrix = glm::rotate(modelSpiral.modelMatrix, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0));
        modelSpiral.modelMatrix = glm::rotate(modelSpiral.modelMatrix, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0));

        models.push_back(modelSpiral);

        InitBuffer();

        // 1�� ������ �ٷ� ����
        glutTimerFunc(0, timer, 0);
        if (currentSpiralIndex2 == 0)
            currentSpiralIndex2 = spirals_v.size();
        // 2�� ������ 2�� �Ŀ� ����
        glutTimerFunc(0, startModel2Timer, 0);
        break;
    }
    case '2':
    {
        isTimerRunning = !isTimerRunning;
        key_result = '2';



        for (int i = 0; i < 2; ++i) {
            glm::mat4 transformationMatrix1 = models[i].initialRotation;  // �ʱ� ȸ�� ����
            transformationMatrix1 = glm::scale(transformationMatrix1, glm::vec3(0.2f, 0.2f, 0.2f));  // ������
            transformationMatrix1 = glm::translate(transformationMatrix1, models[i].translationOffset);
            models[i].modelMatrix = transformationMatrix1;
        }

        glutTimerFunc(0, timer, 0);

        break;
    }
    case '3':
        isTimerRunning = !isTimerRunning;
        key_result = '3';

        for (int i = 0; i < 2; ++i) {
            glm::mat4 transformationMatrix1 = models[i].initialRotation;  // �ʱ� ȸ�� ����
            transformationMatrix1 = glm::scale(transformationMatrix1, glm::vec3(0.2f, 0.2f, 0.2f));  // ������
            transformationMatrix1 = glm::translate(transformationMatrix1, models[i].translationOffset);
            models[i].modelMatrix = transformationMatrix1;
        }

        f_f1 = models[0].modelMatrix[3];
        f_f2 = models[1].modelMatrix[3];

        glutTimerFunc(0, timer, 0);
        break;
    default:
        break;
    }
    glutPostRedisplay();  // ȭ�� �ٽ� �׸��� ��û
}

// ���콺 �Է��� ó���ϴ� �Լ�
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        std::cout << "x = " << x << " y = " << y << std::endl;  // ���콺 Ŭ�� ��ǥ ���
    glutPostRedisplay();  // Ű���� �Է� �� ȭ���� �ٽ� �׸����� ��û
}

// ���� �Լ�
int main(int argc, char** argv) {

    width = 800;  // â�� �⺻ �ʺ� ����
    height = 600;  // â�� �⺻ ���� ����

    // ������ ����
    glutInit(&argc, argv);  // GLUT �ʱ�ȭ
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);  // ȭ�� ��� ���� (���� ���۸�, RGBA �÷� ���)
    glutInitWindowPosition(100, 100);  // â�� ���� ��ġ ����
    glutInitWindowSize(width, height);  // â�� ũ�� ����
    glutCreateWindow("template");  // â ���� �� ���� ����

    // GLEW �ʱ�ȭ
    glewExperimental = GL_TRUE;  // GLEW ������ ��� Ȱ��ȭ
    if (glewInit() != GLEW_OK) {  // GLEW �ʱ�ȭ �� ���� üũ
        cerr << "Unable to initialize GLEW" << endl;  // GLEW �ʱ�ȭ ���� �� ���� �޽��� ���
        exit(EXIT_FAILURE);  // ���α׷� ����
    }
    else
        cout << "GLEW Initialized\n";  // �ʱ�ȭ ���� �� �޽��� ���

    make_shaderProgram();  // ���̴� ���α׷� ����
        
    // �𵨵��� �ε��ϰ� ���Ϳ� �߰�
    Model modelSqu, modelSphere;

    // ������ü
    read_obj_file("box3.obj", modelSqu, "cube");
    modelSqu.initialRotation = glm::mat4(1.0f);  // �ʱ� ȸ�� ����� ���� ��ķ� ����
    modelSqu.initialRotation = glm::rotate(modelSqu.initialRotation, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0)); // X�� ȸ��
    modelSqu.initialRotation = glm::rotate(modelSqu.initialRotation, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0)); // Y�� ȸ��
    modelSqu.modelMatrix = modelSqu.initialRotation;
    modelSqu.modelMatrix = glm::scale(modelSqu.modelMatrix, glm::vec3(0.2, 0.2, 0.2));
    modelSqu.translationOffset = glm::vec3(-3.0f, 0.0f, 0.0f);
    modelSqu.modelMatrix = glm::translate(modelSqu.modelMatrix, modelSqu.translationOffset);
    modelSqu.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelSqu);

    read_obj_file("sphere.obj", modelSphere, "sphere");
    modelSphere.initialRotation = glm::mat4(1.0f);  // �ʱ� ȸ�� ����� ���� ��ķ� ����
    modelSphere.initialRotation = glm::rotate(modelSphere.initialRotation, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0)); // X�� ȸ��
    modelSphere.initialRotation = glm::rotate(modelSphere.initialRotation, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0)); // Y�� ȸ��
    modelSphere.modelMatrix = modelSphere.initialRotation;
    modelSphere.modelMatrix = glm::scale(modelSphere.modelMatrix, glm::vec3(0.2, 0.2, 0.2));
    modelSphere.translationOffset = glm::vec3(3.0f, 0.0f, 0.0f);
    modelSphere.modelMatrix = glm::translate(modelSphere.modelMatrix, modelSphere.translationOffset);
    modelSphere.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelSphere);

    Model modelXLine, modelYLine, modelZLine;
    modelXLine.name = "xLine";
    modelXLine.vertices.push_back(x[0]);
    modelXLine.vertices.push_back(x[1]);
    modelXLine.colors.push_back(glm::vec3(1.0, 0.0, 0.0));
    modelXLine.modelMatrix = glm::rotate(modelXLine.modelMatrix, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0));
    modelXLine.modelMatrix = glm::rotate(modelXLine.modelMatrix, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0));

    modelYLine.name = "yLine";
    modelYLine.vertices.push_back(y[0]);
    modelYLine.vertices.push_back(y[1]);
    modelYLine.colors.push_back(glm::vec3(0.0, 1.0, 0.0));
    modelYLine.modelMatrix = glm::rotate(modelYLine.modelMatrix, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0));
    modelYLine.modelMatrix = glm::rotate(modelYLine.modelMatrix, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0));

    modelZLine.name = "zLine";
    modelZLine.vertices.push_back(z[0]);
    modelZLine.vertices.push_back(z[1]);
    modelZLine.colors.push_back(glm::vec3(0.0, 0.0, 1.0));
    modelZLine.modelMatrix = glm::rotate(modelZLine.modelMatrix, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0));
    modelZLine.modelMatrix = glm::rotate(modelZLine.modelMatrix, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0));

    models.push_back(modelXLine);
    models.push_back(modelYLine);
    models.push_back(modelZLine);

    InitBuffer();  // ���� �ʱ�ȭ

    // �ݹ� �Լ� ���
    glutDisplayFunc(drawScene);  // ȭ���� �׸��� �Լ� ���
    glutReshapeFunc(Reshape);  // ȭ�� ũ�� ���� �� ȣ��Ǵ� �Լ� ���
    glutKeyboardFunc(keyBoard);  // Ű���� �Է� ó�� �Լ� ���
    glutMouseFunc(mouse);  // ���콺 �Է� ó�� �Լ� ���
    glutMainLoop();  // �̺�Ʈ ���� ����

    return 0;  // ���α׷� ���� ����
}

// ȭ���� �׸��� �Լ�
GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);  // ȭ���� ������� �ʱ�ȭ
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // �÷� ���ۿ� ���� ���� �ʱ�ȭ

    glUseProgram(shaderProgramID);  // ���̴� ���α׷� ���


    glEnable(GL_DEPTH_TEST);
    // �� ���� �׸���
    for (size_t i = 0; i < models.size(); ++i) {
        glBindVertexArray(vaos[i]);

        GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");

        if (models[i].name == "cube" || models[i].name == "sphere") {
            GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            // �� �׸���
            glUniform1i(modelStatus, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
        }
        else if (models[i].name == "xLine" || models[i].name == "yLine" || models[i].name == "zLine") {
            GLint lineLoc = glGetUniformLocation(shaderProgramID, "Line");
            glUniformMatrix4fv(lineLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            glUniform1i(modelStatus, 1);
            glLineWidth(1.0f);
            glDrawArrays(GL_LINES, 0, 2);
        }
        else if (models[i].name == "spiral" && key_result == '1') {
            GLint lineLoc = glGetUniformLocation(shaderProgramID, "Line");
            glUniformMatrix4fv(lineLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            glUniform1i(modelStatus, 1);
            glLineWidth(1.0f);
            glDrawArrays(GL_LINE_STRIP, 0, models[i].vertices.size());
        }

        glBindVertexArray(0);
    }

    glDisable(GL_DEPTH_TEST);

    glutSwapBuffers();  // ���� ���۸� ���, ȭ�� ����

    // OpenGL ���� üũ ����
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;  // ���� �޽��� ���
    }
}

// ���� �ʱ�ȭ �Լ�
void InitBuffer() {

    // �� �𵨿� ���� VAO, VBO, EBO ����
    vaos.resize(models.size());
    vbos.resize(models.size(), vector<GLuint>(4)); // �𵨸��� 4���� VBO�� �ʿ� (����, ����, ����, �ؽ�ó ��ǥ)

    for (size_t i = 0; i < models.size(); ++i) {
        glGenVertexArrays(1, &vaos[i]);
        glBindVertexArray(vaos[i]);

        // VBO ����
        glGenBuffers(4, vbos[i].data());

        // ���� ���� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][0]);
        glBufferData(GL_ARRAY_BUFFER, models[i].vertices.size() * sizeof(glm::vec3), models[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0�� ���� �Ҵ�
        glEnableVertexAttribArray(0);

        // ���� ���� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][1]);
        glBufferData(GL_ARRAY_BUFFER, models[i].colors.size() * sizeof(glm::vec3), models[i].colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1�� ���� �Ҵ�
        glEnableVertexAttribArray(1);

        // �� �ε��� ������ (EBO) ����
        vector<unsigned int> indices;
        for (const Face& face : models[i].faces) {
            indices.push_back(face.v1);
            indices.push_back(face.v2);
            indices.push_back(face.v3);
        }

        GLuint ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }
}
