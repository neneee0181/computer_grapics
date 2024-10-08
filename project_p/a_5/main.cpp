#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <random>

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis(0.0, 1.0);  // 0.0 ~ 1.0 범위의 랜덤 값 (색상용)
uniform_real_distribution<> pos_dis(-0.9, 0.9);  // -0.9 ~ 0.9 범위의 랜덤 값 (위치용)
uniform_real_distribution<> size_dis(0.05, 0.1); // 0.05 ~ 0.1 범위의 랜덤 값 (크기용)

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

struct rect {
    GLfloat x1, x2, y1, y2;
    GLfloat color1, color2, color3;
};

vector<rect> rect_list;
rect er_rect; // 따라다니는 지우개
bool left_click = false;
GLfloat eraser_size = 0.15f;  // 지우개의 초기 크기

GLfloat window_width, window_height;

// 충돌 감지 함수
bool isColliding(rect& r1, rect& r2) {
    return !(r1.x2 < r2.x1 || r1.x1 > r2.x2 || r1.y2 < r2.y1 || r1.y1 > r2.y2);
}

void mouse(int button, int state, int x, int y) {
    // 마우스 좌클릭을 누르면 사각형이 마우스를 따라다니게 설정
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        left_click = true;

        // OpenGL 좌표계로 마우스 좌표 변환
        GLfloat glX = (x / window_width) * 2 - 1; // -1 ~ 1로 변환
        GLfloat glY = -((y / window_height) * 2 - 1); // -1 ~ 1로 변환, Y축 반전

        er_rect.color1 = 0.0f;
        er_rect.color2 = 0.0f;
        er_rect.color3 = 0.0f;

        er_rect.x1 = glX - eraser_size / 2;
        er_rect.x2 = glX + eraser_size / 2;
        er_rect.y1 = glY - eraser_size / 2;
        er_rect.y2 = glY + eraser_size / 2;
    }

    // 좌클릭을 떼면 사각형을 멈춤
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
        // 40개의 도형 생성
        for (int i = 0; i < 40; ++i) {
            rect r;
            GLfloat size = size_dis(gen);  // 랜덤한 크기
            GLfloat centerX = pos_dis(gen);  // X축 랜덤 위치
            GLfloat centerY = pos_dis(gen);  // Y축 랜덤 위치

            // 사각형의 좌표 설정 (중심 좌표에서 크기만큼의 영역 설정)
            r.x1 = centerX - size / 2;
            r.x2 = centerX + size / 2;
            r.y1 = centerY - size / 2;
            r.y2 = centerY + size / 2;

            // 랜덤한 색상 설정
            r.color1 = dis(gen);
            r.color2 = dis(gen);
            r.color3 = dis(gen);

            // 리스트에 추가
            rect_list.push_back(r);
        }
        glutPostRedisplay();
    }
}

void motion(int x, int y) {
    // 마우스를 움직이는 동안 좌클릭 상태이면 사각형이 따라다님
    if (left_click) {
        // OpenGL 좌표계로 마우스 좌표 변환
        GLfloat glX = (x / window_width) * 2 - 1; // -1 ~ 1로 변환
        GLfloat glY = -((y / window_height) * 2 - 1); // -1 ~ 1로 변환, Y축 반전

        // 마우스를 따라다니는 사각형의 좌표 업데이트
        er_rect.x1 = glX - eraser_size / 2;
        er_rect.x2 = glX + eraser_size / 2;
        er_rect.y1 = glY - eraser_size / 2;
        er_rect.y2 = glY + eraser_size / 2;

        // 사각형과 지우개의 충돌 감지
        for (auto it = rect_list.begin(); it != rect_list.end();) {
            if (isColliding(er_rect, *it)) {
                // 충돌한 사각형의 색상을 지우개의 색상으로 변경
                er_rect.color1 = it->color1;
                er_rect.color2 = it->color2;
                er_rect.color3 = it->color3;

                // 사각형을 리스트에서 제거
                it = rect_list.erase(it);

                // 지우개의 크기를 증가
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
    //윈도우 생성
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("실습5");

    //glew 초기화
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    window_width = (GLfloat)glutGet(GLUT_WINDOW_WIDTH);
    window_height = (GLfloat)glutGet(GLUT_WINDOW_HEIGHT);

    // 40개의 도형 생성
    for (int i = 0; i < 40; ++i) {
        rect r;
        GLfloat size = size_dis(gen);  // 랜덤한 크기
        GLfloat centerX = pos_dis(gen);  // X축 랜덤 위치
        GLfloat centerY = pos_dis(gen);  // Y축 랜덤 위치

        // 사각형의 좌표 설정 (중심 좌표에서 크기만큼의 영역 설정)
        r.x1 = centerX - size / 2;
        r.x2 = centerX + size / 2;
        r.y1 = centerY - size / 2;
        r.y2 = centerY + size / 2;

        // 랜덤한 색상 설정
        r.color1 = dis(gen);
        r.color2 = dis(gen);
        r.color3 = dis(gen);

        // 리스트에 추가
        rect_list.push_back(r);
    }

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);  // 마우스 움직임을 처리하는 함수 등록
    glutKeyboardFunc(keyBoard);
    glutMainLoop();
}

GLvoid drawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // 배경색 흰색
    glClear(GL_COLOR_BUFFER_BIT);

    // 생성된 사각형 그리기
    for (const auto& r : rect_list) {
        glColor3f(r.color1, r.color2, r.color3);  // 색상 설정
        glRectf(r.x1, r.y1, r.x2, r.y2);  // 사각형 그리기
    }

    // 마우스를 따라다니는 지우개 그리기
    glColor3f(er_rect.color1, er_rect.color2, er_rect.color3);
    glRectf(er_rect.x1, er_rect.y1, er_rect.x2, er_rect.y2);

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    window_width = (GLfloat)w;
    window_height = (GLfloat)h;
}
