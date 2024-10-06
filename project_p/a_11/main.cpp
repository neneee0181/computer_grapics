#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>

#include <vector>

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
	LINE,
	PENTA_
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
vector<Shape_gl> triangle;
vector<Shape_gl> systemShapes;
vector<Shape_gl> square;
vector<Shape_gl> penta;
vector<Shape_gl> line;

GLuint vao, vbo[2];

bool isAnimating_t = false;  // �ִϸ��̼� ����
bool isAnimating2_t = false;  // �ִϸ��̼� ����
bool isAnimating_s = false;
bool isAnimating2_s = false;

float stepSize = 0.01f;    // �� ���� ������Ʈ���� �̵��� �Ÿ�

glm::vec3 lineStart(-0.8f, 0.3f, 0.0f);  // �� ���� ��ǥ
glm::vec3 lineEnd(-0.5f, 0.8f, 0.0f);    // �� �� ��ǥ
glm::vec3 currentLinePos = lineStart;     // ���� ���� ��ġ

glm::vec3 triangleEnd(-0.2f, 0.3f, 0.0f);
glm::vec3 currentTriangle = lineEnd;

void updateLineToTriangle(int value) {
	if (value != 1) 
		return;
	if (isAnimating2_t) {
		// x��ǥ�� ����, y��ǥ�� �����ϸ鼭 ��ǥ�� �̵�
		if (abs(currentTriangle.x - triangleEnd.x) > stepSize) {
			currentTriangle.x += stepSize;
		}
		if (abs(currentTriangle.y - triangleEnd.y) > stepSize) {
			currentTriangle.y -= stepSize; // y��ǥ�� �����ؾ� ��
		}

		// �ﰢ���� �ϼ��Ǹ� �ִϸ��̼� ����
		if (abs(currentTriangle.x - triangleEnd.x) <= stepSize && abs(currentTriangle.y - triangleEnd.y) <= stepSize) {
			isAnimating2_t = false;
		}

		// ������ �ٽ� �߰�
		triangle.clear();
		insert_triangle(lineStart, lineEnd, currentTriangle, glm::vec3(0.0f, 0.0f, 0.0f), triangle);
		InitBuffer();
		glutPostRedisplay();  // ȭ���� ����

		// �ִϸ��̼��� ������ �ʾ����� Ÿ�̸� �ݺ�
		if (isAnimating2_t) {
			glutTimerFunc(16, updateLineToTriangle, value);
		}
	}
}

void updateLinePos(int value) {
	if (value != 0)
		return;
	if (isAnimating_t) {
		// ���� �� ��ġ�� �������� �ʾҴٸ� ��� �̵�
		if (currentLinePos.x < lineEnd.x && currentLinePos.y < lineEnd.y) {
			currentLinePos.x += stepSize;
			currentLinePos.y += stepSize;
		}
		else {
			isAnimating_t = false; // ���� ��ǥ ��ġ�� �����ϸ� �ִϸ��̼� ����
			isAnimating2_t = true;
			value++;
			glutTimerFunc(0, updateLineToTriangle, value);
		}
		// ���� shapes ���Ϳ� �ٽ� �߰�
		triangle.clear();
		insert_line(lineStart, currentLinePos, glm::vec3(0.0f, 0.0f, 0.0f), triangle);
		InitBuffer();
		glutPostRedisplay();  // ȭ���� ����

		glutTimerFunc(16, updateLinePos, 0);
	}
}


glm::vec3 triangle1(0.2f, 0.3f, 0.0f);
glm::vec3 triangle2(0.8f, 0.3f, 0.0f);
glm::vec3 triangle3(0.5f, 0.8f, 0.0f);
glm::vec3 currentSquare = triangle2;

void updateTriangelToSquare(int value) {
	if (value != 1)
		return;

	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(16, updateTriangelToSquare, 0);
}

void updateMoveToTriangle(int value) {
	if (value != 0)
		return;

	if (isAnimating_s) {
		glm::vec3 translation(stepSize, 0.0f, 0.0f); // x������ stepSize��ŭ �̵�

		// ��� ���� ������ ������ translation�� �����Ͽ� �̵�
		for (auto& sq : square) {
			for (auto& vertex : sq.vertices) {
				vertex += translation; // �� ������ �̵� ��ȭ�� �����ݴϴ�.
			}
		}

		// ������ ��ǥ ��ġ�� �����ϸ� �ִϸ��̼� ����
		if (square[0].vertices[0].x >= triangle1.x) {  // ��ǥ ������ �����ϸ� ����
			isAnimating_s = false;
		}
	}
	else {

	}

	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(16, updateMoveToTriangle, 0);
}

void keyBoard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'p': // ��
		break;
	case 'l': //��
		if (!isAnimating_t) {
			isAnimating_t = true;
			currentLinePos = lineStart;  // ���� ��ġ�� �ٽ� ���������� ����
			glutTimerFunc(0, updateLinePos, 0);  // Ÿ�̸� ����
		}
		break;
	case 't': //�ﰢ��
		if (triangle.size() != 0) { // triangle�� ������
			if (!isAnimating_s) {
				insert_triangle(lineStart, lineEnd, triangleEnd, glm::vec3(0.0f, 0.0f, 0.0f), square);

				isAnimating_s = true; 
				currentSquare = triangle2;
				glutTimerFunc(0, updateMoveToTriangle, 0);
			}
		}
		else { //������
			if (!isAnimating_s) {
				insert_triangle(triangle1, triangle2, triangle3, glm::vec3(0.0f, 0.0f, 0.0f), square);

				isAnimating_s = true;
				currentSquare = triangle2;
				glutTimerFunc(0, updateTriangelToSquare, 1);
			}
		}
		break;
	case 'r': //�簢��
		break;
	case 'w':
		break;
	case 'a':
		break;
	case 's':
		break;
	case 'd':
		break;
	case 'c':
		triangle.clear();
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

int main(int argc, char** argv) {

	width = 800;
	height = 600;

	//������ ����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("�ǽ� 11��");

	//glew �ʱ�ȭ
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	make_shaderProgram();

	insert_line(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), systemShapes);
	insert_line(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), systemShapes);

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

	for (const Shape_gl& shape : triangle) {
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

	for (const Shape_gl& shape : square) {
		if (shape.type == TRIANGLE) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 3); // �ﰢ���� 3���� ����
			currentIndex += 3; // �ﰢ���� ���� ����ŭ �ε����� ����
		}
		else if (shape.type == SQUARE) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 6); // �簢���� 2���� �ﰢ�� (6���� ����)
			currentIndex += 6; // �簢���� ���� ����ŭ �ε����� ����
		}
	}

	for (const Shape_gl& shape : penta) {
		if (shape.type == SQUARE) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 6); // �簢���� 2���� �ﰢ�� (6���� ����)
			currentIndex += 6; // �簢���� ���� ����ŭ �ε����� ����
		}
		else if (shape.type == PENTA_) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 9); // �簢���� 2���� �ﰢ�� (6���� ����)
			currentIndex += 9; // �簢���� ���� ����ŭ �ε����� ����
		}
	}

	for (const Shape_gl& shape : line) {
		if (shape.type == PENTA_) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 9); // �簢���� 2���� �ﰢ�� (6���� ����)
			currentIndex += 9; // �簢���� ���� ����ŭ �ε����� ����
		}
		else if (shape.type == LINE) {
			glLineWidth(2.0f); // �� �β� ����
			glDrawArrays(GL_LINES, currentIndex, 2); // ���� 2���� ����
			currentIndex += 2; // ���� ���� ����ŭ �ε����� ����
		}
	}
}

void drawSystemShape() {
	for (const Shape_gl& shape : systemShapes) {
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
	drawSystemShape();
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

	for (const Shape_gl& shape : triangle) {
		// ���� �߰�
		allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

		// ���� �߰� (������ ����ŭ ���� �߰�)
		for (size_t i = 0; i < shape.vertices.size(); ++i) {
			allColors.push_back(shape.color);
		}
	}

	for (const Shape_gl& shape : square) {
		// ���� �߰�
		allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

		// ���� �߰� (������ ����ŭ ���� �߰�)
		for (size_t i = 0; i < shape.vertices.size(); ++i) {
			allColors.push_back(shape.color);
		}
	}

	for (const Shape_gl& shape : penta) {
		// ���� �߰�
		allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

		// ���� �߰� (������ ����ŭ ���� �߰�)
		for (size_t i = 0; i < shape.vertices.size(); ++i) {
			allColors.push_back(shape.color);
		}
	}

	for (const Shape_gl& shape : line) {
		// ���� �߰�
		allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

		// ���� �߰� (������ ����ŭ ���� �߰�)
		for (size_t i = 0; i < shape.vertices.size(); ++i) {
			allColors.push_back(shape.color);
		}
	}

	for (const Shape_gl& shape : systemShapes) {
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