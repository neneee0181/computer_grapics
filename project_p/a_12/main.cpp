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
	vector<glm::vec3> vertices; // 도형의 정점
	glm::vec3 color; // 도형의 색상
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

// 모든 도형을 담는 벡터
vector<Shape_gl> shapes;

GLuint vao, vbo[2];

float stepSize = 0.01f;    // 한 번의 업데이트에서 이동할 거리

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

	// 기존 선분 생성 코드
	for (int i = 0; i < 3; ++i) { // 선분 생성
		x = dis(gen);
		y = dis(gen);
		float x2 = dis(gen), y2 = dis(gen);
		insert_line(glm::vec3(x, y, 0.0f), glm::vec3(x2, y2, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), shapes);
	}

	// 삼각형 생성 코드
	std::uniform_real_distribution<> size_dis(0.05f, 0.1f);  // 삼각형과 사각형, 오각형 크기를 0.05 ~ 0.1 사이에서 랜덤하게 생성

	for (int i = 0; i < 3; ++i) { // 삼각형을 3개 생성
		// 삼각형의 한 정점의 기준 좌표를 랜덤하게 설정
		glm::vec3 v1(dis(gen), dis(gen), 0.0f);

		// 랜덤한 크기의 삼각형을 위한 크기 설정
		float size = size_dis(gen);  // 삼각형의 크기를 결정하는 랜덤 길이

		// 첫 번째 정점으로부터 일정 거리만큼 떨어진 나머지 두 정점 설정
		glm::vec3 v2(v1.x + size, v1.y, 0.0f);               // x 방향으로 일정 거리
		glm::vec3 v3(v1.x + size / 2.0f, v1.y + size, 0.0f); // 대각선 방향으로 일정 거리

		// 랜덤한 색상
		glm::vec3 color(dis(gen), dis(gen), dis(gen));

		// 삼각형 추가
		insert_triangle(v1, v2, v3, color, shapes);
	}

	// 사각형 생성 코드
	for (int i = 0; i < 3; ++i) { // 사각형을 3개 생성
		// 사각형의 왼쪽 아래 정점의 기준 좌표를 랜덤하게 설정
		glm::vec3 v1(dis(gen), dis(gen), 0.0f);

		// 랜덤한 크기의 사각형을 위한 크기 설정
		float size = size_dis(gen);  // 사각형의 크기를 결정하는 랜덤 길이

		// 사각형의 나머지 정점 설정
		glm::vec3 v2(v1.x + size, v1.y, 0.0f);               // 오른쪽 아래
		glm::vec3 v3(v1.x, v1.y + size, 0.0f);               // 왼쪽 위
		glm::vec3 v4(v1.x + size, v1.y + size, 0.0f);        // 오른쪽 위

		// 사각형을 두 개의 삼각형으로 나누기 위한 정점 추가
		// v1, v2, v3로 첫 번째 삼각형
		// v2, v3, v4로 두 번째 삼각형
		glm::vec3 color(dis(gen), dis(gen), dis(gen)); // 랜덤한 색상

		// 사각형 추가 (2개의 삼각형으로 구성)
		insert_square(v1, v2, v3, v2, v3, v4, color, shapes);
	}

	// 오각형 생성 코드
	for (int i = 0; i < 3; ++i) { // 오각형을 3개 생성
		// 오각형의 중심 좌표 설정
		glm::vec3 center(dis(gen), dis(gen), 0.0f);

		// 랜덤한 크기의 오각형을 위한 크기 설정
		float radius = size_dis(gen);  // 오각형의 반지름 크기

		// 오각형의 5개의 정점 생성 (각도 계산)
		std::vector<glm::vec3> pentagonVertices;
		for (int j = 0; j < 5; ++j) {
			float angle = j * (2.0f * M_PI / 5);  // 360도를 5등분
			float x = center.x + radius * cos(angle);
			float y = center.y + radius * sin(angle);
			pentagonVertices.push_back(glm::vec3(x, y, 0.0f));
		}

		// 랜덤한 색상
		glm::vec3 color(dis(gen), dis(gen), dis(gen));

		// 오각형을 삼각형으로 나누기 (중심점과 인접한 두 정점으로 삼각형을 만듦)
		for (int j = 0; j < 5; ++j) {
			glm::vec3 v1 = pentagonVertices[j];
			glm::vec3 v2 = pentagonVertices[(j + 1) % 5];  // 다음 정점 (인덱스 순환)
			insert_triangle(center, v1, v2, color, shapes);
		}
	}
}

int main(int argc, char** argv) {

	width = 800;
	height = 600;

	//윈도우 생성
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("실습 12번");

	//glew 초기화
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

int currentIndex = 0; // 현재 정점 인덱스

void drawShapes() {

	for (const Shape_gl& shape : shapes) {
		if (shape.type == TRIANGLE) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 3); // 삼각형은 3개의 정점
			currentIndex += 3; // 삼각형의 정점 수만큼 인덱스를 증가
		}
		else if (shape.type == SQUARE) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 6); // 사각형은 2개의 삼각형 (6개의 정점)
			currentIndex += 6; // 사각형의 정점 수만큼 인덱스를 증가
		}
		else if (shape.type == POINT_) {
			glPointSize(10.0f); // 점 크기 설정
			glDrawArrays(GL_POINTS, currentIndex, 1); // 점은 1개의 정점
			currentIndex += 1; // 점의 정점 수만큼 인덱스를 증가
		}
		else if (shape.type == LINE) {
			glLineWidth(2.0f); // 선 두께 설정
			glDrawArrays(GL_LINES, currentIndex, 2); // 선은 2개의 정점
			currentIndex += 2; // 선의 정점 수만큼 인덱스를 증가
		}
	}
}

GLvoid drawScene() {

	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	glBindVertexArray(vao);
	currentIndex = 0;  // 매번 그리기 시작할 때 정점 인덱스를 0으로 초기화
	drawShapes(); // 도형 그리기
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
		cerr << "ERROR: vertex shader 컴파일 실패 \n" << errorLog << endl;
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
		cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << endl;
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

	// 모든 도형의 정점을 한 배열에 담기
	vector<glm::vec3> allVertices;
	vector<glm::vec3> allColors;

	for (const Shape_gl& shape : shapes) {
		// 정점 추가
		allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

		// 색상 추가 (정점의 수만큼 색상 추가)
		for (size_t i = 0; i < shape.vertices.size(); ++i) {
			allColors.push_back(shape.color);
		}
	}

	// 정점 데이터 OpenGL에 전송
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(glm::vec3), allVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 색상 데이터 OpenGL에 전송
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, allColors.size() * sizeof(glm::vec3), allColors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}