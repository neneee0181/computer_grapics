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

int cnt_shapes = 0;

GLuint vao, vbo[2];

// 스파이럴 관련 변수들
float angle = 0.0f;
float radius = 0.01f;  // 초기 반지름
float centerX, centerY; // 클릭한 좌표를 기준으로 스파이럴 시작
bool isSpiralActive = false; // 스파이럴 그리기 상태 확인
glm::vec3 lastPoint; // 마지막 점 저장
glm::vec3 spiralColor(1.0f, 1.0f, 1.0f); // 스파이럴 색상
int line_mode = 1;

void keyBoard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'p':
		line_mode = 0;
		break;
	case 'l':
		line_mode = 1;
		break;
	case '1':
		cnt_shapes = 1;
		break;
	case '2':
		cnt_shapes = 2;
		break;
	case '3':
		cnt_shapes = 3;
		break;
	case '4':
		cnt_shapes = 4;
		break;
	case '5':
		cnt_shapes = 5;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dd(-1.0f, 1.0f);
void make_spiral() {
	centerX = dd(gen);
	centerY = dd(gen);
	// 스파이럴 초기화
	angle = 0.0f;
	radius = 0.01f;
	lastPoint = glm::vec3(centerX, centerY, 0.0f);
	isSpiralActive = true;
	//glutTimerFunc(16, timer, 0); // 16ms 후에 다시 타이머 호출 (약 60FPS)
}


float PI_ = 3.14159265358979323846f;
int cnt_re_shapes = 0;

void timer(int value) {
	if (isSpiralActive && angle < 10.0f * PI_ && value == 0) { // 5바퀴가 될 때까지
		// 스파이럴의 다음 점 계산
		angle += 0.2f;  // 각도 증가
		radius += 0.0005f;  // 반지름 증가

		// 새로운 점 계산
		float x = centerX + radius * cos(angle);
		float y = centerY + radius * sin(angle);
		glm::vec3 newPoint(x, y, 0.0f);

		// 점선 추가
		insert_line(lastPoint, newPoint, spiralColor, shapes);
		lastPoint = newPoint; // 마지막 점 업데이트

	}
	else if (isSpiralActive && angle > 10.0f * PI_ && value == 0) {
		value = 1;
		centerX += 0.18;
		centerY += 0.035;
	}

	if (value == 1 && radius > 0.005f) { // 반지름이 너무 작아지지 않도록 제한
		// 스파이럴의 다음 점 계산
		angle += 0.2f;  // 각도 감소
		radius -= 0.0005f;  // 반지름 감소

		// 새로운 점 계산
		float x = centerX - radius * cos(angle);
		float y = centerY + radius * sin(angle);
		glm::vec3 newPoint(x, y, 0.0f);

		// 점선 추가
		insert_line(lastPoint, newPoint, spiralColor, shapes);
		lastPoint = newPoint; // 마지막 점 업데이트
	}
	else if (value == 1 && radius < 0.005f) {
		isSpiralActive = false;
		value = 0;
		cnt_re_shapes++;
		if (cnt_shapes != cnt_re_shapes) {
			make_spiral();
		}
	}

	// 버퍼 갱신
	InitBuffer();

	// 화면 갱신
	glutPostRedisplay();
	glutTimerFunc(16, timer, value); // 16ms 후에 다시 타이머 호출 (약 60FPS)
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		std::cout << "x = " << x << " y = " << y << std::endl;

		// OpenGL 좌표로 변환
		centerX = (x / (float)width) * 2.0f - 1.0f;
		centerY = 1.0f - (y / (float)height) * 2.0f;

		// 스파이럴 초기화
		angle = 0.0f;
		radius = 0.01f;
		lastPoint = glm::vec3(centerX, centerY, 0.0f);
		isSpiralActive = true;
		//glutTimerFunc(16, timer, 0); // 16ms 후에 다시 타이머 호출 (약 60FPS)
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
	glutCreateWindow("실습 10번");

	//glew 초기화
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	make_shaderProgram();

	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyBoard);
	glutMouseFunc(mouse);
	glutTimerFunc(16, timer, 0);
	glutMainLoop();

	return 0;
}

void drawShapes() {
	int currentIndex = 0; // 현재 정점 인덱스

	for (const Shape_gl& shape : shapes) {
		if (shape.type == LINE) {
			if (line_mode == 0) {
				glEnable(GL_LINE_STIPPLE);  // 점선 활성화
				glLineStipple(2, 0x00F1);   // 점선 패턴 설정 (적당히 긴 점선)
				glLineWidth(2.0f);          // 선 두께 설정
				glDrawArrays(GL_LINES, currentIndex, 2); // 선은 2개의 정점
				currentIndex += 2; // 선의 정점 수만큼 인덱스를 증가
				glDisable(GL_LINE_STIPPLE);  // 점선 비활성화
			}
			else {
				// 실선 그리기
				glLineWidth(2.0f);           // 실선의 두께 설정
				glDrawArrays(GL_LINES, currentIndex, 2);  // GL_LINES로 라인 그리기
				currentIndex += 2; // 선의 정점 수만큼 인덱스를 증가
			}
		}
	}
}

GLvoid drawScene() {

	if (cnt_re_shapes % 2 == 0) {
		glClearColor(0.0, 0.0, 0.0, 1.0f);
	}
	else {
		glClearColor(1.0, 0.0, 0.0, 1.0f);
	}
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