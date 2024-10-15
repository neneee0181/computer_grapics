#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <random>
#include <vector>
#include "filetobuf.h"

#define M_PI 3.14159265358979323846

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

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(-1.0f, 1.0f);

enum ShapeType {
    TRIANGLE,
    SQUARE,
    POINT_,
    LINE,
    PENTA,  // 오각형 추가
    HEXA
};

struct Shape_gl {
    ShapeType type;
    vector<glm::vec3> vertices; // 도형의 정점
    glm::vec3 color; // 도형의 색상
    int vertex = 0;
    // == 연산자 오버로딩
    bool operator==(const Shape_gl& other) const {
        return (type == other.type && vertices == other.vertices && color == other.color);
    }
};

void insert_triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 color, vector<Shape_gl>& shapes) {
    shapes.push_back(
        {
            TRIANGLE,
        {v1,v2,v3},
        glm::vec3(dis(gen), dis(gen), dis(gen)),
        3
        }
    );
}

void insert_square(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 v5, glm::vec3 v6, glm::vec3 color, vector<Shape_gl>& shapes) {
    shapes.push_back(
        {
            SQUARE,
        {v1,v2,v3,v4,v5,v6},
        glm::vec3(dis(gen), dis(gen), dis(gen)),
        4
        }
    );
}

void insert_point2(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 v5, glm::vec3 v6, glm::vec3 color, vector<Shape_gl>& shapes) {
    shapes.push_back(
        {
            POINT_,
        {v1,v2,v3,v4,v5,v6},
        glm::vec3(dis(gen), dis(gen), dis(gen)),
        1
        }
    );
}

void insert_point(glm::vec3 v1, glm::vec3 color, vector<Shape_gl>& shapes) {
    shapes.push_back(
        {
            POINT_,
        {v1},
        glm::vec3(dis(gen), dis(gen), dis(gen))
        }
    );
}

void insert_line(glm::vec3 v1, glm::vec3 v2, glm::vec3 color, vector<Shape_gl>& shapes) {
    shapes.push_back(
        {
            LINE,
        {v1,v2},
        color,
        2
        }
    );
}

// 모든 도형을 담는 벡터
vector<Shape_gl> shapes;

GLuint vao, vbo[2];

float stepSize = 0.01f;    // 한 번의 업데이트에서 이동할 거리

bool isDragging = false;
Shape_gl* selectedShape = nullptr;
glm::vec2 lastMousePosition;

void keyBoard(unsigned char key, int x, int y) {
    switch (key)
    {
    case 'c':
        shapes.clear();
        break;
    default:
        break;
    }
    InitBuffer();
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    float normalizedX = (float(x) / width) * 2.0f - 1.0f;
    float normalizedY = 1.0f - (float(y) / height) * 2.0f;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // 마우스 클릭 시 도형 선택 여부 확인
        for (Shape_gl& shape : shapes) {
            for (glm::vec3& vertex : shape.vertices) {
                if (abs(vertex.x - normalizedX) < 0.05f && abs(vertex.y - normalizedY) < 0.05f) {
                    selectedShape = &shape;
                    isDragging = true;
                    lastMousePosition = glm::vec2(normalizedX, normalizedY);
                    break;
                }
            }
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {

        if (selectedShape != nullptr) {
            std::vector<Shape_gl> deleteShapes;  // 삭제할 도형을 저장하는 배열

            // 선택된 도형과 현재 마우스 좌표에 있는 다른 도형을 비교.
            for (Shape_gl& shape : shapes) {

                for (glm::vec3& vertex : shape.vertices) {
                    if (abs(vertex.x - normalizedX) < 0.05f && abs(vertex.y - normalizedY) < 0.05f) {

                        if (&shape == selectedShape) {
                            continue;
                        }
                        if (selectedShape == nullptr) {
                            continue;
                        }
                        cout << "도형 발견: " << shape.type << endl;

                        int num = selectedShape->vertex + shape.vertex;

                        if (num >= 7) {
                            num = num % 6;
                        }
                        switch (num)
                        {
                        case 1:
                        {
                            // 점을 생성
                            std::uniform_real_distribution<> size_dis(0.01f, 0.05f);  // 점의 크기를 0.01 ~ 0.05 사이로 랜덤하게 설정
                            float size = size_dis(gen);

                            glm::vec3 center = (vertex + selectedShape->vertices[0]) / 2.0f; // 두 도형의 중간 좌표에 점 생성
                            glm::vec3 v1(center.x - size / 2.0f, center.y - size / 2.0f, 0.0f);
                            glm::vec3 v2(v1.x + size, v1.y, 0.0f);
                            glm::vec3 v3(v1.x, v1.y + size, 0.0f);
                            glm::vec3 v4(v1.x + size, v1.y + size, 0.0f);
                            glm::vec3 color(dis(gen), dis(gen), dis(gen));  // 랜덤한 색상

                            // 점처럼 보이게 작은 사각형으로 처리
                            insert_point2(v1, v2, v3, v2, v3, v4, color, shapes);
                            // 삭제할 도형 배열에 넣기
                            deleteShapes.push_back(shape);
                            deleteShapes.push_back(*selectedShape);

                            break;
                        }
                        case 2:
                        {
                            // 점과 점이 만나면 랜덤한 선을 생성
                            glm::vec3 point1 = selectedShape->vertices[0];
                            glm::vec3 point2 = shape.vertices[0];

                            // 랜덤 크기 설정
                            std::uniform_real_distribution<> size_dis(0.1f, 0.3f); // 선의 길이 범위 0.1 ~ 0.3
                            float size = size_dis(gen);

                            // 선의 방향을 설정 (두 점을 잇는 벡터의 방향)
                            glm::vec3 direction = glm::normalize(point2 - point1);
                            glm::vec3 midpoint = (point1 + point2) / 2.0f;  // 두 점의 중간 위치

                            // 랜덤 방향의 선을 생성
                            glm::vec3 v1 = midpoint - direction * size / 2.0f;
                            glm::vec3 v2 = midpoint + direction * size / 2.0f;

                            // 새로운 랜덤 선 추가
                            glm::vec3 color(dis(gen), dis(gen), dis(gen)); // 랜덤 색상
                            insert_line(v1, v2, color, shapes);
                            // 삭제할 도형 배열에 넣기
                            deleteShapes.push_back(shape);
                            deleteShapes.push_back(*selectedShape);

                            break;
                        }
                        case 3:
                        {
                            // 점과 선이 합쳐졌을 때 랜덤한 삼각형 생성
                            glm::vec3 pointVertex = selectedShape->type == POINT_ ? selectedShape->vertices[0] : shape.vertices[0];
                            vector<glm::vec3> lineVertices = selectedShape->type == LINE ? selectedShape->vertices : shape.vertices;

                            glm::vec3 v1 = lineVertices[0];  // 선의 첫 번째 정점
                            glm::vec3 v2 = lineVertices[1];  // 선의 두 번째 정점

                            // 랜덤 크기 설정
                            std::uniform_real_distribution<> size_dis(0.05f, 0.1f); // 삼각형 크기 0.05 ~ 0.1
                            float size = size_dis(gen);

                            // 랜덤 삼각형의 세 정점을 생성
                            glm::vec3 center = (v1 + v2 + pointVertex) / 3.0f;  // 선과 점의 중간에서 삼각형 생성
                            glm::vec3 randV1 = glm::vec3(center.x - size, center.y - size, 0.0f);
                            glm::vec3 randV2 = glm::vec3(center.x + size, center.y - size, 0.0f);
                            glm::vec3 randV3 = glm::vec3(center.x, center.y + size, 0.0f);

                            // 새로운 랜덤 삼각형 추가
                            glm::vec3 color(dis(gen), dis(gen), dis(gen)); // 랜덤 색상
                            insert_triangle(randV1, randV2, randV3, color, shapes);
                            // 삭제할 도형 배열에 넣기
                            deleteShapes.push_back(shape);
                            deleteShapes.push_back(*selectedShape);

                            break;
                        }
                        case 4:
                        {
                            // 선택된 도형과 충돌한 경우 랜덤한 크기의 사각형 생성
                            std::uniform_real_distribution<> size_dis(0.05f, 0.1f); // 0.05 ~ 0.1 크기의 사각형 생성
                            float size = size_dis(gen);

                            glm::vec3 center = (vertex + selectedShape->vertices[0]) / 2.0f; // 두 도형의 중간 좌표에 사각형 생성
                            glm::vec3 v1(center.x - size / 2.0f, center.y - size / 2.0f, 0.0f);
                            glm::vec3 v2(center.x + size / 2.0f, center.y - size / 2.0f, 0.0f);
                            glm::vec3 v3(center.x - size / 2.0f, center.y + size / 2.0f, 0.0f);
                            glm::vec3 v4(center.x + size / 2.0f, center.y + size / 2.0f, 0.0f);

                            // 새 사각형 추가: v1, v2, v3, v4로 두 개의 삼각형을 만듭니다.
                            glm::vec3 color(dis(gen), dis(gen), dis(gen)); // 랜덤 색상
                            insert_square(v1, v2, v3, v2, v3, v4, color, shapes); // v2, v3를 두 번 넣는 문제 해결
                            // 삭제할 도형 배열에 넣기
                            deleteShapes.push_back(shape);
                            deleteShapes.push_back(*selectedShape);

                            break;
                        }
                        case 5:
                        {
                            // 랜덤한 오각형을 생성
                            glm::vec3 center = (vertex + selectedShape->vertices[0]) / 2.0f; // 두 도형의 중간 좌표에 오각형 생성
                            std::uniform_real_distribution<> size_dis(0.05f, 0.1f); // 0.05 ~ 0.1 크기의 오각형 생성
                            float radius = size_dis(gen);

                            // 오각형의 5개 정점 생성
                            std::vector<glm::vec3> pentagonVertices;
                            for (int i = 0; i < 5; ++i) {
                                float angle = i * (2.0f * M_PI / 5);  // 360도를 5등분
                                float x = center.x + radius * cos(angle);
                                float y = center.y + radius * sin(angle);
                                pentagonVertices.push_back(glm::vec3(x, y, 0.0f));
                            }

                            // 새로운 랜덤 오각형 추가
                            glm::vec3 color(dis(gen), dis(gen), dis(gen)); // 랜덤 색상
                            shapes.push_back({
                                PENTA, pentagonVertices, color, 5  // PENTA 타입으로 추가
                                });

                            deleteShapes.push_back(shape);
                            deleteShapes.push_back(*selectedShape);
                            break;
                        }
                        case 6:
                        { 
                            // 랜덤한 육각형을 생성
                            glm::vec3 center = (vertex + selectedShape->vertices[0]) / 2.0f; // 두 도형의 중간 좌표에 육각형 생성
                            std::uniform_real_distribution<> size_dis(0.05f, 0.1f); // 0.05 ~ 0.1 크기의 육각형 생성
                            float radius = size_dis(gen);

                            // 육각형의 6개 정점 생성
                            std::vector<glm::vec3> hexagonVertices;
                            for (int i = 0; i < 6; ++i) {
                                float angle = i * (2.0f * M_PI / 6);  // 360도를 6등분
                                float x = center.x + radius * cos(angle);
                                float y = center.y + radius * sin(angle);
                                hexagonVertices.push_back(glm::vec3(x, y, 0.0f));
                            }

                            // 새로운 랜덤 육각형 추가
                            glm::vec3 color(dis(gen), dis(gen), dis(gen)); // 랜덤 색상
                            shapes.push_back({
                                HEXA, hexagonVertices, color, 6  // 육각형 생성
                                });

                            deleteShapes.push_back(shape);
                            deleteShapes.push_back(*selectedShape);
                            break; 
                        }
                        default:
                            break;
                        }

                        selectedShape = nullptr;
                        isDragging = false;
                        break;  // 도형의 정점 중 하나라도 조건을 만족하면 해당 도형을 찾았으므로 break
                    }
                }

            }

            // deleteShapes 배열에 있는 도형들을 shapes에서 삭제
            for (const auto& deleteShape : deleteShapes) {
                shapes.erase(std::remove(shapes.begin(), shapes.end(), deleteShape), shapes.end());
            }

            InitBuffer();  // 버퍼 업데이트 (도형 이동 반영)
            glutPostRedisplay();  // 화면 다시 그리기 요청
        }

        isDragging = false;
        selectedShape = nullptr;
    }
}

void motion(int x, int y) {
    if (isDragging && selectedShape != nullptr) {
        float normalizedX = (float(x) / width) * 2.0f - 1.0f;
        float normalizedY = 1.0f - (float(y) / height) * 2.0f;

        glm::vec2 mouseDelta = glm::vec2(normalizedX, normalizedY) - lastMousePosition;

        // 선택된 도형의 모든 정점을 이동
        for (glm::vec3& vertex : selectedShape->vertices) {
            vertex.x += mouseDelta.x;
            vertex.y += mouseDelta.y;
        }

        lastMousePosition = glm::vec2(normalizedX, normalizedY);  // 마지막 마우스 위치 업데이트

        InitBuffer();  // 버퍼 업데이트 (도형 이동 반영)
        glutPostRedisplay();  // 화면 다시 그리기 요청
    }
}

void makeShape3() {
    float x = 0.0f, y = 0.0f;

    // 기존 선분 생성 코드
    for (int i = 0; i < 3; ++i) { // 선분 생성
        x = dis(gen);
        y = dis(gen);
        float x2 = dis(gen), y2 = dis(gen);
        insert_line(glm::vec3(x, y, 0.0f), glm::vec3(x2, y2, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), shapes);
    }

    // 삼각형 생성 코드
    std::uniform_real_distribution<> size_dis(0.05f, 0.1f);  // 삼각형과 사각형, 오각형 크기를 0.05 ~ 0.1 사이에서 랜덤하게 생성

    for (int i = 0; i < 3; ++i) { // 삼각형을 3개 생성
        glm::vec3 v1(dis(gen), dis(gen), 0.0f);
        float size = size_dis(gen);
        glm::vec3 v2(v1.x + size, v1.y, 0.0f);
        glm::vec3 v3(v1.x + size / 2.0f, v1.y + size, 0.0f);
        glm::vec3 color(dis(gen), dis(gen), dis(gen));
        insert_triangle(v1, v2, v3, color, shapes);
    }

    // 사각형 생성 코드
    for (int i = 0; i < 3; ++i) {
        glm::vec3 v1(dis(gen), dis(gen), 0.0f);
        float size = size_dis(gen);
        glm::vec3 v2(v1.x + size, v1.y, 0.0f);
        glm::vec3 v3(v1.x, v1.y + size, 0.0f);
        glm::vec3 v4(v1.x + size, v1.y + size, 0.0f);
        glm::vec3 color(dis(gen), dis(gen), dis(gen));
        insert_square(v1, v2, v3, v2, v3, v4, color, shapes);
    }

    // 오각형 생성 코드
    for (int i = 0; i < 3; ++i) {
        glm::vec3 center(dis(gen), dis(gen), 0.0f);
        float radius = size_dis(gen);
        std::vector<glm::vec3> pentagonVertices;
        for (int j = 0; j < 5; ++j) {
            float angle = j * (2.0f * M_PI / 5);  // 360도를 5등분
            float x = center.x + radius * cos(angle);
            float y = center.y + radius * sin(angle);
            pentagonVertices.push_back(glm::vec3(x, y, 0.0f));
        }
        glm::vec3 color(dis(gen), dis(gen), dis(gen));
        shapes.push_back({
            PENTA, pentagonVertices, color, 5  // PENTA 타입으로 추가
            });
    }

    // 점 생성 코드 (점은 작은 사각형처럼 표현)
    for (int i = 0; i < 3; ++i) {
        glm::vec3 v1(dis(gen), dis(gen), 0.0f); // 점의 중심 좌표
        float size = 0.01f; // 점의 크기를 매우 작게 설정
        glm::vec3 v2(v1.x + size, v1.y, 0.0f);
        glm::vec3 v3(v1.x, v1.y + size, 0.0f);
        glm::vec3 v4(v1.x + size, v1.y + size, 0.0f);
        glm::vec3 color(dis(gen), dis(gen), dis(gen)); // 랜덤한 색상

        // 점처럼 보이게 작은 사각형으로 처리
        insert_point2(v1, v2, v3, v2, v3, v4, color, shapes);
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
    glutCreateWindow("실습 12번");

    //glew 초기화
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    make_shaderProgram();
    makeShape3();
    InitBuffer();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyBoard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);  // 마우스 이동 함수 등록
    glutMainLoop();

    return 0;
}

int currentIndex = 0;

void drawShapes() {
    for (const Shape_gl& shape : shapes) {
        if (shape.type == TRIANGLE) {
            glDrawArrays(GL_TRIANGLES, currentIndex, 3); // 삼각형은 3개의 정점
            currentIndex += 3;
        }
        else if (shape.type == SQUARE) {
            glDrawArrays(GL_TRIANGLES, currentIndex, 6); // 사각형은 2개의 삼각형 (6개의 정점)
            currentIndex += 6;
        }
        else if (shape.type == PENTA) {
            glDrawArrays(GL_TRIANGLE_FAN, currentIndex, 5); // 오각형은 5개의 정점
            currentIndex += 5;
        }
        else if (shape.type == POINT_) {
            glDrawArrays(GL_TRIANGLES, currentIndex, 6); // 사각형은 2개의 삼각형 (6개의 정점)
            currentIndex += 6;
        }
        else if (shape.type == LINE) {
            glLineWidth(2.0f);
            glDrawArrays(GL_LINES, currentIndex, 2);
            currentIndex += 2;
        }
        else if (shape.type == HEXA) {
            glDrawArrays(GL_TRIANGLE_FAN, currentIndex, 6); // 육각형은 6개의 정점
            currentIndex += 6;
        }
    }
}

GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    glBindVertexArray(vao);
    currentIndex = 0;  // 매번 그리기 시작할 때 정점 인덱스를 0으로 초기화
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

    vector<glm::vec3> allVertices;
    vector<glm::vec3> allColors;

    for (const Shape_gl& shape : shapes) {
        allVertices.insert(allVertices.end(), shape.vertices.begin(), shape.vertices.end());

        for (size_t i = 0; i < shape.vertices.size(); ++i) {
            allColors.push_back(shape.color); 
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(glm::vec3), allVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, allColors.size() * sizeof(glm::vec3), allColors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
}
