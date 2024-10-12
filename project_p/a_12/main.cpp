#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
//#include <gl/glm/glm.hpp>
#include <gl/glm/glm/glm.hpp>
#include <random>

#include <vector>

#include "filetobuf.h"

#define M_PI 3.14159265358979323846

using namespace std;

void make_vertexShaders();
void make_fragmentShaders();
void InitBuffer();
void make_shaderProgram();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

GLint width, height;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLchar* vertexSource, * fragmentSource;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(-1.0f, 1.0f);

enum ShapeType {
	TRIANGLE,
	SQUARE,
	POINT_,
	LINE,
};

struct Shape_gl {
	ShapeType type;
	vector<glm::vec3> vertices; // ������ ����
	glm::vec3 color; // ������ ����
};


void insert_triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 color, vector<Shape_gl>& shapes) {
	shapes.push_back(
		{
			TRIANGLE,
		{v1,v2,v3},
		glm::vec3(dis(gen), dis(gen), dis(gen))
		}
	);
}

void insert_square(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 v5, glm::vec3 v6, glm::vec3 color, vector<Shape_gl>& shapes) {
	shapes.push_back(
		{
			SQUARE,
		{v1,v2,v3,v4,v5,v6},
		glm::vec3(dis(gen), dis(gen), dis(gen))
		}
	);
}

void insert_point(glm::vec3 v1, glm::vec3 color, vector<Shape_gl>& shapes) {
	shapes.push_back(
		{
			POINT_,
		{v1},
		glm::vec3(dis(gen), dis(gen), dis(gen))
		}
	);
}

void insert_line(glm::vec3 v1, glm::vec3 v2, glm::vec3 color, vector<Shape_gl>& shapes) {
	shapes.push_back(
		{
			LINE,
		{v1,v2},
		color
		}
	);
}

// ��� ������ ��� ����
vector<Shape_gl> shapes;

GLuint vao, vbo[2];

float stepSize = 0.01f;    // �� ���� ������Ʈ���� �̵��� �Ÿ�

void keyBoard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'c':
		shapes.clear();
		break;
	default:
		break;
	}
	InitBuffer();
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		std::cout << "x = " << x << " y = " << y << std::endl;
}

void makeShape3() {
	float x = 0.0f, y = 0.0f;

	// ���� ���� ���� �ڵ�
	for (int i = 0; i < 3; ++i) { // ���� ����
		x = dis(gen);
		y = dis(gen);
		float x2 = dis(gen), y2 = dis(gen);
		insert_line(glm::vec3(x, y, 0.0f), glm::vec3(x2, y2, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), shapes);
	}

	// �ﰢ�� ���� �ڵ�
	std::uniform_real_distribution<> size_dis(0.05f, 0.1f);  // �ﰢ���� �簢��, ������ ũ�⸦ 0.05 ~ 0.1 ���̿��� �����ϰ� ����

	for (int i = 0; i < 3; ++i) { // �ﰢ���� 3�� ����
		// �ﰢ���� �� ������ ���� ��ǥ�� �����ϰ� ����
		glm::vec3 v1(dis(gen), dis(gen), 0.0f);

		// ������ ũ���� �ﰢ���� ���� ũ�� ����
		float size = size_dis(gen);  // �ﰢ���� ũ�⸦ �����ϴ� ���� ����

		// ù ��° �������κ��� ���� �Ÿ���ŭ ������ ������ �� ���� ����
		glm::vec3 v2(v1.x + size, v1.y, 0.0f);               // x �������� ���� �Ÿ�
		glm::vec3 v3(v1.x + size / 2.0f, v1.y + size, 0.0f); // �밢�� �������� ���� �Ÿ�

		// ������ ����
		glm::vec3 color(dis(gen), dis(gen), dis(gen));

		// �ﰢ�� �߰�
		insert_triangle(v1, v2, v3, color, shapes);
	}

	// �簢�� ���� �ڵ�
	for (int i = 0; i < 3; ++i) { // �簢���� 3�� ����
		// �簢���� ���� �Ʒ� ������ ���� ��ǥ�� �����ϰ� ����
		glm::vec3 v1(dis(gen), dis(gen), 0.0f);

		// ������ ũ���� �簢���� ���� ũ�� ����
		float size = size_dis(gen);  // �簢���� ũ�⸦ �����ϴ� ���� ����

		// �簢���� ������ ���� ����
		glm::vec3 v2(v1.x + size, v1.y, 0.0f);               // ������ �Ʒ�
		glm::vec3 v3(v1.x, v1.y + size, 0.0f);               // ���� ��
		glm::vec3 v4(v1.x + size, v1.y + size, 0.0f);        // ������ ��

		// �簢���� �� ���� �ﰢ������ ������ ���� ���� �߰�
		// v1, v2, v3�� ù ��° �ﰢ��
		// v2, v3, v4�� �� ��° �ﰢ��
		glm::vec3 color(dis(gen), dis(gen), dis(gen)); // ������ ����

		// �簢�� �߰� (2���� �ﰢ������ ����)
		insert_square(v1, v2, v3, v2, v3, v4, color, shapes);
	}

	// ������ ���� �ڵ�
	for (int i = 0; i < 3; ++i) { // �������� 3�� ����
		// �������� �߽� ��ǥ ����
		glm::vec3 center(dis(gen), dis(gen), 0.0f);

		// ������ ũ���� �������� ���� ũ�� ����
		float radius = size_dis(gen);  // �������� ������ ũ��

		// �������� 5���� ���� ���� (���� ���)
		std::vector<glm::vec3> pentagonVertices;
		for (int j = 0; j < 5; ++j) {
			float angle = j * (2.0f * M_PI / 5);  // 360���� 5���
			float x = center.x + radius * cos(angle);
			float y = center.y + radius * sin(angle);
			pentagonVertices.push_back(glm::vec3(x, y, 0.0f));
		}

		// ������ ����
		glm::vec3 color(dis(gen), dis(gen), dis(gen));

		// �������� �ﰢ������ ������ (�߽����� ������ �� �������� �ﰢ���� ����)
		for (int j = 0; j < 5; ++j) {
			glm::vec3 v1 = pentagonVertices[j];
			glm::vec3 v2 = pentagonVertices[(j + 1) % 5];  // ���� ���� (�ε��� ��ȯ)
			insert_triangle(center, v1, v2, color, shapes);
		}
	}
}

int main(int argc, char** argv) {

	width = 800;
	height = 600;

	//������ ����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("�ǽ� 12��");

	//glew �ʱ�ȭ
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	make_shaderProgram();
	makeShape3();
	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyBoard);
	glutMouseFunc(mouse);
	glutMainLoop();

	return 0;
}

int currentIndex = 0; // ���� ���� �ε���

void drawShapes() {

	for (const Shape_gl& shape : shapes) {
		if (shape.type == TRIANGLE) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 3); // �ﰢ���� 3���� ����
			currentIndex += 3; // �ﰢ���� ���� ����ŭ �ε����� ����
		}
		else if (shape.type == SQUARE) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 6); // �簢���� 2���� �ﰢ�� (6���� ����)
			currentIndex += 6; // �簢���� ���� ����ŭ �ε����� ����
		}
		else if (shape.type == POINT_) {
			glPointSize(10.0f); // �� ũ�� ����
			glDrawArrays(GL_POINTS, currentIndex, 1); // ���� 1���� ����
			currentIndex += 1; // ���� ���� ����ŭ �ε����� ����
		}
		else if (shape.type == LINE) {
			glLineWidth(2.0f); // �� �β� ����
			glDrawArrays(GL_LINES, currentIndex, 2); // ���� 2���� ����
			currentIndex += 2; // ���� ���� ����ŭ �ε����� ����
		}
	}
}

GLvoid drawScene() {

	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	glBindVertexArray(vao);
	currentIndex = 0;  // �Ź� �׸��� ������ �� ���� �ε����� 0���� �ʱ�ȭ
	drawShapes(); // ���� �׸���
	glutSwapBuffers();

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error: " << err << endl;
	}
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void make_vertexShaders() {

	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	if (!result) {
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader ������ ���� \n" << errorLog << endl;
		return;
	}

}

void make_fragmentShaders() {

	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

	if (!result) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: frag_shader ������ ����\n" << errorLog << endl;
		return;
	}
}

void make_shaderProgram() {

	make_vertexShaders();
	make_fragmentShaders();

	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgramID);

}

void InitBuffer() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, vbo);

	// ��� ������ ������ �� �迭�� ���
	vector<glm::vec3> allVertices;
	vector<glm::vec3> allColors;

	for (const Shape_gl& shape : shapes) {
		// ���� �߰�
		allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

		// ���� �߰� (������ ����ŭ ���� �߰�)
		for (size_t i = 0; i < shape.vertices.size(); ++i) {
			allColors.push_back(shape.color);
		}
	}

	// ���� ������ OpenGL�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(glm::vec3), allVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// ���� ������ OpenGL�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, allColors.size() * sizeof(glm::vec3), allColors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}