#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

struct rect {
    GLfloat x1, y1, x2, y2;
    GLfloat color1, color2, color3;

    rect() {
        // ���� ��ġ�� ũ�� ����
        x1 = static_cast<float>(rand()) / RAND_MAX * 2 - 1;
        y1 = static_cast<float>(rand()) / RAND_MAX * 2 - 1;
        x2 = x1 + 0.2f; // ������ ���� ����
        y2 = y1 + 0.2f; // ������ ���� ����
        // ���� ���� ����
        color1 = static_cast<float>(rand()) / RAND_MAX;
        color2 = static_cast<float>(rand()) / RAND_MAX;
        color3 = static_cast<float>(rand()) / RAND_MAX;
    }

    // ���콺 ��ǥ�� �簢�� �ȿ� �ִ��� Ȯ���ϴ� �Լ�
    bool isInside(float x, float y) {
        return x >= x1 && x <= x2 && y >= y1 && y <= y2;
    }

    // �簢���� ��ġ�� ������Ʈ�ϴ� �Լ�
    void updatePosition(float dx, float dy) {
        x1 += dx;
        y1 += dy;
        x2 += dx;
        y2 += dy;
    }

    // �簢���� ��ġ���� Ȯ���ϴ� �Լ�
    bool isOverlapping(const rect& other) const {
        return !(x2 < other.x1 || other.x2 < x1 || y2 < other.y1 || other.y2 < y1);
    }

    // �� �簢���� ��ġ�� �Լ�
    void mergeWith(const rect& other) {
        // x��, y���� �ּ�, �ִ밪�� ���
        x1 = min(x1, other.x1);
        y1 = min(y1, other.y1);
        x2 = max(x2, other.x2);
        y2 = max(y2, other.y2);

        // ���� ���� ����
        color1 = static_cast<float>(rand()) / RAND_MAX;
        color2 = static_cast<float>(rand()) / RAND_MAX;
        color3 = static_cast<float>(rand()) / RAND_MAX;
    }
};

vector<rect> rect_list;

bool isDragging = false;
int selectedRect = -1;  // ���õ� �簢�� �ε���
float prevMouseX, prevMouseY;  // ���� ���콺 ��ǥ

// OpenGL ��ǥ�� ��ȯ
float screenToOpenGLX(int x, int windowWidth) {
    return (2.0f * x) / windowWidth - 1.0f;
}

float screenToOpenGLY(int y, int windowHeight) {
    return 1.0f - (2.0f * y) / windowHeight;
}

void Mouse(int button, int state, int x, int y) {
    float oglX = screenToOpenGLX(x, glutGet(GLUT_WINDOW_WIDTH));
    float oglY = screenToOpenGLY(y, glutGet(GLUT_WINDOW_HEIGHT));

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            // Ŭ���� ��ǥ�� ������ �ִ��� Ȯ��
            for (int i = 0; i < rect_list.size(); ++i) {
                if (rect_list[i].isInside(oglX, oglY)) {
                    selectedRect = i;  // ���� ����
                    isDragging = true;  // �巡�� ����
                    prevMouseX = oglX;
                    prevMouseY = oglY;
                    break;
                }
            }
        }
        else if (state == GLUT_UP) {
            isDragging = false;  // �巡�� ����

            // ���õ� ���� �������� �ٸ� ������� ��ġ���� Ȯ��
            if (selectedRect != -1) {
                for (int i = 0; i < rect_list.size(); ++i) {
                    if (i != selectedRect && rect_list[selectedRect].isOverlapping(rect_list[i])) {
                        // �������ٸ� ���õ� �簢���� �ٸ� �簢���� �����ϰ�, �ٸ� �簢���� ����
                        rect_list[selectedRect].mergeWith(rect_list[i]);
                        rect_list.erase(rect_list.begin() + i);
                        if (i < selectedRect) {
                            // ���õ� �簢���� �ε����� ������ ��������Ƿ� ����
                            selectedRect--;
                        }
                        i--;  // ����Ʈ ũ�Ⱑ �پ����Ƿ� �ε��� ����
                    }
                }
            }

            selectedRect = -1;  // ���� ����
        }
    }
}


void Motion(int x, int y) {
    if (isDragging && selectedRect != -1) {
        // ���콺�� ������ �� ������ ��ǥ�� ������Ʈ
        float oglX = screenToOpenGLX(x, glutGet(GLUT_WINDOW_WIDTH));
        float oglY = screenToOpenGLY(y, glutGet(GLUT_WINDOW_HEIGHT));

        float dx = oglX - prevMouseX;
        float dy = oglY - prevMouseY;

        // ���õ� ������ ��ǥ ������Ʈ
        rect_list[selectedRect].updatePosition(dx, dy);

        prevMouseX = oglX;
        prevMouseY = oglY;

        // ȭ���� �ٽ� �׸����� ��û
        glutPostRedisplay();
    }
}

void initializeRectangles() {
    // ���� 10�� �߰�
    if (rect_list.size() >= 10) {
        return;
    }
    rect_list.push_back(rect());
    glutPostRedisplay();
}

void keyBoard(unsigned char key, int x, int y) {
    switch (key)
    {
    case 'a':
        initializeRectangles();
        break;
    default:
        break;
    }
}

void main(int argc, char** argv) {
    // ������ ����
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("�ǽ�3");

    // glew �ʱ�ȭ
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    // ���� �õ� �ʱ�ȭ
    srand(static_cast<unsigned int>(time(0)));

    // â�� ������ �� ���� 10���� �� �� �ʱ�ȭ
    initializeRectangles();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(keyBoard);
    glutMotionFunc(Motion);  // ���콺 �̵� �ݹ� ���
    glutMainLoop();
}

GLvoid drawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // rect_list�� �ִ� ���� �׸���
    for (const auto& r : rect_list) {
        glColor3f(r.color1, r.color2, r.color3);  // ���� ����
        glBegin(GL_QUADS);  // �簢�� �׸��� ����
        glVertex2f(r.x1, r.y1);
        glVertex2f(r.x2, r.y1);
        glVertex2f(r.x2, r.y2);
        glVertex2f(r.x1, r.y2);
        glEnd();  // �簢�� �׸��� ��
    }

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}
