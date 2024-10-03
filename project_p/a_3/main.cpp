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
        // 랜덤 위치와 크기 설정
        x1 = static_cast<float>(rand()) / RAND_MAX * 2 - 1;
        y1 = static_cast<float>(rand()) / RAND_MAX * 2 - 1;
        x2 = x1 + 0.2f; // 도형의 가로 길이
        y2 = y1 + 0.2f; // 도형의 세로 길이
        // 랜덤 색상 설정
        color1 = static_cast<float>(rand()) / RAND_MAX;
        color2 = static_cast<float>(rand()) / RAND_MAX;
        color3 = static_cast<float>(rand()) / RAND_MAX;
    }

    // 마우스 좌표가 사각형 안에 있는지 확인하는 함수
    bool isInside(float x, float y) {
        return x >= x1 && x <= x2 && y >= y1 && y <= y2;
    }

    // 사각형의 위치를 업데이트하는 함수
    void updatePosition(float dx, float dy) {
        x1 += dx;
        y1 += dy;
        x2 += dx;
        y2 += dy;
    }

    // 사각형이 겹치는지 확인하는 함수
    bool isOverlapping(const rect& other) const {
        return !(x2 < other.x1 || other.x2 < x1 || y2 < other.y1 || other.y2 < y1);
    }

    // 두 사각형을 합치는 함수
    void mergeWith(const rect& other) {
        // x축, y축의 최소, 최대값을 계산
        x1 = min(x1, other.x1);
        y1 = min(y1, other.y1);
        x2 = max(x2, other.x2);
        y2 = max(y2, other.y2);

        // 랜덤 색상 설정
        color1 = static_cast<float>(rand()) / RAND_MAX;
        color2 = static_cast<float>(rand()) / RAND_MAX;
        color3 = static_cast<float>(rand()) / RAND_MAX;
    }
};

vector<rect> rect_list;

bool isDragging = false;
int selectedRect = -1;  // 선택된 사각형 인덱스
float prevMouseX, prevMouseY;  // 이전 마우스 좌표

// OpenGL 좌표로 변환
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
            // 클릭된 좌표에 도형이 있는지 확인
            for (int i = 0; i < rect_list.size(); ++i) {
                if (rect_list[i].isInside(oglX, oglY)) {
                    selectedRect = i;  // 도형 선택
                    isDragging = true;  // 드래그 시작
                    prevMouseX = oglX;
                    prevMouseY = oglY;
                    break;
                }
            }
        }
        else if (state == GLUT_UP) {
            isDragging = false;  // 드래그 종료

            // 선택된 도형 기준으로 다른 도형들과 겹치는지 확인
            if (selectedRect != -1) {
                for (int i = 0; i < rect_list.size(); ++i) {
                    if (i != selectedRect && rect_list[selectedRect].isOverlapping(rect_list[i])) {
                        // 겹쳐진다면 선택된 사각형과 다른 사각형을 병합하고, 다른 사각형을 삭제
                        rect_list[selectedRect].mergeWith(rect_list[i]);
                        rect_list.erase(rect_list.begin() + i);
                        if (i < selectedRect) {
                            // 선택된 사각형의 인덱스가 앞으로 당겨졌으므로 조정
                            selectedRect--;
                        }
                        i--;  // 리스트 크기가 줄었으므로 인덱스 조정
                    }
                }
            }

            selectedRect = -1;  // 선택 해제
        }
    }
}


void Motion(int x, int y) {
    if (isDragging && selectedRect != -1) {
        // 마우스가 움직일 때 도형의 좌표를 업데이트
        float oglX = screenToOpenGLX(x, glutGet(GLUT_WINDOW_WIDTH));
        float oglY = screenToOpenGLY(y, glutGet(GLUT_WINDOW_HEIGHT));

        float dx = oglX - prevMouseX;
        float dy = oglY - prevMouseY;

        // 선택된 도형의 좌표 업데이트
        rect_list[selectedRect].updatePosition(dx, dy);

        prevMouseX = oglX;
        prevMouseY = oglY;

        // 화면을 다시 그리도록 요청
        glutPostRedisplay();
    }
}

void initializeRectangles() {
    // 도형 10개 추가
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
    // 윈도우 생성
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("실습3");

    // glew 초기화
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    // 랜덤 시드 초기화
    srand(static_cast<unsigned int>(time(0)));

    // 창이 생성될 때 도형 10개를 한 번 초기화
    initializeRectangles();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(keyBoard);
    glutMotionFunc(Motion);  // 마우스 이동 콜백 등록
    glutMainLoop();
}

GLvoid drawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // rect_list에 있는 도형 그리기
    for (const auto& r : rect_list) {
        glColor3f(r.color1, r.color2, r.color3);  // 색상 설정
        glBegin(GL_QUADS);  // 사각형 그리기 시작
        glVertex2f(r.x1, r.y1);
        glVertex2f(r.x2, r.y1);
        glVertex2f(r.x2, r.y2);
        glVertex2f(r.x1, r.y2);
        glEnd();  // 사각형 그리기 끝
    }

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}
