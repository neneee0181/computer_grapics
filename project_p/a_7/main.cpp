#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>

#include <vector>
#include <random>

#include "filetobuf.h"

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

enum ShapeType {
	TRIANGLE,
	SQUARE,
	POINT_,
	LINE
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
		color
		}
	);
}

void insert_square(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 v5, glm::vec3 v6, glm::vec3 color, vector<Shape_gl>& shapes) {
	shapes.push_back(
		{
			SQUARE,
		{v1,v2,v3,v4,v5,v6},
		color
		}
	);
}

void insert_point(glm::vec3 v1, glm::vec3 color, vector<Shape_gl>& shapes) {
	shapes.push_back(
		{
			POINT_,
		{v1},
		color
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

ShapeType statusType = ShapeType::TRIANGLE;
int selectStatus = 0;

void initShapes() {
	// �ﰢ�� �߰�

	//insert_triangle(glm::vec3(-0.7f, -0.5f, 0.0f), glm::vec3(0.7f, -0.5f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), shapes);
	insert_square(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(-0.5f, 0.5f, 0.0f),
		glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), shapes);
	insert_point(glm::vec3(0.0f, 0.9f, 0.4f), glm::vec3(0.0f, 0.0f, 1.0f), shapes);
	insert_line(glm::vec3(-0.9f, 0.0f, 0.0f), glm::vec3(0.9f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), shapes);

}

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> rand_color(0.0f, 1.0f);
void keyBoard(unsigned char key, int x, int y) {

	uniform_int_distribution<> rand_select(0, shapes.size() > 0 ? shapes.size() - 1 : 0);
	const float moveDistance = 0.05f;
	cout << selectStatus << endl;
	switch (key)
	{
	case 'p': // ��
		statusType = ShapeType::POINT_;
		selectStatus = rand_select(gen);
		break;
	case 'l': //��
		statusType = ShapeType::LINE;
		selectStatus = rand_select(gen);
		break;
	case 't': //�ﰢ��
		statusType = ShapeType::TRIANGLE;
		selectStatus = rand_select(gen);
		break;
	case 'r': //�簢��
		statusType = ShapeType::SQUARE;
		selectStatus = rand_select(gen);
		break;
	case 'w': // ���� �̵�
		for (auto& vertex : shapes[selectStatus].vertices) {
			vertex.y += moveDistance;
		}
		cout << "w" << endl;
		break;
	case 's': // �Ʒ��� �̵�
		for (auto& vertex : shapes[selectStatus].vertices) {
			vertex.y -= moveDistance;
		}
		cout << "s" << endl;
		break;
	case 'a': // �������� �̵�
		for (auto& vertex : shapes[selectStatus].vertices) {
			vertex.x -= moveDistance;
		}
		cout << "a" << endl;
		break;
	case 'd': // ���������� �̵�
		for (auto& vertex : shapes[selectStatus].vertices) {
			vertex.x += moveDistance;
		}
		cout << "d" << endl;
		break;
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

	static glm::vec3 lineStart; // ���� �������� ����
	static bool lineStarted = false; // ���� ���۵Ǿ����� ����

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//std::cout << "x = " << x << " y = " << y << std::endl;

		float glX = (x / (float)width) * 2.0f - 1.0f; // x ��ǥ ��ȯ
		float glY = 1.0f - (y / (float)height) * 2.0f; // y ��ǥ ��ȯ

		// ��ȯ�� ��ǥ�� �ֿܼ� ���
		std::cout << "x = " << glX << ", y = " << glY << std::endl;

		if (shapes.size() > 10)
			return;

		switch (statusType)
		{
		case TRIANGLE:
			insert_triangle(
				glm::vec3(glX - 0.1f, glY - 0.1f, 0.0f),
				glm::vec3(glX + 0.1f, glY - 0.1f, 0.0f),
				glm::vec3(glX, glY + 0.1f, 0.0f), 
				glm::vec3(rand_color(gen), rand_color(gen), rand_color(gen)),
				shapes
			);
			break;
		case SQUARE:
			// �簢�� ���� �߰� (0.1 ũ���)
			insert_square(
				glm::vec3(glX - 0.1f, glY - 0.1f, 0.0f), // ���� �Ʒ�
				glm::vec3(glX + 0.1f, glY - 0.1f, 0.0f), // ������ �Ʒ�
				glm::vec3(glX + 0.1f, glY + 0.1f, 0.0f), // ������ ��
				glm::vec3(glX - 0.1f, glY + 0.1f, 0.0f), // ���� ��
				glm::vec3(glX - 0.1f, glY - 0.1f, 0.0f), // ���� �Ʒ� (�ߺ�)
				glm::vec3(glX + 0.1f, glY + 0.1f, 0.0f), // ������ �� (�ߺ�)
				glm::vec3(rand_color(gen), rand_color(gen), rand_color(gen)), // ����
				shapes
			);
			break;
		case POINT_:
			insert_point(glm::vec3(glX, glY, 0.0f), glm::vec3(rand_color(gen), rand_color(gen), rand_color(gen)), shapes);
			break;
		case LINE:
			if (!lineStarted) {
				lineStart = glm::vec3(glX, glY, 0.0f); // ���� ������ ����
				lineStarted = true; // ���� ���۵�
			}
			else {
				// ���� ������ �߰��ϰ� ���� ����
				insert_line(lineStart, glm::vec3(glX, glY, 0.0f), glm::vec3(rand_color(gen), rand_color(gen), rand_color(gen)), shapes);
				lineStarted = false; // �� ������ �ʱ�ȭ
			}
			break;
		default:
			break;
		}
	}
	InitBuffer();
	glutPostRedisplay();
}

int main(int argc, char** argv) {

	width = 800;
	height = 600;

	//������ ����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("�ǽ� 7��");

	//glew �ʱ�ȭ
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	make_shaderProgram();
	//initShapes();
	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyBoard);
	glutMouseFunc(mouse);
	glutMainLoop();

	return 0;
}

void drawShapes() {
	int currentIndex = 0; // ���� ���� �ε���

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

	// shapes ���Ͱ� ��� �ִ��� Ȯ��
	if (shapes.empty()) {
		cout << "No shapes to initialize buffers." << endl;
		return; // shapes�� ��������� �ʱ�ȭ �ߴ�
	}

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