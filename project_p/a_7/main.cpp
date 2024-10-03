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
	POINT_
};

struct Shape {
	ShapeType type;
	vector<glm::vec3> vertices; // 도형의 정점
	glm::vec3 color; // 도형의 색상
};

// 모든 도형을 담는 벡터
vector<Shape> shapes;

GLuint vao, vbo[2];

void initShapes() {
	// 삼각형 추가
	shapes.push_back({
		TRIANGLE,
		{ glm::vec3(-0.7f, -0.5f, 0.0f), glm::vec3(0.7f, -0.5f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f) },
		glm::vec3(1.0f, 0.0f, 0.0f) // 빨간색
		});

	shapes.push_back({
	  SQUARE,
	  { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, -0.5f, 0.0f), // 첫 번째 삼각형
		glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(-0.5f, 0.5f, 0.0f), // 두 번째 삼각형
		glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f) // 첫 번째 삼각형 (중복)
	  },
	  glm::vec3(0.0f, 1.0f, 0.0f) // 초록색
		});

	// 점 추가
	shapes.push_back({
		POINT_,
		{ glm::vec3(0.0f, 0.9f, 0.4f) },
		glm::vec3(0.0f, 0.0f, 1.0f) // 파란색
		});
}

void keyBoard(unsigned char key, int x, int y) {
	switch (key)
	{
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		std::cout << "x = " << x << " y = " << y << std::endl;
}

int main(int argc, char** argv) {

	width = 800;
	height = 600;

	//윈도우 생성
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("실습 7번");

	//glew 초기화
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	make_shaderProgram();
	initShapes();
	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyBoard);
	glutMouseFunc(mouse);
	glutMainLoop();

	return 0;
}

void drawShapes() {
	int currentIndex = 0; // 현재 정점 인덱스

	for (const Shape& shape : shapes) {
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
	}
}

GLvoid drawScene() {

	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	glBindVertexArray(vao);

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

	for (const Shape& shape : shapes) {
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