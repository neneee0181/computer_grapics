#include <iostream>   // ǥ�� ����� ���̺귯�� ����
#include <gl/glew.h>  // GLEW ���̺귯�� ���� (OpenGL Ȯ�� ����� ����ϱ� ���� �ʿ�)
#include <gl/freeglut.h>  // GLUT ���̺귯�� ���� (������ â �� ����� �Է� ó���� ���� ���)
#include <gl/freeglut_ext.h>  // GLUT Ȯ�� ��� ����
#include <gl/glm/glm/glm.hpp>  // GLM ���̺귯�� ���� (�������� ����� ���� ���� ���̺귯��)

#include <vector>  // ǥ�� ���� ���̺귯�� ���� (���� �迭�� ����ϱ� ���� �ʿ�)

#include "filetobuf.h"  // ���̴� �ҽ� ������ ���۷� �о���� �Լ��� ������ �������
#include "ReadObj.h"

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

GLuint vao, vbo[3];  // VAO�� VBO ID�� ������ ������

const GLfloat colors[12][3] = {
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
    {1.0, 0.5, 0.5}    // ��ũ
};

Model modelSqu;

// Ű���� �Է��� ó���ϴ� �Լ�
void keyBoard(unsigned char key, int x, int y) {
    switch (key)
    {
    default:
        break;
    }
    glutPostRedisplay();  // Ű���� �Է� �� ȭ���� �ٽ� �׸����� ��û
}

// ���콺 �Է��� ó���ϴ� �Լ�
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        std::cout << "x = " << x << " y = " << y << std::endl;  // ���콺 Ŭ�� ��ǥ ���
}

// ���� �Լ�
int main(int argc, char** argv) {

    width = 800;  // â�� �⺻ �ʺ� ����
    height = 600;  // â�� �⺻ ���� ����

    // ������ ����
    glutInit(&argc, argv);  // GLUT �ʱ�ȭ
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);  // ȭ�� ��� ���� (���� ���۸�, RGBA �÷� ���)
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
        
    read_obj_file("cube.obj", modelSqu); //obj ���� ��������

    try {
        std::cout << "OBJ ���� �ε� ����!" << std::endl;
        std::cout << "���� ����: " << modelSqu.vertices.size() << std::endl;
        for (size_t i = 0; i < modelSqu.vertices.size(); ++i) {
            std::cout << "v[" << i << "] = ("
                << modelSqu.vertices[i].x << ", "
                << modelSqu.vertices[i].y << ", "
                << modelSqu.vertices[i].z << ")" << std::endl;
        }
        std::cout << "�ؽ�ó ��ǥ ����: " << modelSqu.texCoords.size() << std::endl;
        std::cout << "���� ���� ����: " << modelSqu.normals.size() << std::endl;
        std::cout << "�� ����: " << modelSqu.faces.size() << std::endl;
        for (size_t i = 0; i < modelSqu.faces.size(); ++i) {
            const Face& face = modelSqu.faces[i];
            std::cout << "Face[" << i << "] = ("
                << face.v1 << ", " << face.v2 << ", " << face.v3 << ")"
                << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;  // ���� �޽��� ���
    }

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

    glUseProgram(shaderProgramID);  // ������ ���̴� ���α׷� ���

    glBindVertexArray(vao);  // VAO ���ε�

    // �ε��� ���۸� ����� �� �׸���
    glDrawElements(GL_TRIANGLES, modelSqu.faces.size() * 3, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);  // VAO ���ε� ����

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
    glGenVertexArrays(1, &vao);  // VAO ����
    glBindVertexArray(vao);  // VAO ���ε�

    glGenBuffers(2, vbo);  // VBO ���� (2��)

    // VBO ���� �� ������ ���� (������ ��ǥ)
    glGenBuffers(1, &vbo[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, modelSqu.vertices.size() * sizeof(glm::vec3), modelSqu.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // ���� ���� ����
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // �� �ε��� ������ (EBO) ����
    std::vector<unsigned int> indices;
    for (const Face& face : modelSqu.faces) {
        indices.push_back(face.v1);
        indices.push_back(face.v2);
        indices.push_back(face.v3);
    }

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}