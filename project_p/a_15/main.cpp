#include <iostream>   // 표준 입출력 라이브러리 포함
#include <gl/glew.h>  // GLEW 라이브러리 포함 (OpenGL 확장 기능을 사용하기 위해 필요)
#include <gl/freeglut.h>  // GLUT 라이브러리 포함 (윈도우 창 및 사용자 입력 처리를 위해 사용)
#include <gl/freeglut_ext.h>  // GLUT 확장 기능 포함
#include <gl/glm/glm/glm.hpp>  // GLM 라이브러리 포함 (수학적인 계산을 돕기 위한 라이브러리)
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector>  // 표준 벡터 라이브러리 포함 (동적 배열을 사용하기 위해 필요)

#include "filetobuf.h"  // 쉐이더 소스 파일을 버퍼로 읽어오는 함수를 정의한 헤더파일
#include "ReadObj.h"
#include "LoadTexture.h"

using namespace std;  // 네임스페이스 std 사용으로 코드 내에서 std:: 생략 가능

// 함수 선언부
void make_vertexShaders();  // 버텍스 쉐이더를 만드는 함수 선언
void make_fragmentShaders();  // 프래그먼트 쉐이더를 만드는 함수 선언
void InitBuffer();  // 버퍼 초기화 함수 선언
void make_shaderProgram();  // 쉐이더 프로그램을 만드는 함수 선언
GLvoid drawScene(GLvoid);  // 화면을 그리는 함수 선언
GLvoid Reshape(int w, int h);  // 화면 크기가 변경될 때 호출되는 함수 선언

// 전역 변수 선언부
GLint width, height;  // 창의 너비와 높이를 저장하는 변수
GLuint shaderProgramID;  // 쉐이더 프로그램의 ID를 저장하는 변수
GLuint vertexShader;  // 버텍스 쉐이더 ID를 저장하는 변수
GLuint fragmentShader;  // 프래그먼트 쉐이더 ID를 저장하는 변수

GLchar* vertexSource, * fragmentSource;  // 쉐이더 소스 코드를 저장할 변수들

GLuint xVBO, xVAO;
GLuint yVBO, yVAO;
GLuint zVBO, zVAO;

// 여러 3D 모델을 관리하는 벡터
vector<Model> models;
vector<GLuint> vaos;
vector<vector<GLuint>> vbos;

const Vertex x[2] = {
    {-0.8f,0.0f, 0.0f},
    {0.8f,0.0f, 0.0f}
};

const Vertex y[2] = {
    {0.0f,-0.8f, 0.0f},
    {0.0f, 0.8f, 0.0f}
};

const Vertex z[2] = {
    {0.0f, 0.0f, -0.8f},
    {0.0f, 0.0f, 0.8f}
};

int number_status = 0;
char key_result = ' ';
bool x_status = false, y_status = false;

void timer(int value) {
    if (number_status == 0) {
        return;
    }

    float speed = 0.5f;
    for (int i = 0; i < number_status; ++i) {
        if (key_result == 'x' || key_result == 'X') {
            if (x_status) {
                models[i].modelMatrix = glm::rotate(models[i].modelMatrix, glm::radians(speed), glm::vec3(1.0, 0.0, 0.0));
            }
            else {
                models[i].modelMatrix = glm::rotate(models[i].modelMatrix, glm::radians(-speed), glm::vec3(1.0, 0.0, 0.0));
            }
        }

        if (key_result == 'y' || key_result == 'Y') {
            if (y_status) {
                models[i].modelMatrix = glm::rotate(models[i].modelMatrix, glm::radians(speed), glm::vec3(0.0, 1.0, 0.0));
            }
            else {
                models[i].modelMatrix = glm::rotate(models[i].modelMatrix, glm::radians(-speed), glm::vec3(0.0, 1.0, 0.0));
            }
        }
    }
    glutPostRedisplay();  // 화면 다시 그리기 요청
    glutTimerFunc(16, timer, 0);
}

void keyBoard(unsigned char key, int x, int y) {
    switch (key)
    {
    case 'x':
        key_result = 'x';
        x_status = true;
        break;
    case 'X':
        key_result = 'X';
        x_status = false;
        break;
    case 'y':
        key_result = 'y';
        y_status = true;
        break;
    case 'Y':
        key_result = 'Y';
        y_status = false;
        break;
    case '0':
        number_status = 0;
        break;
    case '1':
        number_status = 1;
        glutTimerFunc(0, timer, 0);
        break;
    case '2':
        number_status = 2;
        glutTimerFunc(0, timer, 0);
        break;
    case '3':
        number_status = 3;
        glutTimerFunc(0, timer, 0);
        break;
    default:
        break;
    }
    glutPostRedisplay();  // 화면 다시 그리기 요청
}

void specialKeys(int key, int x, int y) {
    glutPostRedisplay();  // 화면 다시 그리기 요청
}
// 마우스 입력을 처리하는 함수
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        std::cout << "x = " << x << " y = " << y << std::endl;  // 마우스 클릭 좌표 출력
    glutPostRedisplay();  // 키보드 입력 후 화면을 다시 그리도록 요청
}

// 메인 함수
int main(int argc, char** argv) {

    width = 800;  // 창의 기본 너비 설정
    height = 600;  // 창의 기본 높이 설정

    // 윈도우 생성
    glutInit(&argc, argv);  // GLUT 초기화
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);  // 화면 모드 설정 (더블 버퍼링, RGBA 컬러 모드)
    glutInitWindowPosition(100, 100);  // 창의 시작 위치 설정
    glutInitWindowSize(width, height);  // 창의 크기 설정
    glutCreateWindow("실습 15번");  // 창 생성 및 제목 설정

    // GLEW 초기화
    glewExperimental = GL_TRUE;  // GLEW 실험적 기능 활성화
    if (glewInit() != GLEW_OK) {  // GLEW 초기화 및 오류 체크
        cerr << "Unable to initialize GLEW" << endl;  // GLEW 초기화 실패 시 오류 메시지 출력
        exit(EXIT_FAILURE);  // 프로그램 종료
    }
    else
        cout << "GLEW Initialized\n";  // 초기화 성공 시 메시지 출력

    make_shaderProgram();  // 쉐이더 프로그램 생성
        
    // 모델들을 로드하고 벡터에 추가
    Model modelSqu, modelCone, modelSphere, modelCylinder;
    read_obj_file("box3.obj", modelSqu, "cube");
    // 정육면체
    modelSqu.modelMatrix = glm::rotate(modelSqu.modelMatrix, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0));
    modelSqu.modelMatrix = glm::rotate(modelSqu.modelMatrix, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0));
    modelSqu.modelMatrix = glm::scale(modelSqu.modelMatrix, glm::vec3(0.2, 0.2, 0.2));
    modelSqu.modelMatrix = glm::translate(modelSqu.modelMatrix, glm::vec3(-3.0, 0.0, 0.0));
    modelSqu.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelSqu);

    Model modelXLine, modelYLine, modelZLine;
    modelXLine.name = "xLine";
    modelXLine.vertices.push_back(x[0]);
    modelXLine.vertices.push_back(x[1]);
    modelXLine.colors.push_back(glm::vec3(1.0, 0.0, 0.0));
    modelXLine.modelMatrix = glm::rotate(modelXLine.modelMatrix, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0));
    modelXLine.modelMatrix = glm::rotate(modelXLine.modelMatrix, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0));

    modelYLine.name = "yLine";
    modelYLine.vertices.push_back(y[0]);
    modelYLine.vertices.push_back(y[1]);
    modelYLine.colors.push_back(glm::vec3(0.0, 1.0, 0.0));
    modelYLine.modelMatrix = glm::rotate(modelYLine.modelMatrix, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0));
    modelYLine.modelMatrix = glm::rotate(modelYLine.modelMatrix, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0));

    modelZLine.name = "zLine";
    modelZLine.vertices.push_back(z[0]);
    modelZLine.vertices.push_back(z[1]);
    modelZLine.colors.push_back(glm::vec3(0.0, 0.0, 1.0));
    modelZLine.modelMatrix = glm::rotate(modelZLine.modelMatrix, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0));
    modelZLine.modelMatrix = glm::rotate(modelZLine.modelMatrix, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0));

    models.push_back(modelXLine);
    models.push_back(modelYLine);
    models.push_back(modelZLine);

    InitBuffer();  // 버퍼 초기화

    // 콜백 함수 등록
    glutDisplayFunc(drawScene);  // 화면을 그리는 함수 등록
    glutReshapeFunc(Reshape);  // 화면 크기 변경 시 호출되는 함수 등록
    glutKeyboardFunc(keyBoard);  // 키보드 입력 처리 함수 등록
    glutMouseFunc(mouse);  // 마우스 입력 처리 함수 등록
    glutSpecialFunc(specialKeys);  // 특수 키 (방향키 등) 입력 처리 함수 등록
    glutMainLoop();  // 이벤트 루프 시작

    return 0;  // 프로그램 정상 종료
}

// 화면을 그리는 함수
GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);  // 화면을 흰색으로 초기화
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 컬러 버퍼와 깊이 버퍼 초기화

    glUseProgram(shaderProgramID);  // 쉐이더 프로그램 사용


    glEnable(GL_DEPTH_TEST);
    // 각 모델을 그리기
    for (size_t i = 0; i < models.size(); ++i) {
        glBindVertexArray(vaos[i]);

        GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");

        if (models[i].name == "cube") {
            GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            // 면 그리기
            glUniform1i(modelStatus, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
        }
        else if (models[i].name == "xLine" || models[i].name == "yLine" || models[i].name == "zLine") {
            GLint lineLoc = glGetUniformLocation(shaderProgramID, "Line");
            glUniformMatrix4fv(lineLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            glUniform1i(modelStatus, 1);
            glLineWidth(1.0f);
            glDrawArrays(GL_LINES, 0, 2);
        }

        glBindVertexArray(0);
    }

    glDisable(GL_DEPTH_TEST);

    glutSwapBuffers();  // 더블 버퍼링 사용, 화면 갱신

    // OpenGL 오류 체크 루프
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;  // 오류 메시지 출력
    }
}

// 화면 크기가 변경될 때 호출되는 함수
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);  // 뷰포트 크기 설정
}

// 버텍스 쉐이더를 만드는 함수
void make_vertexShaders() {
    vertexSource = filetobuf("vertex.glsl");  // 버텍스 쉐이더 파일 읽기
    vertexShader = glCreateShader(GL_VERTEX_SHADER);  // 버텍스 쉐이더 생성
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);  // 쉐이더 소스 코드 설정
    glCompileShader(vertexShader);  // 쉐이더 컴파일

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);  // 컴파일 성공 여부 확인

    if (!result) {  // 컴파일 실패 시
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);  // 오류 로그 받기
        cerr << "ERROR: vertex shader 컴파일 실패 \n" << errorLog << endl;  // 오류 메시지 출력
        return;
    }
}

// 프래그먼트 쉐이더를 만드는 함수
void make_fragmentShaders() {
    fragmentSource = filetobuf("fragment.glsl");  // 프래그먼트 쉐이더 파일 읽기
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  // 프래그먼트 쉐이더 생성
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);  // 쉐이더 소스 코드 설정
    glCompileShader(fragmentShader);  // 쉐이더 컴파일

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);  // 컴파일 성공 여부 확인

    if (!result) {  // 컴파일 실패 시
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);  // 오류 로그 받기
        cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << endl;  // 오류 메시지 출력
        return;
    }
}

// 쉐이더 프로그램을 만드는 함수
void make_shaderProgram() {
    make_vertexShaders();  // 버텍스 쉐이더 생성
    make_fragmentShaders();  // 프래그먼트 쉐이더 생성

    shaderProgramID = glCreateProgram();  // 쉐이더 프로그램 생성
    glAttachShader(shaderProgramID, vertexShader);  // 버텍스 쉐이더 프로그램에 첨부
    glAttachShader(shaderProgramID, fragmentShader);  // 프래그먼트 쉐이더 프로그램에 첨부
    glLinkProgram(shaderProgramID);  // 쉐이더 프로그램 링크

    glDeleteShader(vertexShader);  // 버텍스 쉐이더 삭제 (이미 프로그램에 포함되었으므로)
    glDeleteShader(fragmentShader);  // 프래그먼트 쉐이더 삭제

    glUseProgram(shaderProgramID);  // 쉐이더 프로그램 사용
}

// 버퍼 초기화 함수
void InitBuffer() {

    // 각 모델에 대한 VAO, VBO, EBO 설정
    vaos.resize(models.size());
    vbos.resize(models.size(), vector<GLuint>(4)); // 모델마다 4개의 VBO가 필요 (정점, 색상, 법선, 텍스처 좌표)

    for (size_t i = 0; i < models.size(); ++i) {
        glGenVertexArrays(1, &vaos[i]);
        glBindVertexArray(vaos[i]);

        // VBO 생성
        glGenBuffers(4, vbos[i].data());

        // 정점 버퍼 설정
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][0]);
        glBufferData(GL_ARRAY_BUFFER, models[i].vertices.size() * sizeof(glm::vec3), models[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0에 정점 할당
        glEnableVertexAttribArray(0);

        // 색상 버퍼 설정
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][1]);
        glBufferData(GL_ARRAY_BUFFER, models[i].colors.size() * sizeof(glm::vec3), models[i].colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 색상 할당
        glEnableVertexAttribArray(1);

        // 면 인덱스 데이터 (EBO) 설정
        vector<unsigned int> indices;
        for (const Face& face : models[i].faces) {
            indices.push_back(face.v1);
            indices.push_back(face.v2);
            indices.push_back(face.v3);
        }

        GLuint ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }
}
