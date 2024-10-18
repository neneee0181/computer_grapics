#include <iostream>   // 표준 입출력 라이브러리 포함
#include <gl/glew.h>  // GLEW 라이브러리 포함 (OpenGL 확장 기능을 사용하기 위해 필요)
#include <gl/freeglut.h>  // GLUT 라이브러리 포함 (윈도우 창 및 사용자 입력 처리를 위해 사용)
#include <gl/freeglut_ext.h>  // GLUT 확장 기능 포함
#include <gl/glm/glm/glm.hpp>  // GLM 라이브러리 포함 (수학적인 계산을 돕기 위한 라이브러리)
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector>  // 표준 벡터 라이브러리 포함 (동적 배열을 사용하기 위해 필요)

#include "LoadObj.h"
#include "LoadTexture.h"
#include "shaderMaker.h"

using namespace std;  // 네임스페이스 std 사용으로 코드 내에서 std:: 생략 가능

// 함수 선언부
void InitBuffer();  // 버퍼 초기화 함수 선언
GLvoid drawScene(GLvoid);  // 화면을 그리는 함수 선언
GLvoid Reshape(int w, int h);  // 화면 크기가 변경될 때 호출되는 함수 선언

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

// 화면 크기가 변경될 때 호출되는 함수
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);  // 뷰포트 크기 설정
}

int currentSpiralIndex1 = 0;  // 1번 도형의 현재 스파이럴 인덱스
int currentSpiralIndex2 = 0;  // 2번 도형의 현재 스파이럴 인덱스

int Direction1 = 1;  // 1번 도형의 스파이럴 진행 방향 (1: 앞으로, -1: 뒤로)
int Direction2 = -1;  // 2번 도형의 스파이럴 진행 방향 (1: 앞으로, -1: 뒤로)

bool startModel2 = false;  // 2번 도형이 스파이럴을 돌기 시작했는지 여부

vector<Vertex> spirals_v;

char key_result = ' ';
bool isTimerRunning = false;  // 타이머가 실행 중인지 여부를 관리하는 플래그

glm::vec3 f_f1;
glm::vec3 f_f2;

void timer(int value) {

    if (!isTimerRunning) {
        return;  // 타이머가 멈추면 더 이상 갱신하지 않음
    }


    if (key_result == '1') {
        // 1번 도형 스파이럴 이동 처리
        currentSpiralIndex1 += Direction1;

        // 1번 도형이 스파이럴 끝에 도달했을 때 방향 전환
        if (currentSpiralIndex1 >= spirals_v.size() || currentSpiralIndex1 < 0) {
            Direction1 *= -1;
            currentSpiralIndex1 += Direction1;
        }

        // 스파이럴 경로의 1번 도형 위치
        Vertex currentPos1 = spirals_v[currentSpiralIndex1];
        float scaleFactor = 3.5f;  // 스파이럴 반지름 확장
        glm::vec3 scaledPos1 = glm::vec3(currentPos1.x * scaleFactor, currentPos1.y * scaleFactor, currentPos1.z * scaleFactor);

        // 1번 도형의 변환 설정
        glm::mat4 transformationMatrix1 = models[0].initialRotation;  // 초기 회전 적용
        transformationMatrix1 = glm::scale(transformationMatrix1, glm::vec3(0.2f, 0.2f, 0.2f));  // 스케일
        transformationMatrix1 = glm::translate(transformationMatrix1, glm::vec3(spirals_v[0].x, spirals_v[0].y, spirals_v[0].z));
        transformationMatrix1 = glm::translate(transformationMatrix1, scaledPos1);  // 위치 설정
        models[0].modelMatrix = transformationMatrix1;

        // 2번 도형 시작 여부 확인 (2초 후 시작)
        if (startModel2) {
            currentSpiralIndex2 += Direction2;

            // 2번 도형이 스파이럴 끝에 도달했을 때 방향 전환
            if (currentSpiralIndex2 >= spirals_v.size() || currentSpiralIndex2 < 0) {
                Direction2 *= -1;
                currentSpiralIndex2 += Direction2;
            }

            // 스파이럴 경로의 2번 도형 위치
            Vertex currentPos2 = spirals_v[currentSpiralIndex2];
            glm::vec3 scaledPos2 = glm::vec3(currentPos2.x * scaleFactor, currentPos2.y * scaleFactor, currentPos2.z * scaleFactor);

            // 2번 도형의 변환 설정
            glm::mat4 transformationMatrix2 = models[1].initialRotation;  // 초기 회전 적용
            transformationMatrix2 = glm::scale(transformationMatrix2, glm::vec3(0.2f, 0.2f, 0.2f));  // 스케일
            transformationMatrix2 = glm::translate(transformationMatrix2, glm::vec3(spirals_v[0].x, spirals_v[0].y, spirals_v[0].z));
            transformationMatrix2 = glm::translate(transformationMatrix2, scaledPos2);  // 위치 설정
            models[1].modelMatrix = transformationMatrix2;
        }
    }
    else if (key_result == '2') {
        float speed = 0.1f;

       

    }
    else if (key_result == '3') {
        // 1번 도형의 위치로 0번 도형을 이동
        
        glm::vec3 targetPosition1 = f_f2 - glm::vec3(models[0].modelMatrix[3]);  // 1번 도형의 위치 추출
        models[0].modelMatrix = glm::translate(models[0].modelMatrix, targetPosition1);

        glm::vec3 targetPosition2 = f_f1 - glm::vec3(models[1].modelMatrix[3]);  // 1번 도형의 위치 추출
        models[1].modelMatrix = glm::translate(models[1].modelMatrix, targetPosition2);
        
    }

    glutPostRedisplay();  // 화면 다시 그리기 요청
    glutTimerFunc(16, timer, value);  // 타이머 재설정 (약 60 FPS)
}

// 2번 도형 시작 타이머 (2초 후 시작)
void startModel2Timer(int value) {
    startModel2 = true;  // 2번 도형 스파이럴 시작
}

void makeSpiral() {
    spirals_v.clear();  // 기존 데이터를 초기화

    float radiusIncreaseRate = 0.001f;  // 반지름 증가율
    float angleStep = 0.1f;  // 각도 증가량 (라디안)
    int numTurns = 6;  // 스파이럴의 회전 수 (몇 바퀴 돌 것인지)
    int numPointsPerTurn = 100;  // 한 바퀴 당 생성할 점의 수
    float initialRadius = 0.0f;  // 초기 반지름

    for (int i = 0; i < numTurns * numPointsPerTurn; ++i) {
        float angle = i * angleStep;  // 각도 계산
        float radius = initialRadius + i * radiusIncreaseRate;  // 현재 반지름 계산

        float x = radius * cos(angle);  // x 좌표
        float z = radius * sin(angle);  // z 좌표

        spirals_v.push_back({ x, 0.0f, z });  // XZ 평면에 점 추가
    }
}

void keyBoard(unsigned char key, int x, int y) {
    switch (key)
    {
    case '1':
    {
        isTimerRunning = !isTimerRunning;

        key_result = '1';
        makeSpiral();
        Model modelSpiral;
        modelSpiral.name = "spiral";
        modelSpiral.vertices = spirals_v;
        modelSpiral.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
        modelSpiral.modelMatrix = glm::rotate(modelSpiral.modelMatrix, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0));
        modelSpiral.modelMatrix = glm::rotate(modelSpiral.modelMatrix, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0));

        models.push_back(modelSpiral);

        InitBuffer();

        // 1번 도형은 바로 시작
        glutTimerFunc(0, timer, 0);
        if (currentSpiralIndex2 == 0)
            currentSpiralIndex2 = spirals_v.size();
        // 2번 도형은 2초 후에 시작
        glutTimerFunc(0, startModel2Timer, 0);
        break;
    }
    case '2':
    {
        isTimerRunning = !isTimerRunning;
        key_result = '2';



        for (int i = 0; i < 2; ++i) {
            glm::mat4 transformationMatrix1 = models[i].initialRotation;  // 초기 회전 적용
            transformationMatrix1 = glm::scale(transformationMatrix1, glm::vec3(0.2f, 0.2f, 0.2f));  // 스케일
            transformationMatrix1 = glm::translate(transformationMatrix1, models[i].translationOffset);
            models[i].modelMatrix = transformationMatrix1;
        }

        glutTimerFunc(0, timer, 0);

        break;
    }
    case '3':
        isTimerRunning = !isTimerRunning;
        key_result = '3';

        for (int i = 0; i < 2; ++i) {
            glm::mat4 transformationMatrix1 = models[i].initialRotation;  // 초기 회전 적용
            transformationMatrix1 = glm::scale(transformationMatrix1, glm::vec3(0.2f, 0.2f, 0.2f));  // 스케일
            transformationMatrix1 = glm::translate(transformationMatrix1, models[i].translationOffset);
            models[i].modelMatrix = transformationMatrix1;
        }

        f_f1 = models[0].modelMatrix[3];
        f_f2 = models[1].modelMatrix[3];

        glutTimerFunc(0, timer, 0);
        break;
    default:
        break;
    }
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
    glutCreateWindow("template");  // 창 생성 및 제목 설정

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
    Model modelSqu, modelSphere;

    // 정육면체
    read_obj_file("box3.obj", modelSqu, "cube");
    modelSqu.initialRotation = glm::mat4(1.0f);  // 초기 회전 행렬을 단위 행렬로 설정
    modelSqu.initialRotation = glm::rotate(modelSqu.initialRotation, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0)); // X축 회전
    modelSqu.initialRotation = glm::rotate(modelSqu.initialRotation, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0)); // Y축 회전
    modelSqu.modelMatrix = modelSqu.initialRotation;
    modelSqu.modelMatrix = glm::scale(modelSqu.modelMatrix, glm::vec3(0.2, 0.2, 0.2));
    modelSqu.translationOffset = glm::vec3(-3.0f, 0.0f, 0.0f);
    modelSqu.modelMatrix = glm::translate(modelSqu.modelMatrix, modelSqu.translationOffset);
    modelSqu.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelSqu);

    read_obj_file("sphere.obj", modelSphere, "sphere");
    modelSphere.initialRotation = glm::mat4(1.0f);  // 초기 회전 행렬을 단위 행렬로 설정
    modelSphere.initialRotation = glm::rotate(modelSphere.initialRotation, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0)); // X축 회전
    modelSphere.initialRotation = glm::rotate(modelSphere.initialRotation, glm::radians(-35.0f), glm::vec3(0.0, 1.0, 0.0)); // Y축 회전
    modelSphere.modelMatrix = modelSphere.initialRotation;
    modelSphere.modelMatrix = glm::scale(modelSphere.modelMatrix, glm::vec3(0.2, 0.2, 0.2));
    modelSphere.translationOffset = glm::vec3(3.0f, 0.0f, 0.0f);
    modelSphere.modelMatrix = glm::translate(modelSphere.modelMatrix, modelSphere.translationOffset);
    modelSphere.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelSphere);

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

        if (models[i].name == "cube" || models[i].name == "sphere") {
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
        else if (models[i].name == "spiral" && key_result == '1') {
            GLint lineLoc = glGetUniformLocation(shaderProgramID, "Line");
            glUniformMatrix4fv(lineLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            glUniform1i(modelStatus, 1);
            glLineWidth(1.0f);
            glDrawArrays(GL_LINE_STRIP, 0, models[i].vertices.size());
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
