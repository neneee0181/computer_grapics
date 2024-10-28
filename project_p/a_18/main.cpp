#include <iostream>   // ǥ�� ����� ���̺귯�� ����
#include <gl/glew.h>  // GLEW ���̺귯�� ���� (OpenGL Ȯ�� ����� ����ϱ� ���� �ʿ�)
#include <gl/freeglut.h>  // GLUT ���̺귯�� ���� (������ â �� ����� �Է� ó���� ���� ���)
#include <gl/freeglut_ext.h>  // GLUT Ȯ�� ��� ����
#include <gl/glm/glm/glm.hpp>  // GLM ���̺귯�� ���� (�������� ����� ���� ���� ���̺귯��)
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector>  // ǥ�� ���� ���̺귯�� ���� (���� �迭�� ����ϱ� ���� �ʿ�)
#include <random>
#include <unordered_map>

#include "LoadObj.h"
#include "shaderMaker.h"

using namespace std;  // ���ӽ����̽� std ������� �ڵ� ������ std:: ���� ����

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis_color(0.0f, 1.0f);

// �Լ� �����
void InitBuffer();  // ���� �ʱ�ȭ �Լ� ����
GLvoid drawScene(GLvoid);  // ȭ���� �׸��� �Լ� ����
GLvoid Reshape(int w, int h);  // ȭ�� ũ�Ⱑ ����� �� ȣ��Ǵ� �Լ� ����

// ���� 3D ���� �����ϴ� ����
vector<Model> models;
vector<GLuint> vaos;
vector<vector<GLuint>> vbos;
GLuint orbitVAO[6], orbitVBO[6];

// camera
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 4.0);
glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

glm::mat4 projection = glm::mat4(1.0f);

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
    width = w;
    height = h;
}

std::vector<std::vector<glm::vec3>> orbitVertices;
int orbitSegments = 100; // �˵� ���� ���� ���׸�Ʈ ��

void createOrbitVertices(float radius, glm::vec3 center, float angleY) {
    std::vector<glm::vec3> singleOrbit;

    // Y���� �������� angleY��ŭ ȸ���ϴ� ����� ����
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 0.0f, 1.0f));

    for (int i = 0; i <= orbitSegments; ++i) {
        float angle = glm::radians((360.0f / orbitSegments) * i);

        // XZ ��鿡 ���� ����
        glm::vec4 point = glm::vec4(radius * cos(angle), 0.0f, radius * sin(angle), 1.0f);

        // ȸ�� ��� ����
        point = rotationMatrix * point;

        // �߽��� �̵� ����
        singleOrbit.push_back(glm::vec3(point) + center);
    }

    orbitVertices.push_back(singleOrbit);
}

unordered_map<unsigned char, bool> keyStates;
char key_result = ' ';
int y_status = 0;
int z_status = 0;
float speed1 = 0.8f;
float speed2 = 1.0f;
float speed3 = 1.2f;
float speed4 = 1.4f;
float speed5 = 1.6f;

void timer_z(int value) {
    if (keyStates['z']) {
        for (int i = 0; i < models.size(); ++i) {
            if (i == 0)
                continue;
            glm::mat4 orbit = glm::mat4(1.0f);
            orbit = glm::rotate(orbit, glm::radians(speed5), glm::vec3(0.0, 1.0, 0.0));
            models[i].modelMatrix = models[i].modelMatrix * orbit;

            
            glm::mat4 lineO = glm::mat4(1.0f);
            lineO = glm::translate(lineO, glm::vec3(models[i].modelMatrix[3]));
            lineO = glm::rotate(lineO, glm::radians(speed5), glm::vec3(0.0, 0.0, 1.0));
            lineO = glm::translate(lineO, glm::vec3(-models[i].modelMatrix[3]));
            if (i == 2 || i == 4 || i == 6) {
                models[i].modelMatrix = lineO * models[i].modelMatrix;
            }
            if (i == 1 || i == 3 || i == 5) {
                for (size_t j = 0; j < orbitVertices[i].size(); ++j) {
                    glm::vec4 rotatedPoint = lineO * glm::vec4(orbitVertices[i][j], 1.0f);
                    orbitVertices[i][j] = glm::vec3(rotatedPoint);
                }
            }
        }

    }
    else {

    }


    glutPostRedisplay();  // ȭ�� �ٽ� �׸��� ��û
    if (z_status == 1) {
        glutTimerFunc(16, timer_z, z_status);
    }
}

void timer_y(int value) {

    //float speed = 0.8f;

    if (keyStates['y']) {
        for (int i = 0; i < models.size(); ++i) {
            switch (i)
            {
            case 1:
            {

                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                for (size_t j = 0; j < orbitVertices[i].size(); ++j) {

                    // vec3�� vec4�� ��ȯ�Ͽ� ȸ�� ����
                    glm::vec4 rotatedPoint = orbit * glm::vec4(orbitVertices[i][j], 1.0f);

                    // ����� �ٽ� vec3�� ��ȯ�Ͽ� ����
                    orbitVertices[i][j] = glm::vec3(rotatedPoint);
                }

                break;
            }
            case 2:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                orbit = glm::translate(orbit, glm::vec3(models[1].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;
                break;
            }
            case 3:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed3), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(speed3), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                break;
            }
            case 4:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed3), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(speed3), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                orbit = glm::translate(orbit, glm::vec3(models[3].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed3), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(-speed3), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[3].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                for (size_t j = 0; j < orbitVertices[3].size(); ++j) {
                    glm::vec4 rotatedPoint = orbit * glm::vec4(orbitVertices[3][j], 1.0f);
                    orbitVertices[3][j] = glm::vec3(rotatedPoint);
                }
                break;
            }
            case 5:
            {
                glm::mat4 orbit = glm::mat4(1.0f);

                // ���� ��ȯ ���� (ȸ��)
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                break;
            }
            case 6:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                orbit = glm::translate(orbit, glm::vec3(models[5].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed1 + 0.08f), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(speed1 + 0.08f), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[5].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                for (size_t j = 0; j < orbitVertices[5].size(); ++j) {
                    glm::vec4 rotatedPoint = orbit * glm::vec4(orbitVertices[5][j], 1.0f);
                    orbitVertices[5][j] = glm::vec3(rotatedPoint);
                }
                break;
            }
            default:
                break;
            }
        }
    }
    else {
        for (int i = 0; i < models.size(); ++i) {
            switch (i)
            {
            case 1:
            {

                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                for (size_t j = 0; j < orbitVertices[i].size(); ++j) {

                    // vec3�� vec4�� ��ȯ�Ͽ� ȸ�� ����
                    glm::vec4 rotatedPoint = orbit * glm::vec4(orbitVertices[i][j], 1.0f);

                    // ����� �ٽ� vec3�� ��ȯ�Ͽ� ����
                    orbitVertices[i][j] = glm::vec3(rotatedPoint);
                }

                break;
            }
            case 2:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                orbit = glm::translate(orbit, glm::vec3(models[1].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;
                break;
            }
            case 3:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed3), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(-speed3), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                break;
            }
            case 4:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed3), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(-speed3), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                orbit = glm::translate(orbit, glm::vec3(models[3].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed3), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(speed3), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[3].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                for (size_t j = 0; j < orbitVertices[3].size(); ++j) {
                    glm::vec4 rotatedPoint = orbit * glm::vec4(orbitVertices[3][j], 1.0f);
                    orbitVertices[3][j] = glm::vec3(rotatedPoint);
                }
                break;
            }
            case 5:
            {
                glm::mat4 orbit = glm::mat4(1.0f);

                // ���� ��ȯ ���� (ȸ��)
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                break;
            }
            case 6:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                orbit = glm::translate(orbit, glm::vec3(models[5].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed1 + 0.08f), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(-speed1 + 0.08f), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[5].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                for (size_t j = 0; j < orbitVertices[5].size(); ++j) {
                    glm::vec4 rotatedPoint = orbit * glm::vec4(orbitVertices[5][j], 1.0f);
                    orbitVertices[5][j] = glm::vec3(rotatedPoint);
                }
                break;
            }
            default:
                break;
            }
        }
    }

    // VBO ������Ʈ
    for (int i = 0; i < 6; ++i) {
        glBindBuffer(GL_ARRAY_BUFFER, orbitVBO[i]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, orbitVertices[i].size() * sizeof(glm::vec3), orbitVertices[i].data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glutPostRedisplay();  // ȭ�� �ٽ� �׸��� ��û
    if (y_status == 1) {
        glutTimerFunc(16, timer_y, y_status);
    }
}

void keyBoard(unsigned char key, int x, int y) {

    keyStates[key] = !keyStates[key];

    if (key == 'm')
        keyStates['M'] = false;
    else if (key == 'M')
        keyStates['m'] = false;

    if (key == 'y') {
        if (y_status == 0) {
            glutTimerFunc(0, timer_y, y_status);
            y_status = 1;
        }
    }
    else if (key == 'Y') {
        if (y_status == 0) {
            glutTimerFunc(0, timer_y, y_status);
            y_status = 1;
        }
    }

    if (key == 'z') {
        if (z_status == 0) {
            glutTimerFunc(0, timer_z, z_status);
            z_status = 1;
        }
    }
    else if (key == 'z') {
        if (z_status == 0) {
            glutTimerFunc(0, timer_z, z_status);
            z_status = 1;
        }
    }

    if (key == 'p') {
        projection = glm::mat4(1.0f);
        projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -100.0f, 100.0f);
    }
    else if (key == 'P') {
        projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 50.0f);
        projection = glm::translate(projection, glm::vec3(0.0, 0.0, -0.5));
    }

    // ----------
    float speed = 0.1f;
    glm::mat4 orbitTransform = glm::mat4(1.0f);

    switch (key)
    {
    case 'w':
    {
        orbitTransform = glm::translate(orbitTransform, glm::vec3(speed, 0.0, 0.0));
        break;
    }
    case 'a':
    {
        orbitTransform = glm::translate(orbitTransform, glm::vec3(0.0, -speed, 0.0));
        break;
    }
    case 's':
    {
        orbitTransform = glm::translate(orbitTransform, glm::vec3(-speed, 0.0, 0.0));
        break;
    }
    case 'd':
    {
        orbitTransform = glm::translate(orbitTransform, glm::vec3(0.0, speed, 0.0));
        break;
    }
    case '+':
    {
        orbitTransform = glm::translate(orbitTransform, glm::vec3(0.0, 0.0, speed));
        break;
    }
    case '-':
    {
        orbitTransform = glm::translate(orbitTransform, glm::vec3(0.0, 0.0, -speed));
        break;
    }
    default:
        break;
    }

    for (auto& model : models) {
        model.modelMatrix = orbitTransform * model.modelMatrix;
    }

   for (int i = 0; i < orbitVertices.size(); ++i) {
        for (int j = 0; j < orbitVertices[i].size(); ++j) {

            glm::vec4 rotatedPoint = orbitTransform * glm::vec4(orbitVertices[i][j], 1.0f);
            orbitVertices[i][j] = glm::vec3(rotatedPoint);

        }
        // ����� orbitVertices �����͸� VBO�� ���ε�
        glBindBuffer(GL_ARRAY_BUFFER, orbitVBO[i]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, orbitVertices[i].size() * sizeof(glm::vec3), orbitVertices[i].data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }



    glutPostRedisplay();  // ȭ�� �ٽ� �׸��� ��û
}

// ���콺 �Է��� ó���ϴ� �Լ�
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        std::cout << "x = " << x << " y = " << y << std::endl;  // ���콺 Ŭ�� ��ǥ ���
    glutPostRedisplay();  // Ű���� �Է� �� ȭ���� �ٽ� �׸����� ��û
}

void rotation_xyz(Model& model, float radians, glm::vec3 xyz) {
    model.modelMatrix = glm::rotate(model.modelMatrix, glm::radians(radians), xyz);
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
    glutCreateWindow("18��");  // â ���� �� ���� ����

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
    Model modelSphere;

    // ������ü
    read_obj_file("sphere.obj", modelSphere, "sphere");
    // �ʱ� ȸ�� ���¸� ���� (�ʱ� ȸ�� ���´� ������)

    for (int i = 0; i < 7; ++i) {
        Model m = modelSphere;
        m.modelMatrix = glm::mat4(1.0f);
        switch (i)
        {
        case 0:
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.4, 0.4, 0.4));
            break;
        case 1:
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(2.0f, 0.0f, 0.0f));
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.2, 0.2, 0.2));
            m.modelMatrix = glm::rotate(m.modelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            break;
        case 2:
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(1.5f, 0.0f, 0.0f));
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.1, 0.1, 0.1));
            break;
        case 3:
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(1.5f, -1.5f, 0.0f));
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.2, 0.2, 0.2));
            m.modelMatrix = glm::rotate(m.modelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            break;
        case 4:
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(1.5f, -1.5f, 0.0f));
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(0.5f, 0, 0.0f));
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.1, 0.1, 0.1));
            m.modelMatrix = glm::rotate(m.modelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            break;
        case 5:
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(-1.5f, -1.5f, 0.0f));
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.2, 0.2, 0.2));
            m.modelMatrix = glm::rotate(m.modelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            break;
        case 6:
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(-1.5f, -1.5f, 0.0f));
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(-0.5f, 0, 0.0f));
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.1, 0.1, 0.1));
            m.modelMatrix = glm::rotate(m.modelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            break;
        default:
            break;
        }

        glm::vec3 s_color = { dis_color(gen), dis_color(gen), dis_color(gen) };

        for (int i = 0; i < modelSphere.vertices.size(); ++i) {
            m.colors.push_back(s_color);
        }

        models.push_back(m);
    }

    Model modelXLine, modelYLine, modelZLine;
    modelXLine.name = "xLine";
    modelXLine.vertices.push_back(x[0]);
    modelXLine.vertices.push_back(x[1]);
    modelXLine.colors.push_back(glm::vec3(1.0, 0.0, 0.0));

    modelYLine.name = "yLine";
    modelYLine.vertices.push_back(y[0]);
    modelYLine.vertices.push_back(y[1]);
    modelYLine.colors.push_back(glm::vec3(0.0, 1.0, 0.0));

    modelZLine.name = "zLine";
    modelZLine.vertices.push_back(z[0]);
    modelZLine.vertices.push_back(z[1]);
    modelZLine.colors.push_back(glm::vec3(0.0, 0.0, 1.0));

    models.push_back(modelXLine);
    models.push_back(modelYLine);
    models.push_back(modelZLine);

    createOrbitVertices(2.0, glm::vec3(0.0, 0.0, 0.0), 0.0f);
    createOrbitVertices(0.5, glm::vec3(models[1].modelMatrix[3]), 0.0f);
    createOrbitVertices(2.12f, glm::vec3(0.0, 0.0, 0.0), -45.0f);
    createOrbitVertices(0.5, glm::vec3(models[3].modelMatrix[3]), 0.0f);
    createOrbitVertices(2.12f, glm::vec3(0.0, 0.0, 0.0), 45.0f);
    createOrbitVertices(0.5, glm::vec3(models[5].modelMatrix[3]), 0.0f);

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

    // ī�޶�
    cameraPos = glm::vec3(0.0, 0.0, 5.5);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 rotationMatrix_x = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationMatrix_y = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cameraPos = glm::vec3(rotationMatrix_x * rotationMatrix_y * glm::vec4(cameraPos, 1.0f));
    view = glm::lookAt(
        cameraPos,  //--- ī�޶���ġ
        cameraDirection,  //--- ī�޶�ٶ󺸴¹���
        cameraUp   //--- ī�޶����ʹ���
    );
    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    // ����
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    glEnable(GL_DEPTH_TEST);
    GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");

    // �� ���� �׸���
    for (size_t i = 0; i < models.size(); ++i) {
        glBindVertexArray(vaos[i]);

        if (models[i].name == "sphere") {
            GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            // �� �׸���
            glUniform1i(modelStatus, 0);
            glLineWidth(1.0f);
            if (keyStates['m'])
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            else if (keyStates['M'])
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);
    }


    //-------------------------------------------------
    for (int i = 0; i < orbitVertices.size(); ++i) {
        glBindVertexArray(orbitVAO[i]);

        // Line ����� �����ͼ� ���� �˵� ��ġ�� ��ȯ ����
        GLint lineLoc = glGetUniformLocation(shaderProgramID, "model");

        // �� ��� ���, �˵��� �߽��� Line ��ķ� �����մϴ�.
        glm::mat4 lineTransform = glm::mat4(1.0f);  // �ʿ��� ��� �߽� �̵��̳� ȸ�� ���� ����
        glUniformMatrix4fv(lineLoc, 1, GL_FALSE, glm::value_ptr(lineTransform));

        glUniform1i(modelStatus, 2);
        glLineWidth(1.5f);
        glDrawArrays(GL_LINE_STRIP, 0, orbitVertices[i].size());
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

    for (int i = 0; i < orbitVertices.size(); ++i) {
        glGenVertexArrays(1, &orbitVAO[i]);
        glGenBuffers(1, &orbitVBO[i]);

        glBindVertexArray(orbitVAO[i]);

        // glm::vec4 -> glm::vec3 ��ȯ
        std::vector<glm::vec3> vertexData;
        for (const auto& vec4Point : orbitVertices[i]) {
            vertexData.push_back(glm::vec3(vec4Point));  // vec4�� xyz �κи� ������
        }

        glBindBuffer(GL_ARRAY_BUFFER, orbitVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(glm::vec3), vertexData.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // ��ġ ����
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

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
