#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
//#include <gl/glm/glm.hpp>
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
	vector<glm::vec3> vertices; // 도형의 정점
	glm::vec3 color; // 도형의 색상
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

// 모든 도형을 담는 벡터
vector<Shape_gl> triangle;
vector<Shape_gl> systemShapes;
vector<Shape_gl> square;
vector<Shape_gl> penta;
vector<Shape_gl> line;

GLuint vao, vbo[2];

bool isAnimating_t = false;  // 애니메이션 상태
bool isAnimating2_t = false;  // 애니메이션 상태
bool isAnimating_s = false;
bool isAnimating2_s = false;
bool isAnimating_p = false;
bool isAnimating2_p = false;
bool isAnimating_l = false;

float stepSize = 0.01f;    // 한 번의 업데이트에서 이동할 거리

glm::vec3 lineStart(-0.8f, 0.3f, 0.0f);  // 선 시작 좌표
glm::vec3 lineEnd(-0.5f, 0.8f, 0.0f);    // 선 끝 좌표
glm::vec3 currentLinePos = lineStart;     // 현재 선의 위치

glm::vec3 triangleEnd(-0.2f, 0.3f, 0.0f);
glm::vec3 currentTriangle = lineEnd;

void updateLineToTriangle(int value) {
	if (value != 1) 
		return;
	if (isAnimating2_t) {
		// x좌표는 증가, y좌표는 감소하면서 목표로 이동
		if (abs(currentTriangle.x - triangleEnd.x) > stepSize) {
			currentTriangle.x += stepSize;
		}
		if (abs(currentTriangle.y - triangleEnd.y) > stepSize) {
			currentTriangle.y -= stepSize; // y좌표는 감소해야 함
		}

		// 삼각형이 완성되면 애니메이션 종료
		if (abs(currentTriangle.x - triangleEnd.x) <= stepSize && abs(currentTriangle.y - triangleEnd.y) <= stepSize) {
			isAnimating2_t = false;
		}

		// 도형을 다시 추가
		triangle.clear();
		insert_triangle(lineStart, lineEnd, currentTriangle, glm::vec3(0.0f, 0.0f, 0.0f), triangle);
		InitBuffer();
		glutPostRedisplay();  // 화면을 갱신

		// 애니메이션이 끝나지 않았으면 타이머 반복
		if (isAnimating2_t) {
			glutTimerFunc(16, updateLineToTriangle, value);
		}
	}
}

void updateLinePos(int value) {
	if (value != 0)
		return;
	if (isAnimating_t) {
		// 선이 끝 위치에 도달하지 않았다면 계속 이동
		if (currentLinePos.x < lineEnd.x && currentLinePos.y < lineEnd.y) {
			currentLinePos.x += stepSize;
			currentLinePos.y += stepSize;
		}
		else {
			isAnimating_t = false; // 선이 목표 위치에 도달하면 애니메이션 종료
			isAnimating2_t = true;
			value++;
			glutTimerFunc(0, updateLineToTriangle, value);
		}
		// 선을 shapes 벡터에 다시 추가
		triangle.clear();
		insert_line(lineStart, currentLinePos, glm::vec3(0.0f, 0.0f, 0.0f), triangle);
		InitBuffer();
		glutPostRedisplay();  // 화면을 갱신

		glutTimerFunc(16, updateLinePos, 0);
	}
}


glm::vec3 triangle1(0.2f, 0.3f, 0.0f);
glm::vec3 triangle2(0.8f, 0.3f, 0.0f);
glm::vec3 triangle3(0.5f, 0.8f, 0.0f);
glm::vec3 squareLastPoint1(0.2f, 0.8f, 0.0f);
glm::vec3 squareLastPoint2(0.8f, 0.8f, 0.0f);
glm::vec3 currentSquare1 = triangle3;
glm::vec3 currentSquare2 = triangle3;

void updateTriangelToSquare(int value) {
	if (value != 1)
		return;

	if (isAnimating2_s) {
		// 첫 번째 점을 이동시킴
		if (abs(currentSquare1.x - squareLastPoint1.x) > stepSize) {
			currentSquare1.x -= stepSize;
		}
		else {
			currentSquare1.x = squareLastPoint1.x; // 정확히 목표 좌표로 맞춤
		}

		// 두 번째 점도 같은 방식으로 이동
		if (abs(currentSquare2.x - squareLastPoint2.x) > stepSize) {
			currentSquare2.x += stepSize;
		}
		else {
			currentSquare2.x = squareLastPoint2.x; // 정확히 목표 좌표로 맞춤
		}

		// 두 점이 모두 목표 위치에 도달했는지 확인
		if (currentSquare1.x == squareLastPoint1.x && currentSquare2.x == squareLastPoint2.x) {
			isAnimating2_s = false; // 애니메이션 종료
		}
	}

	// 도형을 다시 추가
	square.clear();
	insert_triangle(triangle1, triangle2, currentSquare1, glm::vec3(0.0f, 0.0f, 0.0f), square);
	insert_triangle(currentSquare1, triangle2, currentSquare2, glm::vec3(0.0f, 0.0f, 0.0f), square);

	// 버퍼 초기화 및 화면 갱신
	InitBuffer();
	glutPostRedisplay();

	// 애니메이션이 끝나지 않았으면 타이머 반복
	if (isAnimating2_s) {
		glutTimerFunc(16, updateTriangelToSquare, 1);
	}
}

void updateMoveToTriangle(int value) {
	if (value != 0)
		return;

	if (isAnimating_s) {
		glm::vec3 translation(stepSize, 0.0f, 0.0f); // x축으로 stepSize만큼 이동

		// 모든 도형 정점에 동일한 translation을 적용하여 이동
		for (auto& sq : square) {
			for (auto& vertex : sq.vertices) {
				vertex += translation; // 각 정점에 이동 변화를 더해줍니다.
			}
		}

		// 도형이 목표 위치에 도달하면 애니메이션 종료
		if (square[0].vertices[0].x >= triangle1.x) {  // 목표 지점에 도달하면 종료
			isAnimating_s = false;
		}
	}
	else {
		isAnimating_s = false;
		isAnimating2_s = true;
		square.clear();
		insert_triangle(triangle1, triangle2, currentSquare1, glm::vec3(0.0f, 0.0f, 0.0f), square);
		glutTimerFunc(0, updateTriangelToSquare, 1);
		return;
	}

	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(16, updateMoveToTriangle, 0);
}

glm::vec3 squ[4] = {
	glm::vec3(-0.8f,-0.7f,0.0f),
	glm::vec3(-0.2f,-0.7f,0.0f),
	glm::vec3(-0.8f,-0.2f,0.0f),
	glm::vec3(-0.2f,-0.2f,0.0f)
};

glm::vec3 penta_r[6] = {
	glm::vec3(-0.65f,-0.7f,0.0f),
	glm::vec3(-0.35f,-0.7f,0.0f),
	glm::vec3(-0.75f,-0.4f,0.0f),
	glm::vec3(-0.25f,-0.4f,0.0f),
	glm::vec3(-0.5f,-0.2f,0.0f)
};

glm::vec3 currentSquare[5] = {
	glm::vec3(-0.8f, -0.7f, 0.0f),
	glm::vec3(-0.2f, -0.7f, 0.0f),
	glm::vec3(-0.8f, -0.2f, 0.0f),
	glm::vec3(-0.2f, -0.2f, 0.0f),
	glm::vec3(-0.5f,-0.2f,0.0f)
};

void updateSquaToPenta(int value) {
	if (value != 1)
		return;

	if (isAnimating2_p) {
		// 각 정점이 오각형 좌표로 이동하도록 설정
		if (abs(currentSquare[0].x - penta_r[0].x) > stepSize) {
			currentSquare[0].x += (currentSquare[0].x < penta_r[0].x) ? stepSize : -stepSize;
		}
		if (abs(currentSquare[1].x - penta_r[1].x) > stepSize) {
			currentSquare[1].x += (currentSquare[1].x < penta_r[1].x) ? stepSize : -stepSize;
		}
		if (abs(currentSquare[2].x - penta_r[2].x) > stepSize) {
			currentSquare[2].x += (currentSquare[2].x < penta_r[2].x) ? stepSize : -stepSize;
		}
		if (abs(currentSquare[3].x - penta_r[3].x) > stepSize) {
			currentSquare[3].x += (currentSquare[3].x < penta_r[3].x) ? stepSize : -stepSize;
		}
		if (abs(currentSquare[0].y - penta_r[0].y) > stepSize) {
			currentSquare[0].y += (currentSquare[0].y < penta_r[0].y) ? stepSize : -stepSize;
		}
		if (abs(currentSquare[1].y - penta_r[1].y) > stepSize) {
			currentSquare[1].y += (currentSquare[1].y < penta_r[1].y) ? stepSize : -stepSize;
		}
		if (abs(currentSquare[2].y - penta_r[2].y) > stepSize) {
			currentSquare[2].y += (currentSquare[2].y < penta_r[2].y) ? stepSize : -stepSize;
		}
		if (abs(currentSquare[3].y - penta_r[3].y) > stepSize) {
			currentSquare[3].y += (currentSquare[3].y < penta_r[3].y) ? stepSize : -stepSize;
		}

		// 정점들이 목표 위치에 도달했는지 확인
		if (abs(currentSquare[0].x - penta_r[0].x) <= stepSize &&
			abs(currentSquare[1].x - penta_r[1].x) <= stepSize &&
			abs(currentSquare[2].x - penta_r[2].x) <= stepSize &&
			abs(currentSquare[3].x - penta_r[3].x) <= stepSize) {
			isAnimating2_p = false; // 애니메이션 종료
		}

		// 도형을 다시 추가
		penta.clear();
		insert_triangle(currentSquare[0], currentSquare[1], currentSquare[2], glm::vec3(0.0f, 0.0f, 0.0f), penta);
		insert_triangle(currentSquare[2], currentSquare[1], currentSquare[3], glm::vec3(0.0f, 0.0f, 0.0f), penta);
		insert_triangle(currentSquare[2], currentSquare[3], currentSquare[4], glm::vec3(0.0f, 0.0f, 0.0f), penta);

		InitBuffer();
		glutPostRedisplay();

		// 애니메이션이 끝나지 않았으면 타이머 반복
		if (isAnimating2_p) {
			glutTimerFunc(16, updateSquaToPenta, 1);
		}
	}
}

void updateMoveToSquare(int value) {
	if (value != 0)
		return;

	if (isAnimating_p) {
		glm::vec3 translation(stepSize, stepSize, 0.0f); // x축으로 stepSize만큼 이동

		// 모든 도형 정점에 동일한 translation을 적용하여 이동
		for (auto& sq : penta) {
			for (auto& vertex : sq.vertices) {
					vertex -= translation; // 각 정점에 이동 변화를 더해줍니다.
			}
		}

		// 도형이 목표 위치에 도달하면 애니메이션 종료
		if (penta[0].vertices[0].x <= squ[0].x && penta[0].vertices[0].y <= squ[0].y) {  // 목표 지점에 도달하면 종료
			isAnimating_p = false;
		}

	}
	else {
		isAnimating_p = false;
		isAnimating2_p = true;
		penta.clear();
		insert_triangle(squ[0], squ[1], squ[2], glm::vec3(0.0f, 0.0f, 0.0f), penta);
		insert_triangle(squ[2], squ[1], squ[3], glm::vec3(0.0f, 0.0f, 0.0f), penta);
		glutTimerFunc(0, updateSquaToPenta, 1);
		return;
	}

	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(16, updateMoveToSquare, 0);
}

void updateMoveToLine(int value) {
	if (value != 0)
		return;

	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(16, updateMoveToLine, 0);
}

void keyBoard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'p': // 점
		if (penta.size()!=0) {
			if (!isAnimating_l) { // 잇

				insert_triangle(currentSquare[0], currentSquare[1], currentSquare[2], glm::vec3(0.0f, 0.0f, 0.0f), line);
				insert_triangle(currentSquare[2], currentSquare[1], currentSquare[3], glm::vec3(0.0f, 0.0f, 0.0f), line);
				insert_triangle(currentSquare[2], currentSquare[3], currentSquare[4], glm::vec3(0.0f, 0.0f, 0.0f), line);
				isAnimating_l = true;
				glutTimerFunc(0, updateMoveToLine, 0);
			}
		}
		else { // 없
			if (!isAnimating_l) {

			}
		}
		break;
	case 'l': //선
		if (!isAnimating_t) {
			isAnimating_t = true;
			currentLinePos = lineStart;  // 현재 위치를 다시 시작점으로 설정
			glutTimerFunc(0, updateLinePos, 0);  // 타이머 시작
		}
		break;
	case 't': //삼각형
		if (triangle.size() != 0) { // triangle이 있을때
			if (!isAnimating_s) {
				insert_triangle(lineStart, lineEnd, triangleEnd, glm::vec3(0.0f, 0.0f, 0.0f), square);

				isAnimating_s = true; 
				glutTimerFunc(0, updateMoveToTriangle, 0);
			}
		}
		else { //없을때
			if (!isAnimating_s) {
				square.clear();
				insert_triangle(triangle1, triangle2, currentSquare1, glm::vec3(0.0f, 0.0f, 0.0f), square);

				isAnimating2_s = true;
				glutTimerFunc(0, updateTriangelToSquare, 1);
			}
		}
		break;
	case 'r': //사각형
		if (square.size() != 0) { // 사각형 있을때
			insert_triangle(triangle1, triangle2, squareLastPoint1, glm::vec3(0.0f, 0.0f, 0.0f), penta);
			insert_triangle(squareLastPoint1, triangle2, squareLastPoint2, glm::vec3(0.0f, 0.0f, 0.0f), penta);
			isAnimating_p = true;
			glutTimerFunc(0, updateMoveToSquare, 0);
		}
		else { //업승ㄹ때
			insert_triangle(squ[0], squ[1], squ[2], glm::vec3(0.0f, 0.0f, 0.0f), penta);
			insert_triangle(squ[2], squ[1], squ[3], glm::vec3(0.0f, 0.0f, 0.0f), penta);
			isAnimating2_p = true;
			glutTimerFunc(0, updateSquaToPenta, 1);
		}
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

	//윈도우 생성
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("실습 11번");

	//glew 초기화
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

int currentIndex = 0; // 현재 정점 인덱스

void drawShapes() {

	for (const Shape_gl& shape : triangle) {
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

	for (const Shape_gl& shape : square) {
		if (shape.type == TRIANGLE) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 3); // 삼각형은 3개의 정점
			currentIndex += 3; // 삼각형의 정점 수만큼 인덱스를 증가
		}
		else if (shape.type == SQUARE) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 6); // 사각형은 2개의 삼각형 (6개의 정점)
			currentIndex += 6; // 사각형의 정점 수만큼 인덱스를 증가
		}
	}

	for (const Shape_gl& shape : penta) {
		if (shape.type == TRIANGLE) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 3); // 삼각형은 3개의 정점
			currentIndex += 3; // 삼각형의 정점 수만큼 인덱스를 증가
		}
	}

	for (const Shape_gl& shape : line) {
		if (shape.type == LINE) {
			glLineWidth(2.0f); // 선 두께 설정
			glDrawArrays(GL_LINES, currentIndex, 2); // 선은 2개의 정점
			currentIndex += 2; // 선의 정점 수만큼 인덱스를 증가
		}
		if (shape.type == TRIANGLE) {
			glDrawArrays(GL_TRIANGLES, currentIndex, 3); // 삼각형은 3개의 정점
			currentIndex += 3; // 삼각형의 정점 수만큼 인덱스를 증가
		}
	}
}

void drawSystemShape() {
	for (const Shape_gl& shape : systemShapes) {
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

	for (const Shape_gl& shape : triangle) {
		// 정점 추가
		allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

		// 색상 추가 (정점의 수만큼 색상 추가)
		for (size_t i = 0; i < shape.vertices.size(); ++i) {
			allColors.push_back(shape.color);
		}
	}

	for (const Shape_gl& shape : square) {
		// 정점 추가
		allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

		// 색상 추가 (정점의 수만큼 색상 추가)
		for (size_t i = 0; i < shape.vertices.size(); ++i) {
			allColors.push_back(shape.color);
		}
	}

	for (const Shape_gl& shape : penta) {
		// 정점 추가
		allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

		// 색상 추가 (정점의 수만큼 색상 추가)
		for (size_t i = 0; i < shape.vertices.size(); ++i) {
			allColors.push_back(shape.color);
		}
	}

	for (const Shape_gl& shape : line) {
		// 정점 추가
		allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

		// 색상 추가 (정점의 수만큼 색상 추가)
		for (size_t i = 0; i < shape.vertices.size(); ++i) {
			allColors.push_back(shape.color);
		}
	}

	for (const Shape_gl& shape : systemShapes) {
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