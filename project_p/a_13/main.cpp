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

GLuint vao, vbo[3];  // VAO와 VBO ID를 저장할 변수들
GLuint vao2, vbo2[3];  // VAO와 VBO ID를 저장할 변수들

GLuint xVBO, xVAO;
GLuint yVBO, yVAO;

const GLfloat colors[12][3] = {
    {1.0, 0.0, 0.0},   // 빨강
    {0.0, 1.0, 0.0},   // 초록
    {0.0, 0.0, 1.0},   // 파랑
    {1.0, 1.0, 0.0},   // 노랑
    {1.0, 0.0, 1.0},   // 마젠타
    {0.0, 1.0, 1.0},   // 시안
    {0.5, 0.5, 0.5},   // 회색
    {1.0, 0.5, 0.0},   // 주황
    {0.0, 0.5, 0.5},   // 청록
    {0.5, 0.0, 0.5},   // 보라
    {0.5, 1.0, 0.5},   // 연두
    {1.0, 0.5, 0.5}    // 핑크
};

const GLfloat colors2[4][3] = {
    {1.0, 0.0, 0.0},   // 빨강
    {0.0, 1.0, 0.0},   // 초록
    {0.0, 0.0, 1.0},   // 파랑
    {1.0, 1.0, 0.0},   // 노랑
};

const glm::vec3 x[2] = {
    glm::vec3(-1.0f,0.0f, 0.5f), glm::vec3(1.0f,0.0f,0.5f)
};

const glm::vec3 y[2] = {
      glm::vec3(0.0f,-1.0f,0.5f), glm::vec3(0.0f,1.0f,0.5f)
};

Model modelSqu;
Model modelTre;

// 키보드 입력을 처리하는 함수
void keyBoard(unsigned char key, int x, int y) {
    switch (key)
    {
    default:
        break;
    }
    glutPostRedisplay();  // 키보드 입력 후 화면을 다시 그리도록 요청
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
    glutCreateWindow("실습 13번");  // 창 생성 및 제목 설정

    // GLEW 초기화
    glewExperimental = GL_TRUE;  // GLEW 실험적 기능 활성화
    if (glewInit() != GLEW_OK) {  // GLEW 초기화 및 오류 체크
        cerr << "Unable to initialize GLEW" << endl;  // GLEW 초기화 실패 시 오류 메시지 출력
        exit(EXIT_FAILURE);  // 프로그램 종료
    }
    else
        cout << "GLEW Initialized\n";  // 초기화 성공 시 메시지 출력

    make_shaderProgram();  // 쉐이더 프로그램 생성
        
    read_obj_file("box3.obj", modelSqu); //obj 파일 가져오기
    read_obj_file("tetrahedron.obj", modelTre);

    //try {
    //    std::cout << "OBJ 파일 로딩 성공!" << std::endl;
    //    std::cout << "정점 개수: " << modelSqu.vertices.size() << std::endl;
    //    for (size_t i = 0; i < modelSqu.vertices.size(); ++i) {
    //        std::cout << "v[" << i << "] = ("
    //            << modelSqu.vertices[i].x << ", "
    //            << modelSqu.vertices[i].y << ", "
    //            << modelSqu.vertices[i].z << ")" << std::endl;
    //    }
    //    std::cout << "텍스처 좌표 개수: " << modelSqu.texCoords.size() << std::endl;
    //    std::cout << "법선 벡터 개수: " << modelSqu.normals.size() << std::endl;
    //    std::cout << "면 개수: " << modelSqu.faces.size() << std::endl;
    //    for (size_t i = 0; i < modelSqu.faces.size(); ++i) {
    //        const Face& face = modelSqu.faces[i];
    //        std::cout << "Face[" << i << "] = ("
    //            << face.v1 << ", " << face.v2 << ", " << face.v3 << ")"
    //            << std::endl;
    //    }
    //}
    //catch (const std::exception& e) {
    //    std::cerr << e.what() << std::endl;  // 오류 메시지 출력
    //}

    InitBuffer();  // 버퍼 초기화

    // 콜백 함수 등록
    glutDisplayFunc(drawScene);  // 화면을 그리는 함수 등록
    glutReshapeFunc(Reshape);  // 화면 크기 변경 시 호출되는 함수 등록
    glutKeyboardFunc(keyBoard);  // 키보드 입력 처리 함수 등록
    glutMouseFunc(mouse);  // 마우스 입력 처리 함수 등록
    glutMainLoop();  // 이벤트 루프 시작

    return 0;  // 프로그램 정상 종료
}
glm::mat4 modelTreMatrix = glm::mat4(1.0f);  // 단위 행렬로 초기화
glm::mat4 modelSquMatrix = glm::mat4(1.0f);  // 단위 행렬로 초기화
glm::mat4 xLineMatrix = glm::mat4(1.0f);
glm::mat4 yLineMatrix = glm::mat4(1.0f);

// 화면을 그리는 함수
GLvoid drawScene() {

    glClearColor(0.0, 0.0, 0.0, 1.0f);  // 화면을 흰색으로 초기화
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 컬러 버퍼와 깊이 버퍼 초기화

    glUseProgram(shaderProgramID);  // 쉐이더 프로그램 사용

    GLint isLineLoc = glGetUniformLocation(shaderProgramID, "isLine");

    // 2. 정육면체 그리기
    glBindVertexArray(vao);  // 정육면체 VAO 바인딩
    glEnable(GL_DEPTH_TEST);

    // X축 기준으로 10도 회전
    modelSquMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // Y축 기준으로 10도 회전
    modelSquMatrix = glm::rotate(modelSquMatrix, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    GLint modelLoc = glGetUniformLocation(shaderProgramID, "cube");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSquMatrix));  // 변환 행렬 전달

    // 유니폼 설정 (면 그리기)
    glUniform1i(isLineLoc, 0);  // 면 그릴 때 isLine = 0
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // 면을 그리는 모드
    glDrawElements(GL_TRIANGLES, modelSqu.faces.size() * 3, GL_UNSIGNED_INT, 0);  // 정육면체 면 그리기

    // 유니폼 설정 (선 그리기)
    glUniform1i(isLineLoc, 1);  // 선 그릴 때 isLine = 1
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // 외곽선 그리는 모드
    glLineWidth(1.0f);  // 선 두께 설정
    glDrawElements(GL_TRIANGLES, modelSqu.faces.size() * 3, GL_UNSIGNED_INT, 0);  // 정육면체 외곽선 그리기
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(0);  // VAO 바인딩 해제

    // 3. 정사면체
    glBindVertexArray(vao2); 
    glEnable(GL_DEPTH_TEST);
    modelTreMatrix = glm::mat4(1.0f);
    modelTreMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelTreMatrix = glm::rotate(modelTreMatrix, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelTreMatrix = glm::translate(modelTreMatrix, glm::vec3(0.5f, 0.5f, 0.0f));

    GLint modelLoc2 = glGetUniformLocation(shaderProgramID, "tre");
    glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(modelTreMatrix));  // 변환 행렬 전달

    // 유니폼 설정 (면 그리기)
    glUniform1i(isLineLoc, 4);  // 면 그릴 때 isLine = 0
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // 면을 그리는 모드
    glDrawElements(GL_TRIANGLES, modelTre.faces.size() * 3, GL_UNSIGNED_INT, 0);  // 정육면체 면 그리기

    // 유니폼 설정 (선 그리기)
    //glUniform1i(isLineLoc, 1);  // 선 그릴 때 isLine = 1
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // 외곽선 그리는 모드
    glLineWidth(1.0f);  // 선 두께 설정
    glDrawElements(GL_TRIANGLES, modelTre.faces.size() * 3, GL_UNSIGNED_INT, 0);  // 정육면체 외곽선 그리기
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(0);  // VAO 바인딩 해제

    // 1. X축 그리기
    //glDisable(GL_DEPTH_TEST);
    glBindVertexArray(xVAO);  // X축 VAO 바인딩
    glEnable(GL_DEPTH_TEST);
    GLint xLineLoc = glGetUniformLocation(shaderProgramID, "xLine");
    glUniformMatrix4fv(xLineLoc, 1, GL_FALSE, glm::value_ptr(xLineMatrix));  // 초기화된 변환 행렬 전달
    glUniform1i(isLineLoc, 2);  // isLine 값을 2로 설정 (선 그리기 용)
    glLineWidth(2.0f);  // 선 두께 설정
    glDrawArrays(GL_LINES, 0, 2);  // X축 그리기
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(0);  // VAO 바인딩 해제
    //glEnable(GL_DEPTH_TEST);

    glBindVertexArray(yVAO);  // X축 VAO 바인딩
    glEnable(GL_DEPTH_TEST);
    GLint yLineLoc = glGetUniformLocation(shaderProgramID, "yLine");
    glUniformMatrix4fv(yLineLoc, 1, GL_FALSE, glm::value_ptr(yLineMatrix));  // 초기화된 변환 행렬 전달
    glUniform1i(isLineLoc, 3);  // isLine 값을 2로 설정 (선 그리기 용)
    glLineWidth(2.0f);  // 선 두께 설정
    glDrawArrays(GL_LINES, 0, 2);  // X축 그리기
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(0);  // VAO 바인딩 해제

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


    // X축 좌표계 선을 위한 VAO와 VBO 설정
    glGenVertexArrays(1, &xVAO);  // X축 VAO 생성
    glBindVertexArray(xVAO);  // VAO 바인딩
    glGenBuffers(1, &xVBO);  // VBO 생성
    glBindBuffer(GL_ARRAY_BUFFER, xVBO);  // 버퍼 바인딩
    glBufferData(GL_ARRAY_BUFFER, sizeof(x), x, GL_STATIC_DRAW);  // X축 좌표 데이터 설정
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // 정점 속성 포인터 설정
    glEnableVertexAttribArray(0);  // 정점 속성 사용

    glGenVertexArrays(1, &yVAO); 
    glBindVertexArray(yVAO);  // VAO 바인딩
    glGenBuffers(1, &yVBO);  // VBO 생성
    glBindBuffer(GL_ARRAY_BUFFER, yVBO);  // 버퍼 바인딩
    glBufferData(GL_ARRAY_BUFFER, sizeof(y), y, GL_STATIC_DRAW);  // X축 좌표 데이터 설정
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // 정점 속성 포인터 설정
    glEnableVertexAttribArray(0);  // 정점 속성 사용


    // -----------------------------------------------------------------------------------------------------
    glGenVertexArrays(1, &vao2);  // VAO 생성
    glBindVertexArray(vao2);  // VAO 바인딩

    glGenBuffers(2, vbo2);  // VBO 생성 (2개)
    // VBO 생성 및 데이터 설정 (꼭짓점 좌표)
    glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
    glBufferData(GL_ARRAY_BUFFER, modelTre.vertices.size() * sizeof(glm::vec3), modelTre.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // 색상 버퍼 설정
    glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // 면 인덱스 데이터 (EBO) 설정
    std::vector<unsigned int> indices2;
    for (const Face& face : modelTre.faces) {
        indices2.push_back(face.v1);
        indices2.push_back(face.v2);
        indices2.push_back(face.v3);
    }

    GLuint ebo2;
    glGenBuffers(1, &ebo2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(unsigned int), indices2.data(), GL_STATIC_DRAW);


    // -----------------------------------------------------------------------------------------------------
    glGenVertexArrays(1, &vao);  // VAO 생성
    glBindVertexArray(vao);  // VAO 바인딩

    glGenBuffers(2, vbo);  // VBO 생성 (2개)
    // VBO 생성 및 데이터 설정 (꼭짓점 좌표)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, modelSqu.vertices.size() * sizeof(glm::vec3), modelSqu.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // 색상 버퍼 설정
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // 면 인덱스 데이터 (EBO) 설정
    std::vector<unsigned int> indices;
    for (const Face& face : modelSqu.faces) {
        indices.push_back(face.v1);
        indices.push_back(face.v2);
        indices.push_back(face.v3);
    }

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


}