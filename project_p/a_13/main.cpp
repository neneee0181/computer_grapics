#include <iostream>   // ǥ�� ����� ���̺귯�� ����
#include <gl/glew.h>  // GLEW ���̺귯�� ���� (OpenGL Ȯ�� ����� ����ϱ� ���� �ʿ�)
#include <gl/freeglut.h>  // GLUT ���̺귯�� ���� (������ â �� ����� �Է� ó���� ���� ���)
#include <gl/freeglut_ext.h>  // GLUT Ȯ�� ��� ����
#include <gl/glm/glm/glm.hpp>  // GLM ���̺귯�� ���� (�������� ����� ���� ���� ���̺귯��)

#include <vector>  // ǥ�� ���� ���̺귯�� ���� (���� �迭�� ����ϱ� ���� �ʿ�)

#include "filetobuf.h"  // ���̴� �ҽ� ������ ���۷� �о���� �Լ��� ������ �������

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

GLuint vao, vbo[2];  // VAO�� VBO ID�� ������ ������

// �ﰢ���� ��ǥ�� ������ �迭
const GLfloat triShape[3][3] = {
	{-0.5,-0.5,0.0},{0.5,-0.5,0.0},{0.0,0.5,0.0}
};

// �ﰢ���� �� ���� �ش��ϴ� ���� �迭
const GLfloat colors[3][3] = {
	{1.0,0.0,0.0}, {0.0,1.0,0.0},{0.0,0.0,1.0}
};

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

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);  // ù ��° VBO ���ε� (�ﰢ�� ��ǥ ����)
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape, GL_STATIC_DRAW);  // ��ǥ �����͸� VBO�� ����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // ��ǥ �Ӽ� ������ ����
	glEnableVertexAttribArray(0);  // �Ӽ� �迭 Ȱ��ȭ

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);  // �� ��° VBO ���ε� (���� ������ ����)
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);  // ���� �����͸� VBO�� ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // ���� �Ӽ� ������ ����
	glEnableVertexAttribArray(1);  // �Ӽ� �迭 Ȱ��ȭ
}
