#include <iostream>   // 표준 입출력 라이브러리 포함
#include <gl/glew.h>  // GLEW 라이브러리 포함 (OpenGL 확장 기능을 사용하기 위해 필요)
#include <gl/freeglut.h>  // GLUT 라이브러리 포함 (윈도우 창 및 사용자 입력 처리를 위해 사용)
#include <gl/freeglut_ext.h>  // GLUT 확장 기능 포함
#include <gl/glm/glm/glm.hpp>  // GLM 라이브러리 포함 (수학적인 계산을 돕기 위한 라이브러리)
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector>  // 표준 벡터 라이브러리 포함 (동적 배열을 사용하기 위해 필요)

#include "LoadObj.h"
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

// camera
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 4.0);
glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

const GLfloat x[2][3] = {
    {-0.8f,0.0f, 0.0f},
    {0.8f,0.0f, 0.0f}
};

const GLfloat color_x[2][3] = {
    {1.0,0.0,0.0},
    {1.0,0.0,0.0}
};

const GLfloat y[2][3] = {
    {0.0f,-0.8f, 0.0f},
    {0.0f, 0.8f, 0.0f}
};

const GLfloat color_y[2][3] = {
    {0.0,1.0,0.0},
    {0.0,1.0,0.0}
};

const GLfloat z[2][3] = {
    {0.0f, 0.0f, -0.8f},
    {0.0f, 0.0f, 0.8f}
};

const GLfloat color_z[2][3] = {
    {0.0,0.0,1.0},
    {0.0,0.0,1.0}
};

GLuint xyz_VBO[6], xyz_VAO[3];

// 화면 크기가 변경될 때 호출되는 함수
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);  // 뷰포트 크기 설정
    width = w;
    height = h;
}

void timer(int value) {
    glutPostRedisplay();  // 화면 다시 그리기 요청
    glutTimerFunc(16, timer, 0);
}

void keyBoard(unsigned char key, int x, int y) {
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
    Model modelBoard, modelBottomBox;

    // 바닥
    read_obj_file("obj/board.obj", modelBoard, "board");
    // 초기 회전 상태를 저장 (초기 회전 상태는 고정됨)
    modelBoard.initialRotation = glm::mat4(1.0f);  // 초기 회전 행렬을 단위 행렬로 설정
    modelBoard.modelMatrix = modelBoard.initialRotation;
    modelBoard.modelMatrix = glm::translate(modelBoard.modelMatrix, glm::vec3(0.0, 0.0, 0.0));
    modelBoard.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelBoard);

    read_obj_file("obj/box_bottom.obj", modelBottomBox, "bottom_b");
    modelBottomBox.initialRotation = glm::mat4(1.0f);
    modelBottomBox.modelMatrix = modelBottomBox.initialRotation;
    modelBottomBox.modelMatrix = glm::translate(modelBottomBox.modelMatrix, glm::vec3(0.0, 0.0, 0.0));
    modelBottomBox.modelMatrix = glm::scale(modelBottomBox.modelMatrix, glm::vec3(0.1, 0.1, 0.1));
    modelBottomBox.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelBottomBox);

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
    cameraPos = glm::vec3(0.0, 0.0, 6.0);
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
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    glEnable(GL_DEPTH_TEST);

    GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    // 각 모델을 그리기
    for (size_t i = 0; i < models.size(); ++i) {
        glBindVertexArray(vaos[i]);

        // 각 모델의 색상 정보 전달
        GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
        glUniform3fv(KdLoc, 1, glm::value_ptr(models[i].material.Kd));

        if (models[i].name == "board" || models[i].name == "bottom_b") {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            // 면 그리기
            glUniform1i(modelStatus, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
        }
       
        glBindVertexArray(0);
    }
    
    for (int i = 0; i < 3; ++i) {
        glBindVertexArray(xyz_VAO[i]); 
        glUniform1i(modelStatus, 0);
        glDrawArrays(GL_LINES, 0, 2);
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

    glGenVertexArrays(1, &xyz_VAO[0]);
    glBindVertexArray(xyz_VAO[0]);
    glGenBuffers(1, &xyz_VBO[0]);  
    glBindBuffer(GL_ARRAY_BUFFER, xyz_VBO[0]); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(x), x, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); 
    glEnableVertexAttribArray(0); 

    glGenBuffers(1, &xyz_VBO[1]);  
    glBindBuffer(GL_ARRAY_BUFFER, xyz_VBO[1]); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_x), color_x, GL_STATIC_DRAW); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); 
    glEnableVertexAttribArray(1); 

    glGenVertexArrays(1, &xyz_VAO[1]);
    glBindVertexArray(xyz_VAO[1]);
    glGenBuffers(1, &xyz_VBO[2]);  
    glBindBuffer(GL_ARRAY_BUFFER, xyz_VBO[2]);  
    glBufferData(GL_ARRAY_BUFFER, sizeof(y), y, GL_STATIC_DRAW); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); 
    glEnableVertexAttribArray(0);  

    glGenBuffers(1, &xyz_VBO[3]); 
    glBindBuffer(GL_ARRAY_BUFFER, xyz_VBO[3]); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_y), color_y, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glGenVertexArrays(1, &xyz_VAO[2]);
    glBindVertexArray(xyz_VAO[2]);
    glGenBuffers(1, &xyz_VBO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, xyz_VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(z), z, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &xyz_VBO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, xyz_VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_z), color_z, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

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
        //glBindBuffer(GL_ARRAY_BUFFER, vbos[i][1]);
        //glBufferData(GL_ARRAY_BUFFER, models[i].colors.size() * sizeof(glm::vec3), models[i].colors.data(), GL_STATIC_DRAW);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 색상 할당
        //glEnableVertexAttribArray(1);

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
