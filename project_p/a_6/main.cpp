#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <random>

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> urd(0.0f, 1.0f);
uniform_real_distribution<> size_urd(0.1, 0.2);
uniform_real_distribution<> c_urd(5, 10);
uniform_real_distribution<> pos_dis(-0.9, 0.9);  // -0.9 ~ 0.9 ������ ���� �� (��ġ��)

uniform_int_distribution<int> rand_move(1, 4);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

struct rect {
	GLfloat x1, x2, y1, y2;
	GLfloat color1, color2, color3;
	bool x_l = false, x_r = false, y_u = false, y_d = false;
};

vector<rect> rect_list;

bool isColliding(rect& r1, GLfloat x, GLfloat y) {
	return (r1.x1 < x && r1.x2 > x && r1.y1 < y && r1.y2 > y);
}

void make_r1(rect r) {

	// �߽� ��ǥ ���
	GLfloat centerX = (r.x1 + r.x2) / 2;
	GLfloat centerY = (r.y1 + r.y2) / 2;

	// 4���� ���ο� �簢���� ����
	rect new_r1, new_r2, new_r3, new_r4;

	// �� �簢���� ������ ���� �簢���� ������ ����
	new_r1.color1 = r.color1; new_r1.color2 = r.color2; new_r1.color3 = r.color3;
	new_r2.color1 = r.color1; new_r2.color2 = r.color2; new_r2.color3 = r.color3;
	new_r3.color1 = r.color1; new_r3.color2 = r.color2; new_r3.color3 = r.color3;
	new_r4.color1 = r.color1; new_r4.color2 = r.color2; new_r4.color3 = r.color3;

	// ��(����) �簢��
	new_r1.x1 = r.x1;
	new_r1.x2 = centerX;
	new_r1.y1 = centerY;
	new_r1.y2 = r.y2;
	new_r1.x_l = true;

	//// ��(�Ʒ���) �簢��
	new_r2.x1 = r.x1;
	new_r2.x2 = centerX;
	new_r2.y1 = r.y1;
	new_r2.y2 = centerY;
	new_r2.y_d = true;

	//// ��(����) �簢��
	new_r3.x1 = centerX;
	new_r3.x2 = r.x2;
	new_r3.y1 = centerY;
	new_r3.y2 = r.y2;
	new_r3.y_u = true;

	//// ��(������) �簢��
	new_r4.x1 = centerX;
	new_r4.x2 = r.x2;
	new_r4.y1 = centerY;
	new_r4.y2 = centerY + (centerY - r.y2);
	new_r4.x_r = true;

	// ���ο� �簢���� ����Ʈ�� �߰�
	rect_list.push_back(new_r1);
	rect_list.push_back(new_r2);
	rect_list.push_back(new_r3);
	rect_list.push_back(new_r4);
}


void make_r2(rect r) {

	// �߽� ��ǥ ���
	GLfloat centerX = (r.x1 + r.x2) / 2;
	GLfloat centerY = (r.y1 + r.y2) / 2;

	// 4���� ���ο� �簢���� ����
	rect new_r1, new_r2, new_r3, new_r4;

	// �� �簢���� ������ ���� �簢���� ������ ����
	new_r1.color1 = r.color1; new_r1.color2 = r.color2; new_r1.color3 = r.color3;
	new_r2.color1 = r.color1; new_r2.color2 = r.color2; new_r2.color3 = r.color3;
	new_r3.color1 = r.color1; new_r3.color2 = r.color2; new_r3.color3 = r.color3;
	new_r4.color1 = r.color1; new_r4.color2 = r.color2; new_r4.color3 = r.color3;

	// ��(����) �簢��
	new_r1.x1 = r.x1;
	new_r1.x2 = centerX;
	new_r1.y1 = centerY;
	new_r1.y2 = r.y2;
	new_r1.x_l = true;
	new_r1.y_u = true;

	//// ��(�Ʒ���) �簢��
	new_r2.x1 = r.x1;
	new_r2.x2 = centerX;
	new_r2.y1 = r.y1;
	new_r2.y2 = centerY;
	new_r2.y_d = true;
	new_r2.x_l = true;

	//// ��(����) �簢��
	new_r3.x1 = centerX;
	new_r3.x2 = r.x2;
	new_r3.y1 = centerY;
	new_r3.y2 = r.y2;
	new_r3.y_u = true;
	new_r3.x_r = true;

	//// ��(������) �簢��
	new_r4.x1 = centerX;
	new_r4.x2 = r.x2;
	new_r4.y1 = centerY;
	new_r4.y2 = centerY + (centerY - r.y2);
	new_r4.x_r = true;
	new_r4.y_d = true;

	// ���ο� �簢���� ����Ʈ�� �߰�
	rect_list.push_back(new_r1);
	rect_list.push_back(new_r2);
	rect_list.push_back(new_r3);
	rect_list.push_back(new_r4);
}



void make_r3(rect r, int rand) {

	// �߽� ��ǥ ���
	GLfloat centerX = (r.x1 + r.x2) / 2;
	GLfloat centerY = (r.y1 + r.y2) / 2;

	// 4���� ���ο� �簢���� ����
	rect new_r1, new_r2, new_r3, new_r4;

	// �� �簢���� ������ ���� �簢���� ������ ����
	new_r1.color1 = r.color1; new_r1.color2 = r.color2; new_r1.color3 = r.color3;
	new_r2.color1 = r.color1; new_r2.color2 = r.color2; new_r2.color3 = r.color3;
	new_r3.color1 = r.color1; new_r3.color2 = r.color2; new_r3.color3 = r.color3;
	new_r4.color1 = r.color1; new_r4.color2 = r.color2; new_r4.color3 = r.color3;

	// ��(����) �簢��
	new_r1.x1 = r.x1;
	new_r1.x2 = centerX;
	new_r1.y1 = centerY;
	new_r1.y2 = r.y2;

	//// ��(�Ʒ���) �簢��
	new_r2.x1 = r.x1;
	new_r2.x2 = centerX;
	new_r2.y1 = r.y1;
	new_r2.y2 = centerY;

	//// ��(����) �簢��
	new_r3.x1 = centerX;
	new_r3.x2 = r.x2;
	new_r3.y1 = centerY;
	new_r3.y2 = r.y2;

	//// ��(������) �簢��
	new_r4.x1 = centerX;
	new_r4.x2 = r.x2;
	new_r4.y1 = centerY;
	new_r4.y2 = centerY + (centerY - r.y2);

	switch (rand)
	{
	case 1:
		new_r1.x_l = true;
		new_r2.x_l = true;
		new_r3.x_l = true;
		new_r4.x_l = true;
		break;
	case 2:
		new_r1.x_r = true;
		new_r2.x_r = true;
		new_r3.x_r = true;
		new_r4.x_r = true;
		break;
	case 3:
		new_r1.y_u = true;
		new_r2.y_u = true;
		new_r3.y_u = true;
		new_r4.y_u = true;
		break;
	case 4:
		new_r1.y_d = true;
		new_r2.y_d = true;
		new_r3.y_d = true;
		new_r4.y_d = true;
		break;
	default:
		break;
	}

	// ���ο� �簢���� ����Ʈ�� �߰�
	rect_list.push_back(new_r1);
	rect_list.push_back(new_r2);
	rect_list.push_back(new_r3);
	rect_list.push_back(new_r4);
}

void make_r4(rect r) {
	// �߽� ��ǥ ���
	GLfloat centerX = (r.x1 + r.x2) / 2;
	GLfloat centerY = (r.y1 + r.y2) / 2;
	GLfloat quarterX = (r.x2 - r.x1) / 4; // ���� ������ 1/4
	GLfloat quarterY = (r.y2 - r.y1) / 4; // ���� ������ 1/4

	// 8���� ���ο� �簢���� ����
	rect new_r1, new_r2, new_r3, new_r4, new_r5, new_r6, new_r7, new_r8;

	// �� �簢���� ������ ���� �簢���� ������ ����
	new_r1.color1 = r.color1; new_r1.color2 = r.color2; new_r1.color3 = r.color3;
	new_r2.color1 = r.color1; new_r2.color2 = r.color2; new_r2.color3 = r.color3;
	new_r3.color1 = r.color1; new_r3.color2 = r.color2; new_r3.color3 = r.color3;
	new_r4.color1 = r.color1; new_r4.color2 = r.color2; new_r4.color3 = r.color3;
	new_r5.color1 = r.color1; new_r5.color2 = r.color2; new_r5.color3 = r.color3;
	new_r6.color1 = r.color1; new_r6.color2 = r.color2; new_r6.color3 = r.color3;
	new_r7.color1 = r.color1; new_r7.color2 = r.color2; new_r7.color3 = r.color3;
	new_r8.color1 = r.color1; new_r8.color2 = r.color2; new_r8.color3 = r.color3;

	// �簢���� 8����Ͽ� ����
	// �»��
	new_r1.x1 = r.x1; new_r1.x2 = r.x1 + quarterX;
	new_r1.y1 = r.y1 + 3 * quarterY; new_r1.y2 = r.y2;
	new_r1.x_l = true;
	new_r1.y_u = true;

	// ���ߴ�
	new_r2.x1 = r.x1; new_r2.x2 = r.x1 + quarterX;
	new_r2.y1 = r.y1 + 2 * quarterY; new_r2.y2 = r.y1 + 3 * quarterY;
	new_r2.x_l = true;

	// ���ϴ�
	new_r3.x1 = r.x1; new_r3.x2 = r.x1 + quarterX;
	new_r3.y1 = r.y1; new_r3.y2 = r.y1 + quarterY;
	new_r3.x_l = true;
	new_r3.y_d = true;

	// �߾� ���
	new_r4.x1 = r.x1 + quarterX; new_r4.x2 = r.x1 + 2 * quarterX;
	new_r4.y1 = r.y1 + 2 * quarterY; new_r4.y2 = r.y2;
	new_r4.y_u = true;

	// �߾� �ϴ�
	new_r5.x1 = r.x1 + quarterX; new_r5.x2 = r.x1 + 2 * quarterX;
	new_r5.y1 = r.y1; new_r5.y2 = r.y1 + 2 * quarterY;
	new_r5.y_d = true;

	// ����
	new_r6.x1 = r.x1 + 2 * quarterX; new_r6.x2 = r.x2;
	new_r6.y1 = r.y1 + 3 * quarterY; new_r6.y2 = r.y2;
	new_r6.x_r = true;
	new_r6.y_u = true;

	// ���ߴ�
	new_r7.x1 = r.x1 + 2 * quarterX; new_r7.x2 = r.x2;
	new_r7.y1 = r.y1 + 2 * quarterY; new_r7.y2 = r.y1 + 3 * quarterY;
	new_r7.x_r = true;

	// ���ϴ�
	new_r8.x1 = r.x1 + 2 * quarterX; new_r8.x2 = r.x2;
	new_r8.y1 = r.y1; new_r8.y2 = r.y1 + 2 * quarterY;
	new_r8.x_r = true;
	new_r8.y_d = true;

	// ���ο� �簢���� ����Ʈ�� �߰�
	rect_list.push_back(new_r1);
	rect_list.push_back(new_r2);
	rect_list.push_back(new_r3);
	rect_list.push_back(new_r4);
	rect_list.push_back(new_r5);
	rect_list.push_back(new_r6);
	rect_list.push_back(new_r7);
	rect_list.push_back(new_r8);
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		GLfloat window_width = (GLfloat)glutGet(GLUT_WINDOW_WIDTH);
		GLfloat window_height = (GLfloat)glutGet(GLUT_WINDOW_HEIGHT);

		GLfloat glX = (x / window_width) * 2 - 1;
		GLfloat glY = -((y / window_height) * 2 - 1);

		cout << "x = " << glX << ", y = " << glY << endl;

		for (int i = 0; i < rect_list.size(); ++i) {
			if (isColliding(rect_list[i], glX, glY)) {

				switch (rand_move(gen))
				{
				case 1:
					make_r1(rect_list[i]);
					break;
				case 2:
					make_r2(rect_list[i]);
					break;
				case 3:
					make_r3(rect_list[i], rand_move(gen));
					break;
				case 4:
					make_r4(rect_list[i]);
					break;
				default:
					break;
				}

				rect_list.erase(rect_list.begin() + i);

				break;
			}
		}


		glutPostRedisplay(); // ȭ�� ����

	}
}

void timer(int value) {

	for (auto& r : rect_list) {
		if (r.x_l) {
			r.x1 = r.x1 - 0.01;
			r.x2 = r.x2 - 0.01;
		}

		if (r.x_r) {
			r.x1 = r.x1 + 0.01;
			r.x2 = r.x2 + 0.01;
		}

		if (r.y_d) {
			r.y1 = r.y1 - 0.01;
			r.y2 = r.y2 - 0.01;
		}

		if (r.y_u) {
			r.y1 = r.y1 + 0.01;
			r.y2 = r.y2 + 0.01;
		}
	}

	glutTimerFunc(16, timer, 0);
	glutPostRedisplay(); // ȭ�� ����
}

void main(int argc, char** argv) {
	//������ ����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	glutCreateWindow("�ǽ�6");

	//glew �ʱ�ȭ
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	int num = c_urd(gen);

	for (int i = 0; i < num; ++i) {
		rect r;
		r.color1 = urd(gen);
		r.color2 = urd(gen);
		r.color3 = urd(gen);

		GLfloat size = size_urd(gen);
		GLfloat centerX = pos_dis(gen);
		GLfloat centerY = pos_dis(gen);

		r.x1 = centerX - size / 2;
		r.x2 = centerX + size / 2;
		r.y1 = centerY - size / 2;
		r.y2 = centerY + size / 2;
		rect_list.push_back(r);

	}

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(mouse);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (const auto& r : rect_list) {
		glColor3f(r.color1, r.color2, r.color3);
		glRectf(r.x1, r.y1, r.x2, r.y2);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}