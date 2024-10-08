#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <random>

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis(0.0, 1.0);  // 0.0 ~ 1.0 ������ ���� �� (�����)
uniform_real_distribution<> pos_dis(-0.9, 0.9);  // -0.9 ~ 0.9 ������ ���� �� (��ġ��)
uniform_real_distribution<> size_dis(0.05, 0.1); // 0.05 ~ 0.1 ������ ���� �� (ũ���)

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

struct rect {
    GLfloat x1, x2, y1, y2;
    GLfloat color1, color2, color3;
};

vector<rect> rect_list;
rect er_rect; // ����ٴϴ� ���찳
bool left_click = false;
GLfloat eraser_size = 0.15f;  // ���찳�� �ʱ� ũ��

GLfloat window_width, window_height;

// �浹 ���� �Լ�
bool isColliding(rect& r1, rect& r2) {
    return !(r1.x2 < r2.x1 || r1.x1 > r2.x2 || r1.y2 < r2.y1 || r1.y1 > r2.y2);
}

void mouse(int button, int state, int x, int y) {
    // ���콺 ��Ŭ���� ������ �簢���� ���콺�� ����ٴϰ� ����
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        left_click = true;

        // OpenGL ��ǥ��� ���콺 ��ǥ ��ȯ
        GLfloat glX = (x / window_width) * 2 - 1; // -1 ~ 1�� ��ȯ
        GLfloat glY = -((y / window_height) * 2 - 1); // -1 ~ 1�� ��ȯ, Y�� ����

        er_rect.color1 = 0.0f;
        er_rect.color2 = 0.0f;
        er_rect.color3 = 0.0f;

        er_rect.x1 = glX - eraser_size / 2;
        er_rect.x2 = glX + eraser_size / 2;
        er_rect.y1 = glY - eraser_size / 2;
        er_rect.y2 = glY + eraser_size / 2;
    }

    // ��Ŭ���� ���� �簢���� ����
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        left_click = false;
        er_rect.x1 = 0.0f;
        er_rect.x2 = 0.0f;
        er_rect.y1 = 0.0f;
        er_rect.y2 = 0.0f;
        eraser_size = 0.15f;
    }

    glutPostRedisplay();
}

void keyBoard(unsigned char key, int x, int y) {
    if (key == 'r') {
        rect_list.clear();
        // 40���� ���� ����
        for (int i = 0; i < 40; ++i) {
            rect r;
            GLfloat size = size_dis(gen);  // ������ ũ��
            GLfloat centerX = pos_dis(gen);  // X�� ���� ��ġ
            GLfloat centerY = pos_dis(gen);  // Y�� ���� ��ġ

            // �簢���� ��ǥ ���� (�߽� ��ǥ���� ũ�⸸ŭ�� ���� ����)
            r.x1 = centerX - size / 2;
            r.x2 = centerX + size / 2;
            r.y1 = centerY - size / 2;
            r.y2 = centerY + size / 2;

            // ������ ���� ����
            r.color1 = dis(gen);
            r.color2 = dis(gen);
            r.color3 = dis(gen);

            // ����Ʈ�� �߰�
            rect_list.push_back(r);
        }
        glutPostRedisplay();
    }
}

void motion(int x, int y) {
    // ���콺�� �����̴� ���� ��Ŭ�� �����̸� �簢���� ����ٴ�
    if (left_click) {
        // OpenGL ��ǥ��� ���콺 ��ǥ ��ȯ
        GLfloat glX = (x / window_width) * 2 - 1; // -1 ~ 1�� ��ȯ
        GLfloat glY = -((y / window_height) * 2 - 1); // -1 ~ 1�� ��ȯ, Y�� ����

        // ���콺�� ����ٴϴ� �簢���� ��ǥ ������Ʈ
        er_rect.x1 = glX - eraser_size / 2;
        er_rect.x2 = glX + eraser_size / 2;
        er_rect.y1 = glY - eraser_size / 2;
        er_rect.y2 = glY + eraser_size / 2;

        // �簢���� ���찳�� �浹 ����
        for (auto it = rect_list.begin(); it != rect_list.end();) {
            if (isColliding(er_rect, *it)) {
                // �浹�� �簢���� ������ ���찳�� �������� ����
                er_rect.color1 = it->color1;
                er_rect.color2 = it->color2;
                er_rect.color3 = it->color3;

                // �簢���� ����Ʈ���� ����
                it = rect_list.erase(it);

                // ���찳�� ũ�⸦ ����
                eraser_size += 0.02f;
            }
            else {
                ++it;
            }
        }

        glutPostRedisplay();
    }
}

void main(int argc, char** argv) {
    //������ ����
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("�ǽ�5");

    //glew �ʱ�ȭ
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    window_width = (GLfloat)glutGet(GLUT_WINDOW_WIDTH);
    window_height = (GLfloat)glutGet(GLUT_WINDOW_HEIGHT);

    // 40���� ���� ����
    for (int i = 0; i < 40; ++i) {
        rect r;
        GLfloat size = size_dis(gen);  // ������ ũ��
        GLfloat centerX = pos_dis(gen);  // X�� ���� ��ġ
        GLfloat centerY = pos_dis(gen);  // Y�� ���� ��ġ

        // �簢���� ��ǥ ���� (�߽� ��ǥ���� ũ�⸸ŭ�� ���� ����)
        r.x1 = centerX - size / 2;
        r.x2 = centerX + size / 2;
        r.y1 = centerY - size / 2;
        r.y2 = centerY + size / 2;

        // ������ ���� ����
        r.color1 = dis(gen);
        r.color2 = dis(gen);
        r.color3 = dis(gen);

        // ����Ʈ�� �߰�
        rect_list.push_back(r);
    }

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);  // ���콺 �������� ó���ϴ� �Լ� ���
    glutKeyboardFunc(keyBoard);
    glutMainLoop();
}

GLvoid drawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // ���� ���
    glClear(GL_COLOR_BUFFER_BIT);

    // ������ �簢�� �׸���
    for (const auto& r : rect_list) {
        glColor3f(r.color1, r.color2, r.color3);  // ���� ����
        glRectf(r.x1, r.y1, r.x2, r.y2);  // �簢�� �׸���
    }

    // ���콺�� ����ٴϴ� ���찳 �׸���
    glColor3f(er_rect.color1, er_rect.color2, er_rect.color3);
    glRectf(er_rect.x1, er_rect.y1, er_rect.x2, er_rect.y2);

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    window_width = (GLfloat)w;
    window_height = (GLfloat)h;
}
