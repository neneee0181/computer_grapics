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
	vector<glm::vec3> vertices; // ������ ����
	glm::vec3 color; // ������ ����
	int quadrant = 0;
	int move = 0; // 0 -> stop
	int x, y = 0;
	bool status = false;
	float move_y = 0.0f;
};


void insert_triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 color, vector<Shape_gl>& shapes, int quadrant,int move,int x,int y) {
	shapes.push_back(
		{
			TRIANGLE,
		{v1,v2,v3},
		color,
		quadrant,
		move,
		x,
		y
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

// ��� ������ ��� ����
vector<Shape_gl> shapes;
vector<Shape_gl> systemShapes;

GLuint vao, vbo[2];

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> rand_(0.0f, 1.0f);
uniform_real_distribution<> size_(0.1f, 0.15f);

int mode = 0; // 0 -> ��, 1 -> ��

int findQuadrant(float x, float y) { // ��и�  ã�Ƽ� �ֱ�
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

int modeMove = 0;

void keyBoard(unsigned char key, int x, int y) {
	
	switch (key)
	{
	case 'a':
		mode = 0; // ������ �׸���
		std::cout << "Mode: ��" << std::endl;
		break;
	case 'b':
		mode = 1; // ������ �׸���
		std::cout << "Mode: ��" << std::endl;
		break;
	case '0':
		modeMove = 0;
		break;
	case '1':
		modeMove = 1;
		break;
	case '2':
		modeMove = 2;
		break;
	case '3':
		modeMove = 3;
		break;
	case '4':
		modeMove = 4;
		break;
	default:
		break;
	}
	glutPostRedisplay(); // ȭ�� ����
}

void make_triangle(float glX, float glY, int quadmant) {
	// ������ ũ��� ���� ����
	float triangleSize = size_(gen); // ������ �ﰢ�� ũ��
	glm::vec3 randomColor(rand_(gen), rand_(gen), rand_(gen)); // ���� ����

	// �ﰢ���� �� ���� ������ ���� (Ŭ���� ��ġ�� ��������)
	glm::vec3 v1(glX, glY, 0.0f); // Ŭ���� ��ġ�� ù ��° ����
	glm::vec3 v2(glX + triangleSize, glY, 0.0f);
	glm::vec3 v3(glX + triangleSize / 2.0f, glY + triangleSize + 0.1, 0.0f);

	// ��и� ã��
	int quadrant = findQuadrant(glX, glY);
	int x = 0, y = 0;

	switch (quadmant)
	{
	case '1':
		x = 0; y = 0;
		break;
	case '2':
		x = 1; y = 0;
		break;
	case '3':
		x = 0; y = 1;
		break;
	case '4':
		x = 1; y = 1;
		break;
	default:
		break;
	}

	// ���ο� �ﰢ�� ����
	insert_triangle(v1, v2, v3, randomColor, shapes, quadrant, 0, x, y);

}

void mouse(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		std::cout << "x = " << x << " y = " << y << std::endl;

		float glX = (x / (float)width) * 2.0f - 1.0f; // x ��ǥ ��ȯ
		float glY = 1.0f - (y / (float)height) * 2.0f; // y ��ǥ ��ȯ

		int quadmant = findQuadrant(glX, glY);

		for (int i = 0; i < shapes.size(); ++i) {
			if (shapes[i].quadrant == quadmant) {
				shapes.erase(shapes.begin() + i);
				i--;
			}
		}

		make_triangle(glX, glY, quadmant);
		return;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		float glX = (x / (float)width) * 2.0f - 1.0f; // x ��ǥ ��ȯ
		float glY = 1.0f - (y / (float)height) * 2.0f; // y ��ǥ ��ȯ

		int quadmant = findQuadrant(glX, glY);
		int cnt = 0;
		for (int i = 0; i < shapes.size(); ++i) {
			if (shapes[i].quadrant == quadmant) {
				cnt++;
			}
		}

		if (cnt >= 3)
			return;

		make_triangle(glX, glY, quadmant);

		return;
	}

	InitBuffer();

}

void check() {

}

void timer(int value) {
	for (auto& shape : shapes) {

		switch (modeMove)
		{
		case 1:
		{
			float speed = 0.01f;

			for (auto& vertex : shape.vertices) {
				if (shape.x == 0) {
					vertex.x -= speed;
				}
				else if (shape.x == 1) {
					vertex.x += speed;
				}

				if (shape.y == 0) {
					vertex.y -= speed;
				}
				else if (shape.y == 1) {
					vertex.y += speed;
				}
			}
			// ��迡 �ε��� �� ������ ����
			for (const auto& vertex : shape.vertices) {
				if (vertex.x >= 1.0f || vertex.x <= -1.0f) {
					shape.x = shape.x == 0 ? 1 : 0;
				}
				if (vertex.y >= 1.0f || vertex.y - 0.2 <= -1.0f) {
					shape.y = shape.y == 0 ? 1 : 0;
				}
			}
			break;
		}
		case 2:
		{
			float speed = 0.01f;

			for (auto& vertex : shape.vertices) {
				if (!shape.status) {
					// status�� false�� �� x�� �̵�
					if (shape.x == 0) {
						vertex.x -= speed;
					}
					else if (shape.x == 1) {
						vertex.x += speed;
					}
				}
				else {
					// status�� true�� �� y�� �̵�
					if (shape.y == 0) {
						vertex.y -= speed;
						shape.move_y += speed;
					}
					else if (shape.y == 1) {
						vertex.y += speed;
						shape.move_y += speed;
					}
				}
			}

			// x�� ��� Ȯ��
			for (auto& vertex : shape.vertices) {
				if (vertex.x >= 1.0f || vertex.x <= -1.0f) {
					shape.x = shape.x == 0 ? 1 : 0;
					shape.status = true;  // x�࿡�� ��迡 �����ϸ� y������ ��ȯ
				}
			}

			// y������ �̵��� �Ÿ��� 0.15f �̻��̸� �ٽ� x�� �̵� ����
			if (shape.move_y >= 0.15f) {
				shape.status = false;  // x������ �̵� ��ȯ
				shape.move_y = 0.0f;   // y�� �̵� �Ÿ� �ʱ�ȭ
			}

			// y�� ��� Ȯ��
			for (auto& vertex : shape.vertices) {
				if (vertex.y >= 1.0f || vertex.y - 0.2 <= -1.0f) {
					shape.y = shape.y == 0 ? 1 : 0;
				}
			}
			break;
		}
		case 3:
			break;
		case 4:
			break;
		default:
			break;
		}
	}
	InitBuffer(); // �̵� �� ���� ����
	glutPostRedisplay(); // ȭ�� ����
	glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv) {

	width = 800;
	height = 600;

	//������ ����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("�ǽ� 8��");

	//glew �ʱ�ȭ
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	make_shaderProgram();

	insert_line(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), systemShapes); // �⺻ ���������� ��
	insert_line(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), systemShapes);

	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyBoard);
	glutMouseFunc(mouse);
	glutTimerFunc(16, timer, 0);
	glutMainLoop();

	return 0;
}
int currentIndex = 0; // ���� ���� �ε���

void drawShapes() {
	for (const Shape_gl& shape : shapes) {
		if (shape.type == TRIANGLE) {
			if (mode == 0) {
				glDrawArrays(GL_TRIANGLES, currentIndex, 3); // �ﰢ���� ������ �׸���
			}
			else if (mode == 1) {
				glDrawArrays(GL_LINE_LOOP, currentIndex, 3); // �ﰢ���� ������ �׸���
			}
			currentIndex += 3; // �ﰢ���� ���� ����ŭ �ε����� ����
		}
	}
}

void drawSystemShapes() {

	for (const Shape_gl& systemShape : systemShapes) {
		if (systemShape.type == LINE) {
			glLineWidth(2.0f); // �� �β� ����
			glDrawArrays(GL_LINES, currentIndex, 2); // ���� 2���� ����
			currentIndex += 2; // ���� ���� ����ŭ �ε����� ����
		}
	}
}


GLvoid drawScene() {

	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	glBindVertexArray(vao);

	drawShapes(); // ���� �׸���
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
		cerr << "ERROR: vertex shader ������ ���� \n" << errorLog << endl;
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
		cerr << "ERROR: frag_shader ������ ����\n" << errorLog << endl;
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

	// ��� ������ ������ �� �迭�� ���
	vector<glm::vec3> allVertices;
	vector<glm::vec3> allColors;

	// shapes ���Ϳ��� ������ ���� �߰�
	for (const Shape_gl& shape : shapes) {
		// ���� �߰�
		allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

		// ���� �߰� (������ ����ŭ ���� �߰�)
		for (size_t i = 0; i < shape.vertices.size(); ++i) {
			allColors.push_back(shape.color);
		}
	}

	// systemShapes ���Ϳ��� ������ ���� �߰� (��и� ��)
	for (const Shape_gl& systemShape : systemShapes) {
		allVertices.insert(allVertices.end(), systemShape.vertices.begin(), systemShape.vertices.end());

		for (size_t i = 0; i < systemShape.vertices.size(); ++i) {
			allColors.push_back(systemShape.color);
		}
	}

	// ���� ������ OpenGL�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(glm::vec3), allVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// ���� ������ OpenGL�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, allColors.size() * sizeof(glm::vec3), allColors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}