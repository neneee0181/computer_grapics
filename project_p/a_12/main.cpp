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
    PENTA,  // ������ �߰�
    HEXA
};

struct Shape_gl {
    ShapeType type;
    vector<glm::vec3> vertices; // ������ ����
    glm::vec3 color; // ������ ����
    int vertex = 0;
    // == ������ �����ε�
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

// ��� ������ ��� ����
vector<Shape_gl> shapes;

GLuint vao, vbo[2];

float stepSize = 0.01f;    // �� ���� ������Ʈ���� �̵��� �Ÿ�

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
        // ���콺 Ŭ�� �� ���� ���� ���� Ȯ��
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
            std::vector<Shape_gl> deleteShapes;  // ������ ������ �����ϴ� �迭

            // ���õ� ������ ���� ���콺 ��ǥ�� �ִ� �ٸ� ������ ��.
            for (Shape_gl& shape : shapes) {

                for (glm::vec3& vertex : shape.vertices) {
                    if (abs(vertex.x - normalizedX) < 0.05f && abs(vertex.y - normalizedY) < 0.05f) {

                        if (&shape == selectedShape) {
                            continue;
                        }
                        if (selectedShape == nullptr) {
                            continue;
                        }
                        cout << "���� �߰�: " << shape.type << endl;

                        int num = selectedShape->vertex + shape.vertex;

                        if (num >= 7) {
                            num = num % 6;
                        }
                        switch (num)
                        {
                        case 1:
                        {
                            // ���� ����
                            std::uniform_real_distribution<> size_dis(0.01f, 0.05f);  // ���� ũ�⸦ 0.01 ~ 0.05 ���̷� �����ϰ� ����
                            float size = size_dis(gen);

                            glm::vec3 center = (vertex + selectedShape->vertices[0]) / 2.0f; // �� ������ �߰� ��ǥ�� �� ����
                            glm::vec3 v1(center.x - size / 2.0f, center.y - size / 2.0f, 0.0f);
                            glm::vec3 v2(v1.x + size, v1.y, 0.0f);
                            glm::vec3 v3(v1.x, v1.y + size, 0.0f);
                            glm::vec3 v4(v1.x + size, v1.y + size, 0.0f);
                            glm::vec3 color(dis(gen), dis(gen), dis(gen));  // ������ ����

                            // ��ó�� ���̰� ���� �簢������ ó��
                            insert_point2(v1, v2, v3, v2, v3, v4, color, shapes);
                            // ������ ���� �迭�� �ֱ�
                            deleteShapes.push_back(shape);
                            deleteShapes.push_back(*selectedShape);

                            break;
                        }
                        case 2:
                        {
                            // ���� ���� ������ ������ ���� ����
                            glm::vec3 point1 = selectedShape->vertices[0];
                            glm::vec3 point2 = shape.vertices[0];

                            // ���� ũ�� ����
                            std::uniform_real_distribution<> size_dis(0.1f, 0.3f); // ���� ���� ���� 0.1 ~ 0.3
                            float size = size_dis(gen);

                            // ���� ������ ���� (�� ���� �մ� ������ ����)
                            glm::vec3 direction = glm::normalize(point2 - point1);
                            glm::vec3 midpoint = (point1 + point2) / 2.0f;  // �� ���� �߰� ��ġ

                            // ���� ������ ���� ����
                            glm::vec3 v1 = midpoint - direction * size / 2.0f;
                            glm::vec3 v2 = midpoint + direction * size / 2.0f;

                            // ���ο� ���� �� �߰�
                            glm::vec3 color(dis(gen), dis(gen), dis(gen)); // ���� ����
                            insert_line(v1, v2, color, shapes);
                            // ������ ���� �迭�� �ֱ�
                            deleteShapes.push_back(shape);
                            deleteShapes.push_back(*selectedShape);

                            break;
                        }
                        case 3:
                        {
                            // ���� ���� �������� �� ������ �ﰢ�� ����
                            glm::vec3 pointVertex = selectedShape->type == POINT_ ? selectedShape->vertices[0] : shape.vertices[0];
                            vector<glm::vec3> lineVertices = selectedShape->type == LINE ? selectedShape->vertices : shape.vertices;

                            glm::vec3 v1 = lineVertices[0];  // ���� ù ��° ����
                            glm::vec3 v2 = lineVertices[1];  // ���� �� ��° ����

                            // ���� ũ�� ����
                            std::uniform_real_distribution<> size_dis(0.05f, 0.1f); // �ﰢ�� ũ�� 0.05 ~ 0.1
                            float size = size_dis(gen);

                            // ���� �ﰢ���� �� ������ ����
                            glm::vec3 center = (v1 + v2 + pointVertex) / 3.0f;  // ���� ���� �߰����� �ﰢ�� ����
                            glm::vec3 randV1 = glm::vec3(center.x - size, center.y - size, 0.0f);
                            glm::vec3 randV2 = glm::vec3(center.x + size, center.y - size, 0.0f);
                            glm::vec3 randV3 = glm::vec3(center.x, center.y + size, 0.0f);

                            // ���ο� ���� �ﰢ�� �߰�
                            glm::vec3 color(dis(gen), dis(gen), dis(gen)); // ���� ����
                            insert_triangle(randV1, randV2, randV3, color, shapes);
                            // ������ ���� �迭�� �ֱ�
                            deleteShapes.push_back(shape);
                            deleteShapes.push_back(*selectedShape);

                            break;
                        }
                        case 4:
                        {
                            // ���õ� ������ �浹�� ��� ������ ũ���� �簢�� ����
                            std::uniform_real_distribution<> size_dis(0.05f, 0.1f); // 0.05 ~ 0.1 ũ���� �簢�� ����
                            float size = size_dis(gen);

                            glm::vec3 center = (vertex + selectedShape->vertices[0]) / 2.0f; // �� ������ �߰� ��ǥ�� �簢�� ����
                            glm::vec3 v1(center.x - size / 2.0f, center.y - size / 2.0f, 0.0f);
                            glm::vec3 v2(center.x + size / 2.0f, center.y - size / 2.0f, 0.0f);
                            glm::vec3 v3(center.x - size / 2.0f, center.y + size / 2.0f, 0.0f);
                            glm::vec3 v4(center.x + size / 2.0f, center.y + size / 2.0f, 0.0f);

                            // �� �簢�� �߰�: v1, v2, v3, v4�� �� ���� �ﰢ���� ����ϴ�.
                            glm::vec3 color(dis(gen), dis(gen), dis(gen)); // ���� ����
                            insert_square(v1, v2, v3, v2, v3, v4, color, shapes); // v2, v3�� �� �� �ִ� ���� �ذ�
                            // ������ ���� �迭�� �ֱ�
                            deleteShapes.push_back(shape);
                            deleteShapes.push_back(*selectedShape);

                            break;
                        }
                        case 5:
                        {
                            // ������ �������� ����
                            glm::vec3 center = (vertex + selectedShape->vertices[0]) / 2.0f; // �� ������ �߰� ��ǥ�� ������ ����
                            std::uniform_real_distribution<> size_dis(0.05f, 0.1f); // 0.05 ~ 0.1 ũ���� ������ ����
                            float radius = size_dis(gen);

                            // �������� 5�� ���� ����
                            std::vector<glm::vec3> pentagonVertices;
                            for (int i = 0; i < 5; ++i) {
                                float angle = i * (2.0f * M_PI / 5);  // 360���� 5���
                                float x = center.x + radius * cos(angle);
                                float y = center.y + radius * sin(angle);
                                pentagonVertices.push_back(glm::vec3(x, y, 0.0f));
                            }

                            // ���ο� ���� ������ �߰�
                            glm::vec3 color(dis(gen), dis(gen), dis(gen)); // ���� ����
                            shapes.push_back({
                                PENTA, pentagonVertices, color, 5  // PENTA Ÿ������ �߰�
                                });

                            deleteShapes.push_back(shape);
                            deleteShapes.push_back(*selectedShape);
                            break;
                        }
                        case 6:
                        { 
                            // ������ �������� ����
                            glm::vec3 center = (vertex + selectedShape->vertices[0]) / 2.0f; // �� ������ �߰� ��ǥ�� ������ ����
                            std::uniform_real_distribution<> size_dis(0.05f, 0.1f); // 0.05 ~ 0.1 ũ���� ������ ����
                            float radius = size_dis(gen);

                            // �������� 6�� ���� ����
                            std::vector<glm::vec3> hexagonVertices;
                            for (int i = 0; i < 6; ++i) {
                                float angle = i * (2.0f * M_PI / 6);  // 360���� 6���
                                float x = center.x + radius * cos(angle);
                                float y = center.y + radius * sin(angle);
                                hexagonVertices.push_back(glm::vec3(x, y, 0.0f));
                            }

                            // ���ο� ���� ������ �߰�
                            glm::vec3 color(dis(gen), dis(gen), dis(gen)); // ���� ����
                            shapes.push_back({
                                HEXA, hexagonVertices, color, 6  // ������ ����
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
                        break;  // ������ ���� �� �ϳ��� ������ �����ϸ� �ش� ������ ã�����Ƿ� break
                    }
                }

            }

            // deleteShapes �迭�� �ִ� �������� shapes���� ����
            for (const auto& deleteShape : deleteShapes) {
                shapes.erase(std::remove(shapes.begin(), shapes.end(), deleteShape), shapes.end());
            }

            InitBuffer();  // ���� ������Ʈ (���� �̵� �ݿ�)
            glutPostRedisplay();  // ȭ�� �ٽ� �׸��� ��û
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

        // ���õ� ������ ��� ������ �̵�
        for (glm::vec3& vertex : selectedShape->vertices) {
            vertex.x += mouseDelta.x;
            vertex.y += mouseDelta.y;
        }

        lastMousePosition = glm::vec2(normalizedX, normalizedY);  // ������ ���콺 ��ġ ������Ʈ

        InitBuffer();  // ���� ������Ʈ (���� �̵� �ݿ�)
        glutPostRedisplay();  // ȭ�� �ٽ� �׸��� ��û
    }
}

void makeShape3() {
    float x = 0.0f, y = 0.0f;

    // ���� ���� ���� �ڵ�
    for (int i = 0; i < 3; ++i) { // ���� ����
        x = dis(gen);
        y = dis(gen);
        float x2 = dis(gen), y2 = dis(gen);
        insert_line(glm::vec3(x, y, 0.0f), glm::vec3(x2, y2, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), shapes);
    }

    // �ﰢ�� ���� �ڵ�
    std::uniform_real_distribution<> size_dis(0.05f, 0.1f);  // �ﰢ���� �簢��, ������ ũ�⸦ 0.05 ~ 0.1 ���̿��� �����ϰ� ����

    for (int i = 0; i < 3; ++i) { // �ﰢ���� 3�� ����
        glm::vec3 v1(dis(gen), dis(gen), 0.0f);
        float size = size_dis(gen);
        glm::vec3 v2(v1.x + size, v1.y, 0.0f);
        glm::vec3 v3(v1.x + size / 2.0f, v1.y + size, 0.0f);
        glm::vec3 color(dis(gen), dis(gen), dis(gen));
        insert_triangle(v1, v2, v3, color, shapes);
    }

    // �簢�� ���� �ڵ�
    for (int i = 0; i < 3; ++i) {
        glm::vec3 v1(dis(gen), dis(gen), 0.0f);
        float size = size_dis(gen);
        glm::vec3 v2(v1.x + size, v1.y, 0.0f);
        glm::vec3 v3(v1.x, v1.y + size, 0.0f);
        glm::vec3 v4(v1.x + size, v1.y + size, 0.0f);
        glm::vec3 color(dis(gen), dis(gen), dis(gen));
        insert_square(v1, v2, v3, v2, v3, v4, color, shapes);
    }

    // ������ ���� �ڵ�
    for (int i = 0; i < 3; ++i) {
        glm::vec3 center(dis(gen), dis(gen), 0.0f);
        float radius = size_dis(gen);
        std::vector<glm::vec3> pentagonVertices;
        for (int j = 0; j < 5; ++j) {
            float angle = j * (2.0f * M_PI / 5);  // 360���� 5���
            float x = center.x + radius * cos(angle);
            float y = center.y + radius * sin(angle);
            pentagonVertices.push_back(glm::vec3(x, y, 0.0f));
        }
        glm::vec3 color(dis(gen), dis(gen), dis(gen));
        shapes.push_back({
            PENTA, pentagonVertices, color, 5  // PENTA Ÿ������ �߰�
            });
    }

    // �� ���� �ڵ� (���� ���� �簢��ó�� ǥ��)
    for (int i = 0; i < 3; ++i) {
        glm::vec3 v1(dis(gen), dis(gen), 0.0f); // ���� �߽� ��ǥ
        float size = 0.01f; // ���� ũ�⸦ �ſ� �۰� ����
        glm::vec3 v2(v1.x + size, v1.y, 0.0f);
        glm::vec3 v3(v1.x, v1.y + size, 0.0f);
        glm::vec3 v4(v1.x + size, v1.y + size, 0.0f);
        glm::vec3 color(dis(gen), dis(gen), dis(gen)); // ������ ����

        // ��ó�� ���̰� ���� �簢������ ó��
        insert_point2(v1, v2, v3, v2, v3, v4, color, shapes);
    }
}

int main(int argc, char** argv) {

    width = 800;
    height = 600;

    //������ ����
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("�ǽ� 12��");

    //glew �ʱ�ȭ
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
    glutMotionFunc(motion);  // ���콺 �̵� �Լ� ���
    glutMainLoop();

    return 0;
}

int currentIndex = 0;

void drawShapes() {
    for (const Shape_gl& shape : shapes) {
        if (shape.type == TRIANGLE) {
            glDrawArrays(GL_TRIANGLES, currentIndex, 3); // �ﰢ���� 3���� ����
            currentIndex += 3;
        }
        else if (shape.type == SQUARE) {
            glDrawArrays(GL_TRIANGLES, currentIndex, 6); // �簢���� 2���� �ﰢ�� (6���� ����)
            currentIndex += 6;
        }
        else if (shape.type == PENTA) {
            glDrawArrays(GL_TRIANGLE_FAN, currentIndex, 5); // �������� 5���� ����
            currentIndex += 5;
        }
        else if (shape.type == POINT_) {
            glDrawArrays(GL_TRIANGLES, currentIndex, 6); // �簢���� 2���� �ﰢ�� (6���� ����)
            currentIndex += 6;
        }
        else if (shape.type == LINE) {
            glLineWidth(2.0f);
            glDrawArrays(GL_LINES, currentIndex, 2);
            currentIndex += 2;
        }
        else if (shape.type == HEXA) {
            glDrawArrays(GL_TRIANGLE_FAN, currentIndex, 6); // �������� 6���� ����
            currentIndex += 6;
        }
    }
}

GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    glBindVertexArray(vao);
    currentIndex = 0;  // �Ź� �׸��� ������ �� ���� �ε����� 0���� �ʱ�ȭ
    drawShapes(); // ���� �׸���
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
