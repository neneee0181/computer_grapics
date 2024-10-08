#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <random>

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis(0.0, 1.0);
uniform_real_distribution<> size_dis(0.05, 0.2); // 도형 크기 범위를 지정

struct rect {
	GLfloat x1, y1, x2, y2;
	GLfloat color1, color2, color3;
	GLfloat save_x1, save_y1, save_x2, save_y2;
	int move_status = 0; // 0 -> stop
	int arrow_x = 2; // 1->  2 <- 
	int arrow_y = 2;
};

vector<rect> rect_list;

void checkArrow2() {
	for (auto& r : rect_list) {
		if (r.x2 >= 1.0f)
			r.arrow_x = 2;
		else if (r.x1 <= -1.0f)
			r.arrow_x = 1;
		if (r.y2 >= 1.0f)
			r.arrow_y = 2;
		else if (r.y1 <= -1.0f)
			r.arrow_y = 1;
	}
}

void timer(int value) {

	for (int i = 0; i < rect_list.size(); ++i) {
		switch (rect_list[i].move_status)
		{
		case 1:
			checkArrow2();
			if (rect_list[i].arrow_x == 1) {
				rect_list[i].x1 = rect_list[i].x1 + 0.01;
				rect_list[i].x2 = rect_list[i].x2 + 0.01;
			}
			else if (rect_list[i].arrow_x == 2) {
				rect_list[i].x1 = rect_list[i].x1 - 0.01;
				rect_list[i].x2 = rect_list[i].x2 - 0.01;
			}

			if (rect_list[i].arrow_y == 1) {
				rect_list[i].y1 = rect_list[i].y1 + 0.01;
				rect_list[i].y2 = rect_list[i].y2 + 0.01;
			}
			else if (rect_list[i].arrow_y == 2) {
				rect_list[i].y1 = rect_list[i].y1 - 0.01;
				rect_list[i].y2 = rect_list[i].y2 - 0.01;
			}
			break;
		case 2:
			checkArrow2();
			if (rect_list[i].arrow_x == 1) {
				rect_list[i].x1 = rect_list[i].x1 + 0.01;
				rect_list[i].x2 = rect_list[i].x2 + 0.01;
			}
			else if (rect_list[i].arrow_x == 2) {
				rect_list[i].x1 = rect_list[i].x1 - 0.01;
				rect_list[i].x2 = rect_list[i].x2 - 0.01;
			}
			break;
		case 3:
			// Do nothing for case 3 (if needed, add behavior here)
			break;
		default:
			break;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(16, timer, 0); // 16ms마다 타이머 실행 (약 60fps)
}

void mouse(int button, int state, int x, int y) {
	if (rect_list.size() >= 5) {
		return;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		std::cout << "x = " << x << " y = " << y << std::endl;
		int window_width = glutGet(GLUT_WINDOW_WIDTH);
		int window_height = glutGet(GLUT_WINDOW_HEIGHT);
		GLfloat glX = (x / (GLfloat)window_width) * 2 - 1;
		GLfloat glY = -((y / (GLfloat)window_height) * 2 - 1);
		std::cout << "gl_x = " << glX << " gl_y = " << glY << std::endl;

		GLfloat size = 0.1f;
		rect r;
		r.color1 = dis(gen);
		r.color2 = dis(gen);
		r.color3 = dis(gen);

		r.x1 = glX - 0.1f;
		r.y1 = glY - 0.1f;
		r.x2 = glX + 0.1f;
		r.y2 = glY + 0.1f;
		r.save_x1 = glX - 0.1f;
		r.save_y1 = glY - 0.1f;
		r.save_x2 = glX + 0.1f;
		r.save_y2 = glY + 0.1f;

		rect_list.push_back(r);
	}
}

void keyBoard(unsigned char key, int x, int y) {
	switch (key)
	{
	case '1':
		for (auto& r : rect_list) {
			if (r.move_status == 1)
				r.move_status = 0;
			else
				r.move_status = 1;
		}
		break;
	case '2':
		for (auto& r : rect_list) {
			if (r.move_status == 2)
				r.move_status = 0;
			else
				r.move_status = 2;
		}
		break;
	case '3':
		for (auto& r : rect_list) {
			// 크기를 랜덤하게 변경
			GLfloat new_size = size_dis(gen);
			GLfloat centerX = (r.x1 + r.x2) / 2;
			GLfloat centerY = (r.y1 + r.y2) / 2;
			r.x1 = centerX - new_size / 2;
			r.x2 = centerX + new_size / 2;
			r.y1 = centerY - new_size / 2;
			r.y2 = centerY + new_size / 2;
		}
		break;
	case '4':
		for (auto& r : rect_list) {
			// 색상을 랜덤하게 변경
			r.color1 = dis(gen);
			r.color2 = dis(gen);
			r.color3 = dis(gen);
		}
		break;
	case 's':
		for (auto& r : rect_list) {
			r.move_status = 0;
		}
		break;
	case 'm':
		for (auto& r : rect_list) {
			r.x1 = r.save_x1;
			r.x2 = r.save_x2;
			r.y1 = r.save_y1;
			r.y2 = r.save_y2;
		}
		break;
	case 'r':
		rect_list.clear();
		break;
	case 'q':
		exit(0);
		break;
	default:
		break;
	}
}

void main(int argc, char** argv) {
	//윈도우 생성
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	glutCreateWindow("실습4");

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
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyBoard);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(0.3f, 0.3f, 0.3f, 0.3f);
	glClear(GL_COLOR_BUFFER_BIT);
	//그리기 부분 구현

	for (const auto& r : rect_list) {
		glColor3f(r.color1, r.color2, r.color3);
		glRectf(r.x1, r.y1, r.x2, r.y2);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}
