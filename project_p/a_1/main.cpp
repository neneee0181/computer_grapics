#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <cstdlib>
#include <ctime>

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Timer(int value);

float bgColor[3] = { 1.0f, 1.0f, 1.0f };
bool timerActive = false;

void main(int argc, char** argv) {
	srand(static_cast<unsigned>(time(0)));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("1¹ø ½Ç½À");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'c':
		bgColor[0] = 0.0f;
		bgColor[1] = 1.0f;
		bgColor[2] = 1.0f;
		glutPostRedisplay();
		break;
	case 'm':
		bgColor[0] = 1.0f;
		bgColor[1] = 0.0f;
		bgColor[2] = 1.0f;
		glutPostRedisplay();
		break;
	case 'y':
		bgColor[0] = 1.0f;
		bgColor[1] = 1.0f;
		bgColor[2] = 0.0f;
		glutPostRedisplay();
		break;
	case 'a':
		bgColor[0] = static_cast<float>(rand()) / RAND_MAX;
		bgColor[1] = static_cast<float>(rand()) / RAND_MAX;
		bgColor[2] = static_cast<float>(rand()) / RAND_MAX;
		glutPostRedisplay();
		break;
	case 'w':
		bgColor[0] = 1.0f;
		bgColor[1] = 1.0f;
		bgColor[2] = 1.0f;
		glutPostRedisplay();
		break;
	case 'k':
		bgColor[0] = 0.0f;
		bgColor[1] = 0.0f;
		bgColor[2] = 0.0f;
		glutPostRedisplay();
		break;
	case 't':
		if (!timerActive) {
			timerActive = true;
			glutTimerFunc(500, Timer, 0);
		}
		break;
	case 's':
		timerActive = false;
		break;
	case 'q':
		exit(0);
		break;
	default:
		break;
	}
}

void Timer(int value) {
	if (timerActive) {
		bgColor[0] = static_cast<float>(rand()) / RAND_MAX;
		bgColor[1] = static_cast<float>(rand()) / RAND_MAX;
		bgColor[2] = static_cast<float>(rand()) / RAND_MAX;
		glutPostRedisplay();
		glutTimerFunc(500, Timer, 0);
	}
}
