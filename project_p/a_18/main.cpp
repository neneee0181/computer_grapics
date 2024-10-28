#include <iostream>   // 표준 입출력 라이브러리 포함
#include <gl/glew.h>  // GLEW 라이브러리 포함 (OpenGL 확장 기능을 사용하기 위해 필요)
#include <gl/freeglut.h>  // GLUT 라이브러리 포함 (윈도우 창 및 사용자 입력 처리를 위해 사용)
#include <gl/freeglut_ext.h>  // GLUT 확장 기능 포함
#include <gl/glm/glm/glm.hpp>  // GLM 라이브러리 포함 (수학적인 계산을 돕기 위한 라이브러리)
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector>  // 표준 벡터 라이브러리 포함 (동적 배열을 사용하기 위해 필요)
#include <random>
#include <unordered_map>

#include "LoadObj.h"
#include "shaderMaker.h"

using namespace std;  // 네임스페이스 std 사용으로 코드 내에서 std:: 생략 가능

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis_color(0.0f, 1.0f);

// 함수 선언부
void InitBuffer();  // 버퍼 초기화 함수 선언
GLvoid drawScene(GLvoid);  // 화면을 그리는 함수 선언
GLvoid Reshape(int w, int h);  // 화면 크기가 변경될 때 호출되는 함수 선언

// 여러 3D 모델을 관리하는 벡터
vector<Model> models;
vector<GLuint> vaos;
vector<vector<GLuint>> vbos;
GLuint orbitVAO[6], orbitVBO[6];

// camera
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 4.0);
glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

glm::mat4 projection = glm::mat4(1.0f);

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
    width = w;
    height = h;
}

std::vector<std::vector<glm::vec3>> orbitVertices;
int orbitSegments = 100; // 궤도 선을 위한 세그먼트 수

void createOrbitVertices(float radius, glm::vec3 center, float angleY) {
    std::vector<glm::vec3> singleOrbit;

    // Y축을 기준으로 angleY만큼 회전하는 행렬을 생성
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 0.0f, 1.0f));

    for (int i = 0; i <= orbitSegments; ++i) {
        float angle = glm::radians((360.0f / orbitSegments) * i);

        // XZ 평면에 원을 생성
        glm::vec4 point = glm::vec4(radius * cos(angle), 0.0f, radius * sin(angle), 1.0f);

        // 회전 행렬 적용
        point = rotationMatrix * point;

        // 중심점 이동 적용
        singleOrbit.push_back(glm::vec3(point) + center);
    }

    orbitVertices.push_back(singleOrbit);
}

unordered_map<unsigned char, bool> keyStates;
char key_result = ' ';
int y_status = 0;
int z_status = 0;
float speed1 = 0.8f;
float speed2 = 1.0f;
float speed3 = 1.2f;
float speed4 = 1.4f;
float speed5 = 1.6f;

void timer_z(int value) {
    if (keyStates['z']) {
        for (int i = 0; i < models.size(); ++i) {
            if (i == 0)
                continue;
            glm::mat4 orbit = glm::mat4(1.0f);
            orbit = glm::rotate(orbit, glm::radians(speed5), glm::vec3(0.0, 1.0, 0.0));
            models[i].modelMatrix = models[i].modelMatrix * orbit;

            
            glm::mat4 lineO = glm::mat4(1.0f);
            lineO = glm::translate(lineO, glm::vec3(models[i].modelMatrix[3]));
            lineO = glm::rotate(lineO, glm::radians(speed5), glm::vec3(0.0, 0.0, 1.0));
            lineO = glm::translate(lineO, glm::vec3(-models[i].modelMatrix[3]));
            if (i == 2 || i == 4 || i == 6) {
                models[i].modelMatrix = lineO * models[i].modelMatrix;
            }
            if (i == 1 || i == 3 || i == 5) {
                for (size_t j = 0; j < orbitVertices[i].size(); ++j) {
                    glm::vec4 rotatedPoint = lineO * glm::vec4(orbitVertices[i][j], 1.0f);
                    orbitVertices[i][j] = glm::vec3(rotatedPoint);
                }
            }
        }

    }
    else {

    }


    glutPostRedisplay();  // 화면 다시 그리기 요청
    if (z_status == 1) {
        glutTimerFunc(16, timer_z, z_status);
    }
}

void timer_y(int value) {

    //float speed = 0.8f;

    if (keyStates['y']) {
        for (int i = 0; i < models.size(); ++i) {
            switch (i)
            {
            case 1:
            {

                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                for (size_t j = 0; j < orbitVertices[i].size(); ++j) {

                    // vec3를 vec4로 변환하여 회전 적용
                    glm::vec4 rotatedPoint = orbit * glm::vec4(orbitVertices[i][j], 1.0f);

                    // 결과를 다시 vec3로 변환하여 저장
                    orbitVertices[i][j] = glm::vec3(rotatedPoint);
                }

                break;
            }
            case 2:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                orbit = glm::translate(orbit, glm::vec3(models[1].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;
                break;
            }
            case 3:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed3), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(speed3), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                break;
            }
            case 4:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed3), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(speed3), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                orbit = glm::translate(orbit, glm::vec3(models[3].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed3), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(-speed3), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[3].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                for (size_t j = 0; j < orbitVertices[3].size(); ++j) {
                    glm::vec4 rotatedPoint = orbit * glm::vec4(orbitVertices[3][j], 1.0f);
                    orbitVertices[3][j] = glm::vec3(rotatedPoint);
                }
                break;
            }
            case 5:
            {
                glm::mat4 orbit = glm::mat4(1.0f);

                // 공전 변환 적용 (회전)
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                break;
            }
            case 6:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                orbit = glm::translate(orbit, glm::vec3(models[5].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed1 + 0.08f), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(speed1 + 0.08f), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[5].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                for (size_t j = 0; j < orbitVertices[5].size(); ++j) {
                    glm::vec4 rotatedPoint = orbit * glm::vec4(orbitVertices[5][j], 1.0f);
                    orbitVertices[5][j] = glm::vec3(rotatedPoint);
                }
                break;
            }
            default:
                break;
            }
        }
    }
    else {
        for (int i = 0; i < models.size(); ++i) {
            switch (i)
            {
            case 1:
            {

                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                for (size_t j = 0; j < orbitVertices[i].size(); ++j) {

                    // vec3를 vec4로 변환하여 회전 적용
                    glm::vec4 rotatedPoint = orbit * glm::vec4(orbitVertices[i][j], 1.0f);

                    // 결과를 다시 vec3로 변환하여 저장
                    orbitVertices[i][j] = glm::vec3(rotatedPoint);
                }

                break;
            }
            case 2:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                orbit = glm::translate(orbit, glm::vec3(models[1].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;
                break;
            }
            case 3:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed3), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(-speed3), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                break;
            }
            case 4:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed3), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(-speed3), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                orbit = glm::translate(orbit, glm::vec3(models[3].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed3), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(speed3), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[3].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                for (size_t j = 0; j < orbitVertices[3].size(); ++j) {
                    glm::vec4 rotatedPoint = orbit * glm::vec4(orbitVertices[3][j], 1.0f);
                    orbitVertices[3][j] = glm::vec3(rotatedPoint);
                }
                break;
            }
            case 5:
            {
                glm::mat4 orbit = glm::mat4(1.0f);

                // 공전 변환 적용 (회전)
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                break;
            }
            case 6:
            {
                glm::mat4 orbit = glm::mat4(1.0f);
                orbit = glm::translate(orbit, glm::vec3(models[0].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(speed1), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(-speed1), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[0].modelMatrix[3]));
                orbit = glm::translate(orbit, glm::vec3(models[5].modelMatrix[3]));
                orbit = glm::rotate(orbit, glm::radians(-speed1 + 0.08f), glm::vec3(0.0, 1.0, 0.0));
                orbit = glm::rotate(orbit, glm::radians(-speed1 + 0.08f), glm::vec3(1.0, 0.0, 0.0));
                orbit = glm::translate(orbit, glm::vec3(-models[5].modelMatrix[3]));
                models[i].modelMatrix = orbit * models[i].modelMatrix;

                for (size_t j = 0; j < orbitVertices[5].size(); ++j) {
                    glm::vec4 rotatedPoint = orbit * glm::vec4(orbitVertices[5][j], 1.0f);
                    orbitVertices[5][j] = glm::vec3(rotatedPoint);
                }
                break;
            }
            default:
                break;
            }
        }
    }

    // VBO 업데이트
    for (int i = 0; i < 6; ++i) {
        glBindBuffer(GL_ARRAY_BUFFER, orbitVBO[i]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, orbitVertices[i].size() * sizeof(glm::vec3), orbitVertices[i].data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glutPostRedisplay();  // 화면 다시 그리기 요청
    if (y_status == 1) {
        glutTimerFunc(16, timer_y, y_status);
    }
}

void keyBoard(unsigned char key, int x, int y) {

    keyStates[key] = !keyStates[key];

    if (key == 'm')
        keyStates['M'] = false;
    else if (key == 'M')
        keyStates['m'] = false;

    if (key == 'y') {
        if (y_status == 0) {
            glutTimerFunc(0, timer_y, y_status);
            y_status = 1;
        }
    }
    else if (key == 'Y') {
        if (y_status == 0) {
            glutTimerFunc(0, timer_y, y_status);
            y_status = 1;
        }
    }

    if (key == 'z') {
        if (z_status == 0) {
            glutTimerFunc(0, timer_z, z_status);
            z_status = 1;
        }
    }
    else if (key == 'z') {
        if (z_status == 0) {
            glutTimerFunc(0, timer_z, z_status);
            z_status = 1;
        }
    }

    if (key == 'p') {
        projection = glm::mat4(1.0f);
        projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -100.0f, 100.0f);
    }
    else if (key == 'P') {
        projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 50.0f);
        projection = glm::translate(projection, glm::vec3(0.0, 0.0, -0.5));
    }

    // ----------
    float speed = 0.1f;
    glm::mat4 orbitTransform = glm::mat4(1.0f);

    switch (key)
    {
    case 'w':
    {
        orbitTransform = glm::translate(orbitTransform, glm::vec3(speed, 0.0, 0.0));
        break;
    }
    case 'a':
    {
        orbitTransform = glm::translate(orbitTransform, glm::vec3(0.0, -speed, 0.0));
        break;
    }
    case 's':
    {
        orbitTransform = glm::translate(orbitTransform, glm::vec3(-speed, 0.0, 0.0));
        break;
    }
    case 'd':
    {
        orbitTransform = glm::translate(orbitTransform, glm::vec3(0.0, speed, 0.0));
        break;
    }
    case '+':
    {
        orbitTransform = glm::translate(orbitTransform, glm::vec3(0.0, 0.0, speed));
        break;
    }
    case '-':
    {
        orbitTransform = glm::translate(orbitTransform, glm::vec3(0.0, 0.0, -speed));
        break;
    }
    default:
        break;
    }

    for (auto& model : models) {
        model.modelMatrix = orbitTransform * model.modelMatrix;
    }

   for (int i = 0; i < orbitVertices.size(); ++i) {
        for (int j = 0; j < orbitVertices[i].size(); ++j) {

            glm::vec4 rotatedPoint = orbitTransform * glm::vec4(orbitVertices[i][j], 1.0f);
            orbitVertices[i][j] = glm::vec3(rotatedPoint);

        }
        // 변경된 orbitVertices 데이터를 VBO에 업로드
        glBindBuffer(GL_ARRAY_BUFFER, orbitVBO[i]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, orbitVertices[i].size() * sizeof(glm::vec3), orbitVertices[i].data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }



    glutPostRedisplay();  // 화면 다시 그리기 요청
}

// 마우스 입력을 처리하는 함수
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        std::cout << "x = " << x << " y = " << y << std::endl;  // 마우스 클릭 좌표 출력
    glutPostRedisplay();  // 키보드 입력 후 화면을 다시 그리도록 요청
}

void rotation_xyz(Model& model, float radians, glm::vec3 xyz) {
    model.modelMatrix = glm::rotate(model.modelMatrix, glm::radians(radians), xyz);
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
    glutCreateWindow("18번");  // 창 생성 및 제목 설정

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
    Model modelSphere;

    // 정육면체
    read_obj_file("sphere.obj", modelSphere, "sphere");
    // 초기 회전 상태를 저장 (초기 회전 상태는 고정됨)

    for (int i = 0; i < 7; ++i) {
        Model m = modelSphere;
        m.modelMatrix = glm::mat4(1.0f);
        switch (i)
        {
        case 0:
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.4, 0.4, 0.4));
            break;
        case 1:
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(2.0f, 0.0f, 0.0f));
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.2, 0.2, 0.2));
            m.modelMatrix = glm::rotate(m.modelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            break;
        case 2:
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(1.5f, 0.0f, 0.0f));
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.1, 0.1, 0.1));
            break;
        case 3:
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(1.5f, -1.5f, 0.0f));
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.2, 0.2, 0.2));
            m.modelMatrix = glm::rotate(m.modelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            break;
        case 4:
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(1.5f, -1.5f, 0.0f));
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(0.5f, 0, 0.0f));
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.1, 0.1, 0.1));
            m.modelMatrix = glm::rotate(m.modelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            break;
        case 5:
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(-1.5f, -1.5f, 0.0f));
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.2, 0.2, 0.2));
            m.modelMatrix = glm::rotate(m.modelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            break;
        case 6:
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(-1.5f, -1.5f, 0.0f));
            m.modelMatrix = glm::translate(m.modelMatrix, glm::vec3(-0.5f, 0, 0.0f));
            m.modelMatrix = glm::scale(m.modelMatrix, glm::vec3(0.1, 0.1, 0.1));
            m.modelMatrix = glm::rotate(m.modelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            break;
        default:
            break;
        }

        glm::vec3 s_color = { dis_color(gen), dis_color(gen), dis_color(gen) };

        for (int i = 0; i < modelSphere.vertices.size(); ++i) {
            m.colors.push_back(s_color);
        }

        models.push_back(m);
    }

    Model modelXLine, modelYLine, modelZLine;
    modelXLine.name = "xLine";
    modelXLine.vertices.push_back(x[0]);
    modelXLine.vertices.push_back(x[1]);
    modelXLine.colors.push_back(glm::vec3(1.0, 0.0, 0.0));

    modelYLine.name = "yLine";
    modelYLine.vertices.push_back(y[0]);
    modelYLine.vertices.push_back(y[1]);
    modelYLine.colors.push_back(glm::vec3(0.0, 1.0, 0.0));

    modelZLine.name = "zLine";
    modelZLine.vertices.push_back(z[0]);
    modelZLine.vertices.push_back(z[1]);
    modelZLine.colors.push_back(glm::vec3(0.0, 0.0, 1.0));

    models.push_back(modelXLine);
    models.push_back(modelYLine);
    models.push_back(modelZLine);

    createOrbitVertices(2.0, glm::vec3(0.0, 0.0, 0.0), 0.0f);
    createOrbitVertices(0.5, glm::vec3(models[1].modelMatrix[3]), 0.0f);
    createOrbitVertices(2.12f, glm::vec3(0.0, 0.0, 0.0), -45.0f);
    createOrbitVertices(0.5, glm::vec3(models[3].modelMatrix[3]), 0.0f);
    createOrbitVertices(2.12f, glm::vec3(0.0, 0.0, 0.0), 45.0f);
    createOrbitVertices(0.5, glm::vec3(models[5].modelMatrix[3]), 0.0f);

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

    // 카메라
    cameraPos = glm::vec3(0.0, 0.0, 5.5);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 rotationMatrix_x = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationMatrix_y = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cameraPos = glm::vec3(rotationMatrix_x * rotationMatrix_y * glm::vec4(cameraPos, 1.0f));
    view = glm::lookAt(
        cameraPos,  //--- 카메라위치
        cameraDirection,  //--- 카메라바라보는방향
        cameraUp   //--- 카메라위쪽방향
    );
    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    // 투영
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    glEnable(GL_DEPTH_TEST);
    GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");

    // 각 모델을 그리기
    for (size_t i = 0; i < models.size(); ++i) {
        glBindVertexArray(vaos[i]);

        if (models[i].name == "sphere") {
            GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            // 면 그리기
            glUniform1i(modelStatus, 0);
            glLineWidth(1.0f);
            if (keyStates['m'])
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            else if (keyStates['M'])
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);
    }


    //-------------------------------------------------
    for (int i = 0; i < orbitVertices.size(); ++i) {
        glBindVertexArray(orbitVAO[i]);

        // Line 행렬을 가져와서 현재 궤도 위치로 변환 설정
        GLint lineLoc = glGetUniformLocation(shaderProgramID, "model");

        // 모델 행렬 대신, 궤도의 중심을 Line 행렬로 전달합니다.
        glm::mat4 lineTransform = glm::mat4(1.0f);  // 필요한 경우 중심 이동이나 회전 적용 가능
        glUniformMatrix4fv(lineLoc, 1, GL_FALSE, glm::value_ptr(lineTransform));

        glUniform1i(modelStatus, 2);
        glLineWidth(1.5f);
        glDrawArrays(GL_LINE_STRIP, 0, orbitVertices[i].size());
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

    for (int i = 0; i < orbitVertices.size(); ++i) {
        glGenVertexArrays(1, &orbitVAO[i]);
        glGenBuffers(1, &orbitVBO[i]);

        glBindVertexArray(orbitVAO[i]);

        // glm::vec4 -> glm::vec3 변환
        std::vector<glm::vec3> vertexData;
        for (const auto& vec4Point : orbitVertices[i]) {
            vertexData.push_back(glm::vec3(vec4Point));  // vec4의 xyz 부분만 가져옴
        }

        glBindBuffer(GL_ARRAY_BUFFER, orbitVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(glm::vec3), vertexData.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // 위치 정보
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

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
