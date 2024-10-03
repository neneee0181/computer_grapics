#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

GLfloat glfloat1_color[3] = { static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX };
GLfloat glfloat2_color[3] = { static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX };
GLfloat glfloat3_color[3] = { static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX };
GLfloat glfloat4_color[3] = { static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX };

GLfloat glfloat1_bk_color[3] = { static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX };
GLfloat glfloat2_bk_color[3] = { static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX };
GLfloat glfloat3_bk_color[3] = { static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX };
GLfloat glfloat4_bk_color[3] = { static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX ,static_cast<float>(rand()) / RAND_MAX };

GLfloat glfloat1_rect[4] = { -0.8f, 0.8f, 0.0f, 0.0f };
GLfloat glfloat2_rect[4] = { 0.0f, 0.0f, 0.8f, 0.8f };
GLfloat glfloat3_rect[4] = { -0.8f, -0.8f, 0.0f, 0.0f };
GLfloat glfloat4_rect[4] = { 0.0f, 0.0f, 0.8f, -0.8f };

GLfloat glfloat1_bk_rect[4] = { -1.0f, 1.0f, 0.0f, 0.0f };
GLfloat glfloat2_bk_rect[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat glfloat3_bk_rect[4] = { -1.0f, -1.0f, 0.0f, 0.0f };
GLfloat glfloat4_bk_rect[4] = { 0.0f, 0.0f, 1.0f, -1.0f };


void changeColor(GLfloat color[3]) {
	color[0] = static_cast<float>(rand()) / RAND_MAX;
	color[1] = static_cast<float>(rand()) / RAND_MAX;
	color[2] = static_cast<float>(rand()) / RAND_MAX;
}

void changeRect(GLfloat rect[4], int status) {

	if (status == 0) {
		rect[0] = rect[0] + 0.01;
		rect[1] = rect[1] + 0.01;
		rect[2] = rect[2] - 0.01;
		rect[3] = rect[3] - 0.01;
	}
	else if (status == 1) {
		rect[0] = rect[0] + 0.01;
		rect[1] = rect[1] - 0.01;
		rect[2] = rect[2] - 0.01;
		rect[3] = rect[3] + 0.01;
	}
	else if (status == 2){
		rect[0] = rect[0] - 0.01;
		rect[1] = rect[1] - 0.01;
		rect[2] = rect[2] + 0.01;
		rect[3] = rect[3] + 0.01;
	}
	else if (status == 3) {
		rect[0] = rect[0] - 0.01;
		rect[1] = rect[1] + 0.01;
		rect[2] = rect[2] + 0.01;
		rect[3] = rect[3] - 0.01;
	}
}

void Mouse(int button, int state, int x, int y) {

	int w = glutGet(GLUT_WINDOW_WIDTH);  // 윈도우 너비
	int h = glutGet(GLUT_WINDOW_HEIGHT); // 윈도우 높이

	// 마우스 좌표를 OpenGL 좌표로 변환
	GLfloat mouseX = (float)x / (w / 2) - 1.0f;
	GLfloat mouseY = 1.0f - (float)y / (h / 2);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//cout << "x = " << x << ", y = " << y << endl;
		cout << "mX = " << mouseX << ", mY = " << mouseY << endl;

		if (mouseX >= glfloat1_rect[0] && mouseX <= glfloat1_rect[2] &&
			mouseY <= glfloat1_rect[1] && mouseY >= glfloat1_rect[3]) {
			changeColor(glfloat1_color);
		}
		else if (mouseX >= glfloat1_bk_rect[0] && mouseX <= glfloat1_bk_rect[2] &&
			mouseY <= glfloat1_bk_rect[1] && mouseY >= glfloat1_bk_rect[3]) {
			changeColor(glfloat1_bk_color); // 1번 도형 배경 클릭 시
			return;
		}

		if (mouseX >= glfloat2_rect[0] && mouseX <= glfloat2_rect[2] &&
			mouseY >= glfloat2_rect[1] && mouseY <= glfloat2_rect[3]) {
			changeColor(glfloat2_color);
		}
		else if (mouseX >= glfloat2_bk_rect[0] && mouseX <= glfloat2_bk_rect[2] &&
			mouseY >= glfloat2_bk_rect[1] && mouseY <= glfloat2_bk_rect[3]) {
			changeColor(glfloat2_bk_color); // 2번 도형 배경 클릭 시
			return;
		}

		if (mouseX >= glfloat3_rect[0] && mouseX <= glfloat3_rect[2] &&
			mouseY >= glfloat3_rect[1] && mouseY <= glfloat3_rect[3]) {
			changeColor(glfloat3_color);
		}
		else if (mouseX >= glfloat3_bk_rect[0] && mouseX <= glfloat3_bk_rect[2] &&
			mouseY >= glfloat3_bk_rect[1] && mouseY <= glfloat3_bk_rect[3]) {
			changeColor(glfloat3_bk_color); // 3번 도형 배경 클릭 시
			return;
		}

		if (mouseX >= glfloat4_rect[0] && mouseX <= glfloat4_rect[2] &&
			mouseY <= glfloat4_rect[1] && mouseY >= glfloat4_rect[3]) {
			changeColor(glfloat4_color);
		}
		else if (mouseX >= glfloat4_bk_rect[0] && mouseX <= glfloat4_bk_rect[2] &&
			mouseY <= glfloat4_bk_rect[1] && mouseY >= glfloat4_bk_rect[3]) {
			changeColor(glfloat4_bk_color); // 4번 도형 배경 클릭 시
			return;
		}

	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (mouseX >= glfloat1_rect[0] && mouseX <= glfloat1_rect[2] &&
			mouseY <= glfloat1_rect[1] && mouseY >= glfloat1_rect[3]) {
			changeRect(glfloat1_rect, 1);
		}
		else if (mouseX >= glfloat1_bk_rect[0] && mouseX <= glfloat1_bk_rect[2] &&
			mouseY <= glfloat1_bk_rect[1] && mouseY >= glfloat1_bk_rect[3]) {
			changeRect(glfloat1_rect, 3);
			return;
		}

		if (mouseX >= glfloat2_rect[0] && mouseX <= glfloat2_rect[2] &&
			mouseY >= glfloat2_rect[1] && mouseY <= glfloat2_rect[3]) {
			changeRect(glfloat2_rect, 0);
		}
		else if (mouseX >= glfloat2_bk_rect[0] && mouseX <= glfloat2_bk_rect[2] &&
			mouseY >= glfloat2_bk_rect[1] && mouseY <= glfloat2_bk_rect[3]) {
			changeRect(glfloat2_rect, 2);
			return;
		}

		if (mouseX >= glfloat3_rect[0] && mouseX <= glfloat3_rect[2] &&
			mouseY >= glfloat3_rect[1] && mouseY <= glfloat3_rect[3]) {
			changeRect(glfloat3_rect, 0);
		}
		else if (mouseX >= glfloat3_bk_rect[0] && mouseX <= glfloat3_bk_rect[2] &&
			mouseY >= glfloat3_bk_rect[1] && mouseY <= glfloat3_bk_rect[3]) {
			changeRect(glfloat3_rect, 2);
			return;
		}

		if (mouseX >= glfloat4_rect[0] && mouseX <= glfloat4_rect[2] &&
			mouseY <= glfloat4_rect[1] && mouseY >= glfloat4_rect[3]) {
			changeRect(glfloat4_rect, 1);
		}
		else if (mouseX >= glfloat4_bk_rect[0] && mouseX <= glfloat4_bk_rect[2] &&
			mouseY <= glfloat4_bk_rect[1] && mouseY >= glfloat4_bk_rect[3]) {
			changeRect(glfloat4_rect, 3);
			return;
		}
	}
}

void main(int argc, char** argv) {
	//윈도우 생성
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	glutCreateWindow("실습2번");

	//glew 초기화
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//그리기 부분 구현

	glColor3f(glfloat1_bk_color[0], glfloat1_bk_color[1], glfloat1_bk_color[2]);
	glRectf(glfloat1_bk_rect[0], glfloat1_bk_rect[1], glfloat1_bk_rect[2], glfloat1_bk_rect[3]);
	glColor3f(glfloat1_color[0], glfloat1_color[1], glfloat1_color[2]);
	glRectf(glfloat1_rect[0], glfloat1_rect[1], glfloat1_rect[2], glfloat1_rect[3]);

	glColor3f(glfloat2_bk_color[0], glfloat2_bk_color[1], glfloat2_bk_color[2]);
	glRectf(glfloat2_bk_rect[0], glfloat2_bk_rect[1], glfloat2_bk_rect[2], glfloat2_bk_rect[3]);
	glColor3f(glfloat2_color[0], glfloat2_color[1], glfloat2_color[2]);
	glRectf(glfloat2_rect[0], glfloat2_rect[1], glfloat2_rect[2], glfloat2_rect[3]);

	glColor3f(glfloat3_bk_color[0], glfloat3_bk_color[1], glfloat3_bk_color[2]);
	glRectf(glfloat3_bk_rect[0], glfloat3_bk_rect[1], glfloat3_bk_rect[2], glfloat3_bk_rect[3]);
	glColor3f(glfloat3_color[0], glfloat3_color[1], glfloat3_color[2]);
	glRectf(glfloat3_rect[0], glfloat3_rect[1], glfloat3_rect[2], glfloat3_rect[3]);

	glColor3f(glfloat4_bk_color[0], glfloat4_bk_color[1], glfloat4_bk_color[2]);
	glRectf(glfloat4_bk_rect[0], glfloat4_bk_rect[1], glfloat4_bk_rect[2], glfloat4_bk_rect[3]);
	glColor3f(glfloat4_color[0], glfloat4_color[1], glfloat4_color[2]);
	glRectf(glfloat4_rect[0], glfloat4_rect[1], glfloat4_rect[2], glfloat4_rect[3]);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}