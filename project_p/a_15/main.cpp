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

// ī�޶� ���� ������
glm::vec3 cameraPos = glm::vec3(4.0f, 4.0f, 4.0f);  // �ʱ� ī�޶� ��ġ
glm::vec3 cameraFront = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)); // ī�޶� ���� (���� �ٶ�)
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);   // ���� �� ����
float yaw = -135.0f;  // �¿� ȸ�� ����
float pitch = 0.0f;   // ���� ȸ�� ����
float cameraSpeed = 0.1f;  // ī�޶� �̵� �ӵ�
float sensitivity = 2.00f; // ���콺/Ű���� ȸ�� �ӵ�

const GLfloat colors[20][3] = {
    {1.0, 0.0, 0.0},   // ����
    {0.0, 1.0, 0.0},   // �ʷ�
    {0.0, 0.0, 1.0},   // �Ķ�
    {1.0, 1.0, 0.0},   // ���
    {1.0, 0.0, 1.0},   // ����Ÿ
    {0.0, 1.0, 1.0},   // �þ�
    {0.5, 0.5, 0.5},   // ȸ��
    {1.0, 0.5, 0.0},   // ��Ȳ
    {0.0, 0.5, 0.5},   // û��
    {0.5, 0.0, 0.5},   // ����
    {0.5, 1.0, 0.5},   // ����
    {1.0, 0.5, 0.5},    // ��ũ
     {0.0, 0.5, 0.5},   // û��
    {0.5, 0.0, 0.5},   // ����
    {0.5, 1.0, 0.5},   // ����
    {1.0, 0.5, 0.5},
     {0.0, 0.5, 0.5},   // û��
    {0.5, 0.0, 0.5},   // ����
    {0.5, 1.0, 0.5},   // ����
    {1.0, 0.5, 0.5},

};

const glm::vec3 x[2] = {
    glm::vec3(-0.8f,0.0f, 0.0f), glm::vec3(0.8f,0.0f, 0.0f)
};

const glm::vec3 y[2] = {
    glm::vec3(0.0f,-0.8f, 0.0f), glm::vec3(0.0f, 0.8f, 0.0f)
};

const glm::vec3 z[2] = {
    glm::vec3(0.0f, 0.0f, -0.8f), glm::vec3(0.0f, 0.0f, 0.8f)
};

void keyBoard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':  // ���� ȸ�� (pitch ����)
        pitch += sensitivity;
        if (pitch > 89.0f)
            pitch = 89.0f;
        break;
    case 's':  // �Ʒ��� ȸ�� (pitch ����)
        pitch -= sensitivity;
        if (pitch < -89.0f)
            pitch = -89.0f;
        break;
    case 'a':  // ���� ȸ�� (yaw ����)
        yaw -= sensitivity;
        break;
    case 'd':  // ������ ȸ�� (yaw ����)
        yaw += sensitivity;
        break;
    }
    // ȸ���� yaw, pitch�� ���� cameraFront�� ������Ʈ
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    glutPostRedisplay();  // ȭ�� �ٽ� �׸��� ��û
}


void specialKeys(int key, int x, int y) {
    float speed = 0.1f;  // ī�޶� �̵� �ӵ�

    switch (key) {
    case GLUT_KEY_UP:  // ���� ����Ű: ����
        cameraPos += speed * cameraFront;
        break;
    case GLUT_KEY_DOWN:  // �Ʒ��� ����Ű: ����
        cameraPos -= speed * cameraFront;
        break;
    case GLUT_KEY_LEFT:  // ���� ����Ű: �������� �̵�
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
        break;
    case GLUT_KEY_RIGHT:  // ������ ����Ű: �������� �̵�
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
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
    glutCreateWindow("�ǽ� 13��");  // â ���� �� ���� ����

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
    Model modelSqu;
    read_obj_file("box3.obj", modelSqu, "cube");

    models.push_back(modelSqu);

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

glm::mat4 modelSquMatrix = glm::mat4(1.0f);  // ���� ��ķ� �ʱ�ȭ
glm::mat4 xLineMatrix = glm::mat4(1.0f);
glm::mat4 yLineMatrix = glm::mat4(1.0f);

// ȭ���� �׸��� �Լ�
GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);  // ȭ���� ������� �ʱ�ȭ
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // �÷� ���ۿ� ���� ���� �ʱ�ȭ

    glUseProgram(shaderProgramID);  // ���̴� ���α׷� ���

    GLint isLineLoc = glGetUniformLocation(shaderProgramID, "isLine");

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "modelV");
    modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));

    glEnable(GL_DEPTH_TEST);
    // �� ���� �׸���
    for (size_t i = 0; i < models.size(); ++i) {
        glBindVertexArray(vaos[i]);
       
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        // �� �׸���
        glUniform1i(isLineLoc, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }

    // X�� �׸���
    glBindVertexArray(xVAO);  // X�� VAO ���ε�
    GLint xLineLoc = glGetUniformLocation(shaderProgramID, "xLine");
    glUniformMatrix4fv(xLineLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));  // �ʱ�ȭ�� ��ȯ ��� ����
    glUniform1i(isLineLoc, 2);  // isLine ���� 2�� ���� (�� �׸��� ��)
    glLineWidth(1.0f);  // �� �β� ����
    glDrawArrays(GL_LINES, 0, 2);  // X�� �׸���
    glBindVertexArray(0);  // VAO ���ε� ����

    // Y�� �׸���
    glBindVertexArray(yVAO);  // Y�� VAO ���ε�
    GLint yLineLoc = glGetUniformLocation(shaderProgramID, "yLine");
    glUniformMatrix4fv(yLineLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));  // �ʱ�ȭ�� ��ȯ ��� ����
    glUniform1i(isLineLoc, 3);  // isLine ���� 3���� ���� (�� �׸��� ��)
    glLineWidth(1.0f);  // �� �β� ����
    glDrawArrays(GL_LINES, 0, 2);  // Y�� �׸���
    glBindVertexArray(0);  // VAO ���ε� ����

    // z�� �׸���
    glBindVertexArray(zVAO);  // Y�� VAO ���ε�
    GLint zLineLoc = glGetUniformLocation(shaderProgramID, "xLine");
    glUniformMatrix4fv(zLineLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));  // �ʱ�ȭ�� ��ȯ ��� ����
    glUniform1i(isLineLoc, 3);  // isLine ���� 3���� ���� (�� �׸��� ��)
    glLineWidth(1.0f);  // �� �β� ����
    glDrawArrays(GL_LINES, 0, 2);  // Y�� �׸���
    glBindVertexArray(0);  // VAO ���ε� ����

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
    // X�� ��ǥ�� ���� ���� VAO�� VBO ����
    glGenVertexArrays(1, &xVAO);  // X�� VAO ����
    glBindVertexArray(xVAO);  // VAO ���ε�
    glGenBuffers(1, &xVBO);  // VBO ����
    glBindBuffer(GL_ARRAY_BUFFER, xVBO);  // ���� ���ε�
    glBufferData(GL_ARRAY_BUFFER, sizeof(x), x, GL_STATIC_DRAW);  // X�� ��ǥ ������ ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // ���� �Ӽ� ������ ����
    glEnableVertexAttribArray(0);  // ���� �Ӽ� ���

    glGenVertexArrays(1, &yVAO); 
    glBindVertexArray(yVAO);  // VAO ���ε�
    glGenBuffers(1, &yVBO);  // VBO ����
    glBindBuffer(GL_ARRAY_BUFFER, yVBO);  // ���� ���ε�
    glBufferData(GL_ARRAY_BUFFER, sizeof(y), y, GL_STATIC_DRAW);  // Y�� ��ǥ ������ ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // ���� �Ӽ� ������ ����
    glEnableVertexAttribArray(0);  // ���� �Ӽ� ���

    glGenVertexArrays(1, &zVAO);
    glBindVertexArray(zVAO);  // VAO ���ε�
    glGenBuffers(1, &zVBO);  // VBO ����
    glBindBuffer(GL_ARRAY_BUFFER, zVBO);  // ���� ���ε�
    glBufferData(GL_ARRAY_BUFFER, sizeof(z), z, GL_STATIC_DRAW);  // Y�� ��ǥ ������ ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // ���� �Ӽ� ������ ����
    glEnableVertexAttribArray(0);  // ���� �Ӽ� ���

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
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
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
