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
	vector<glm::vec3> vertices; // 도형의 정점
	glm::vec3 color; // 도형의 색상
	int quadrant = 0;
};


void insert_triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 color, vector<Shape_gl>& shapes, int quadrant) {
	shapes.push_back(
		{
			TRIANGLE,
		{v1,v2,v3},
		color,
		quadrant
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
vector<Shape_gl> systemShapes;

GLuint vao, vbo[2];

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> rand_(0.0f, 1.0f);
uniform_real_distribution<> size_(0.1f, 0.15f);
void initShapes() {
	// 삼각형 추가

	/*insert_triangle(glm::vec3(-0.7f, -0.5f, 0.0f), glm::vec3(0.7f, -0.5f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), shapes);
	insert_square(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(-0.5f, 0.5f, 0.0f),
		glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), shapes);
	insert_point(glm::vec3(0.0f, 0.9f, 0.4f), glm::vec3(0.0f, 0.0f, 1.0f), shapes);
	insert_line(glm::vec3(-0.9f, 0.0f, 0.0f), glm::vec3(0.9f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), shapes);*/



}

int findQuadrant(float x, float y) { // 사분면  찾아서 주기
	if (x < 0 && y > 0)
		return 1;
	else if (x > 0 && y > 0)
		return 2;
	else if (x < 0 && y < 0)
		return 3;
	else if (x > 0 && y < 0)
		return 4;
	return 0;
}

void keyBoard(unsigned char key, int x, int y) {
	switch (key)
	{
	default:
		break;
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		std::cout << "x = " << x << " y = " << y << std::endl;

		float glX = (x / (float)width) * 2.0f - 1.0f; // x 좌표 변환
		float glY = 1.0f - (y / (float)height) * 2.0f; // y 좌표 변환

		/*int quadmant = findQuadrant(glX, glY);

		for (int i = 0; i < shapes.size(); ++i) {
			if (shapes[i].quadrant == quadmant) {
				shapes.erase(shapes.begin() + i);
				break;
			}
		}*/

		// 랜덤한 크기와 색상 생성
		float triangleSize = size_(gen); // 랜덤한 삼각형 크기
		glm::vec3 randomColor(rand_(gen), rand_(gen), rand_(gen)); // 랜덤 색상

		// 삼각형의 세 개의 정점을 정의 (클릭한 위치를 기준으로)
		glm::vec3 v1(glX, glY, 0.0f); // 클릭한 위치가 첫 번째 정점
		glm::vec3 v2(glX + triangleSize, glY, 0.0f);
		glm::vec3 v3(glX + triangleSize / 2.0f, glY + triangleSize, 0.0f);

		// 사분면 찾기
		int quadrant = findQuadrant(glX, glY);

		// 새로운 삼각형 삽입
		insert_triangle(v1, v2, v3, randomColor, shapes, quadrant);


	}
	
	InitBuffer();
	
}

int main(int argc, char** argv) {

	width = 800;
	height = 600;

	//윈도우 생성
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("실습 8번");

	//glew 초기화
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	make_shaderProgram();

	insert_line(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), systemShapes); // 기본 안지워지는 선
	insert_line(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), systemShapes);

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
		else if (shape.type == LINE) {
			glLineWidth(2.0f); // 선 두께 설정
			glDrawArrays(GL_LINES, currentIndex, 2); // 선은 2개의 정점
			currentIndex += 2; // 선의 정점 수만큼 인덱스를 증가
		}
	}
}

void drawSystemShapes() {

	for (const Shape_gl& systemShape : systemShapes) {
		if (systemShape.type == LINE) {
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

	drawShapes(); // 도형 그리기
	drawSystemShapes();

	currentIndex = 0;

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

	// shapes 벡터에서 정점과 색상 추가
	for (const Shape_gl& shape : shapes) {
		// 정점 추가
		allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

		// 색상 추가 (정점의 수만큼 색상 추가)
		for (size_t i = 0; i < shape.vertices.size(); ++i) {
			allColors.push_back(shape.color);
		}
	}

	// systemShapes 벡터에서 정점과 색상 추가 (사분면 선)
	for (const Shape_gl& systemShape : systemShapes) {
		allVertices.insert(allVertices.end(), systemShape.vertices.begin(), systemShape.vertices.end());

		for (size_t i = 0; i < systemShape.vertices.size(); ++i) {
			allColors.push_back(systemShape.color);
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