#include <iostream>   // ǥ�� ����� ���̺귯�� ����
#include <gl/glew.h>  // GLEW ���̺귯�� ���� (OpenGL Ȯ�� ����� ����ϱ� ���� �ʿ�)
#include <gl/freeglut.h>  // GLUT ���̺귯�� ���� (������ â �� ����� �Է� ó���� ���� ���)
#include <gl/freeglut_ext.h>  // GLUT Ȯ�� ��� ����
#include <gl/glm/glm/glm.hpp>  // GLM ���̺귯�� ���� (�������� ����� ���� ���� ���̺귯��)
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector>  // ǥ�� ���� ���̺귯�� ���� (���� �迭�� ����ϱ� ���� �ʿ�)

#include "filetobuf.h"  // ���̴� �ҽ� ������ ���۷� �о���� �Լ��� ������ �������
#include "ReadObj.h"
#include "LoadTexture.h"

using namespace std;  // ���ӽ����̽� std ������� �ڵ� ������ std:: ���� ����

// �Լ� �����
void make_vertexShaders();  // ���ؽ� ���̴��� ����� �Լ� ����
void make_fragmentShaders();  // �����׸�Ʈ ���̴��� ����� �Լ� ����
void InitBuffer();  // ���� �ʱ�ȭ �Լ� ����
void make_shaderProgram();  // ���̴� ���α׷��� ����� �Լ� ����
GLvoid drawScene(GLvoid);  // ȭ���� �׸��� �Լ� ����
GLvoid Reshape(int w, int h);  // ȭ�� ũ�Ⱑ ����� �� ȣ��Ǵ� �Լ� ����

// ���� ���� �����
GLint width, height;  // â�� �ʺ�� ���̸� �����ϴ� ����
GLuint shaderProgramID;  // ���̴� ���α׷��� ID�� �����ϴ� ����
GLuint vertexShader;  // ���ؽ� ���̴� ID�� �����ϴ� ����
GLuint fragmentShader;  // �����׸�Ʈ ���̴� ID�� �����ϴ� ����

GLchar* vertexSource, * fragmentSource;  // ���̴� �ҽ� �ڵ带 ������ ������

GLuint xVBO, xVAO;
GLuint yVBO, yVAO;
GLuint zVBO, zVAO;

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

int number_status = 0;
char key_result = ' ';
bool x_status = false, y_status = false;

void timer(int value) {
    if (number_status == 0) {
        return;
    }

    float speed = 0.5f;
    for (int i = 0; i < number_status; ++i) {
        if (key_result == 'x' || key_result == 'X') {
            if (x_status) {
                models[i].modelMatrix = glm::rotate(models[i].modelMatrix, glm::radians(speed), glm::vec3(1.0, 0.0, 0.0));
            }
            else {
                models[i].modelMatrix = glm::rotate(models[i].modelMatrix, glm::radians(-speed), glm::vec3(1.0, 0.0, 0.0));
            }
        }

        if (key_result == 'y' || key_result == 'Y') {
            if (y_status) {
                models[i].modelMatrix = glm::rotate(models[i].modelMatrix, glm::radians(speed), glm::vec3(0.0, 1.0, 0.0));
            }
            else {
                models[i].modelMatrix = glm::rotate(models[i].modelMatrix, glm::radians(-speed), glm::vec3(0.0, 1.0, 0.0));
            }
        }
    }
    glutPostRedisplay();  // ȭ�� �ٽ� �׸��� ��û
    glutTimerFunc(16, timer, 0);
}

void keyBoard(unsigned char key, int x, int y) {
    switch (key)
    {
    case 'x':
        key_result = 'x';
        x_status = true;
        break;
    case 'X':
        key_result = 'X';
        x_status = false;
        break;
    case 'y':
        key_result = 'y';
        y_status = true;
        break;
    case 'Y':
        key_result = 'Y';
        y_status = false;
        break;
    case '0':
        number_status = 0;
        break;
    case '1':
        number_status = 1;
        glutTimerFunc(0, timer, 0);
        break;
    case '2':
        number_status = 2;
        glutTimerFunc(0, timer, 0);
        break;
    case '3':
        number_status = 3;
        glutTimerFunc(0, timer, 0);
        break;
    default:
        break;
    }
    glutPostRedisplay();  // ȭ�� �ٽ� �׸��� ��û
}

void specialKeys(int key, int x, int y) {
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
    glutCreateWindow("�ǽ� 15��");  // â ���� �� ���� ����

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
    Model modelSqu, modelCone, modelSphere, modelCylinder;
    read_obj_file("box3.obj", modelSqu, "cube");
    // ������ü
    modelSqu.modelMatrix = glm::rotate(modelSqu.modelMatrix, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0));
    modelSqu.modelMatrix = glm::rotate(modelSqu.modelMatrix, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0));
    modelSqu.modelMatrix = glm::scale(modelSqu.modelMatrix, glm::vec3(0.2, 0.2, 0.2));
    modelSqu.modelMatrix = glm::translate(modelSqu.modelMatrix, glm::vec3(-3.0, 0.0, 0.0));
    modelSqu.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelSqu);

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
    glutSpecialFunc(specialKeys);  // Ư�� Ű (����Ű ��) �Է� ó�� �Լ� ���
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

        if (models[i].name == "cube") {
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

// ȭ�� ũ�Ⱑ ����� �� ȣ��Ǵ� �Լ�
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);  // ����Ʈ ũ�� ����
}

// ���ؽ� ���̴��� ����� �Լ�
void make_vertexShaders() {
    vertexSource = filetobuf("vertex.glsl");  // ���ؽ� ���̴� ���� �б�
    vertexShader = glCreateShader(GL_VERTEX_SHADER);  // ���ؽ� ���̴� ����
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);  // ���̴� �ҽ� �ڵ� ����
    glCompileShader(vertexShader);  // ���̴� ������

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);  // ������ ���� ���� Ȯ��

    if (!result) {  // ������ ���� ��
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);  // ���� �α� �ޱ�
        cerr << "ERROR: vertex shader ������ ���� \n" << errorLog << endl;  // ���� �޽��� ���
        return;
    }
}

// �����׸�Ʈ ���̴��� ����� �Լ�
void make_fragmentShaders() {
    fragmentSource = filetobuf("fragment.glsl");  // �����׸�Ʈ ���̴� ���� �б�
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  // �����׸�Ʈ ���̴� ����
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);  // ���̴� �ҽ� �ڵ� ����
    glCompileShader(fragmentShader);  // ���̴� ������

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);  // ������ ���� ���� Ȯ��

    if (!result) {  // ������ ���� ��
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);  // ���� �α� �ޱ�
        cerr << "ERROR: frag_shader ������ ����\n" << errorLog << endl;  // ���� �޽��� ���
        return;
    }
}

// ���̴� ���α׷��� ����� �Լ�
void make_shaderProgram() {
    make_vertexShaders();  // ���ؽ� ���̴� ����
    make_fragmentShaders();  // �����׸�Ʈ ���̴� ����

    shaderProgramID = glCreateProgram();  // ���̴� ���α׷� ����
    glAttachShader(shaderProgramID, vertexShader);  // ���ؽ� ���̴� ���α׷��� ÷��
    glAttachShader(shaderProgramID, fragmentShader);  // �����׸�Ʈ ���̴� ���α׷��� ÷��
    glLinkProgram(shaderProgramID);  // ���̴� ���α׷� ��ũ

    glDeleteShader(vertexShader);  // ���ؽ� ���̴� ���� (�̹� ���α׷��� ���ԵǾ����Ƿ�)
    glDeleteShader(fragmentShader);  // �����׸�Ʈ ���̴� ����

    glUseProgram(shaderProgramID);  // ���̴� ���α׷� ���
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
