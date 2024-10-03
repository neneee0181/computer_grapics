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
vector<Shape_gl> shapes;

GLuint vao, vbo[2];

ShapeType statusType = ShapeType::TRIANGLE;
int selectStatus = 0;

void initShapes() {
	// 삼각형 추가

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
	case 'p': // 점
		statusType = ShapeType::POINT_;
		selectStatus = rand_select(gen);
		break;
	case 'l': //선
		statusType = ShapeType::LINE;
		selectStatus = rand_select(gen);
		break;
	case 't': //삼각형
		statusType = ShapeType::TRIANGLE;
		selectStatus = rand_select(gen);
		break;
	case 'r': //사각형
		statusType = ShapeType::SQUARE;
		selectStatus = rand_select(gen);
		break;
	case 'w': // 위로 이동
		for (auto& vertex : shapes[selectStatus].vertices) {
			vertex.y += moveDistance;
		}
		cout << "w" << endl;
		break;
	case 's': // 아래로 이동
		for (auto& vertex : shapes[selectStatus].vertices) {
			vertex.y -= moveDistance;
		}
		cout << "s" << endl;
		break;
	case 'a': // 왼쪽으로 이동
		for (auto& vertex : shapes[selectStatus].vertices) {
			vertex.x -= moveDistance;
		}
		cout << "a" << endl;
		break;
	case 'd': // 오른쪽으로 이동
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

	static glm::vec3 lineStart; // 선의 시작점을 저장
	static bool lineStarted = false; // 선이 시작되었는지 여부

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//std::cout << "x = " << x << " y = " << y << std::endl;

		float glX = (x / (float)width) * 2.0f - 1.0f; // x 좌표 변환
		float glY = 1.0f - (y / (float)height) * 2.0f; // y 좌표 변환

		// 변환된 좌표를 콘솔에 출력
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
			// 사각형 정점 추가 (0.1 크기로)
			insert_square(
				glm::vec3(glX - 0.1f, glY - 0.1f, 0.0f), // 왼쪽 아래
				glm::vec3(glX + 0.1f, glY - 0.1f, 0.0f), // 오른쪽 아래
				glm::vec3(glX + 0.1f, glY + 0.1f, 0.0f), // 오른쪽 위
				glm::vec3(glX - 0.1f, glY + 0.1f, 0.0f), // 왼쪽 위
				glm::vec3(glX - 0.1f, glY - 0.1f, 0.0f), // 왼쪽 아래 (중복)
				glm::vec3(glX + 0.1f, glY + 0.1f, 0.0f), // 오른쪽 위 (중복)
				glm::vec3(rand_color(gen), rand_color(gen), rand_color(gen)), // 색상
				shapes
			);
			break;
		case POINT_:
			insert_point(glm::vec3(glX, glY, 0.0f), glm::vec3(rand_color(gen), rand_color(gen), rand_color(gen)), shapes);
			break;
		case LINE:
			if (!lineStarted) {
				lineStart = glm::vec3(glX, glY, 0.0f); // 선의 시작점 설정
				lineStarted = true; // 선이 시작됨
			}
			else {
				// 선의 끝점을 추가하고 선을 저장
				insert_line(lineStart, glm::vec3(glX, glY, 0.0f), glm::vec3(rand_color(gen), rand_color(gen), rand_color(gen)), shapes);
				lineStarted = false; // 선 시작점 초기화
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
	int currentIndex = 0; // 현재 정점 인덱스

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

	// shapes 벡터가 비어 있는지 확인
	if (shapes.empty()) {
		cout << "No shapes to initialize buffers." << endl;
		return; // shapes가 비어있으면 초기화 중단
	}

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