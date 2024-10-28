#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <Windows.h>
#include <time.h>
#include <vector>

#include "ReadObj.h"

#define WIDTH 800
#define HEIGHT 600

using namespace std;

GLvoid DrawScene();
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);

void make_vertexShaders();
void make_fragmentShaders();
void InitBuffer();
GLuint make_shaderProgram();

char* filetobuf(const char* file)
{
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb"); // Open file for reading 
    if (!fptr) // Return NULL on failure 
        return NULL;
    fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
    length = ftell(fptr); // Find out how many bytes into the file we are 
    buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
    fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
    fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
    fclose(fptr); // Close the file 
    buf[length] = 0; // Null terminator 
    return buf; // Return the buffer 
}

GLchar* vertexSource, * fragmentSource;   // 쉐이더 소스 코드를 저장할 변수들
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

vector<Model> models;

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


GLuint VAO_x, VBO_x[2];
GLuint VAO_y, VBO_y[2];
vector<GLuint > m_vao;
vector<vector<GLuint>> m_vbo;

const glm::vec3 X[2]{ glm::vec3(-1.0f,0.0f,0.5f),glm::vec3(1.0f,0.0f,0.5f) };      //z축 값을 0.0으로 둬도 되지만 좀 더 상대적 깊이를 주고 싶으면 0.5가 낫다. 숫자가 커질수록 깊이감이 깊어진다
const glm::vec3 Y[2]{ glm::vec3(0.0f,-1.0f,0.5f),glm::vec3(0.0f,1.0f,0.5f) };      //z축 값을 0.0으로 둬도 되지만 좀 더 상대적 깊이를 주고 싶으면 0.5가 낫다. 숫자가 커질수록 깊이감이 깊어진다

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
    //--- 윈도우 생성하기
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("실습 13");
    //--- GLEW 초기화하기

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";
    Model model_Sq, model_Tr;
    read_obj_file("box3.obj", model_Sq, "cube");         //육면체
    read_obj_file("tetrahedron.obj", model_Sq, "tetrahedron");      //사면체

    models.push_back(model_Sq);
    models.push_back(model_Tr);

    //--- 세이더 읽어와서 세이더 프로그램 만들기
    make_vertexShaders(); //--- 버텍스 세이더 만들기
    make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
    shaderProgramID = make_shaderProgram();
    InitBuffer();
    //--- 세이더 프로그램 만들기
    glutKeyboardFunc(Keyboard);
    glutDisplayFunc(DrawScene); //--- 출력 콜백 함수
    glutReshapeFunc(Reshape);
    glutMainLoop();
}

GLvoid Keyboard(unsigned char key, int x, int y) {

    switch (key) {
        //육면체의 각 면 그리기
    case '1':
        break;
    case '2':
        break;
    case '3':
        break;
    case '4':
        break;
    case '5':
        break;
    case '6':
        break;
        //사면체의 각 면 그리기
    case '7':
        break;
    case '8':
        break;
    case '9':
        break;
    case '0':
        break;
    case 'c':
    case 'C':
        break;
    case 't':
    case 'T':
        break;
    }
    glutPostRedisplay();
}

void DrawScene() //--- glutDisplayFunc()함수로 등록한 그리기 콜백 함수
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    glBindVertexArray(VAO_x);
    glDrawArrays(GL_LINES, 0, 2);   //x축

    glBindVertexArray(VAO_y);
    glDrawArrays(GL_LINES, 0, 2);   //y축

    //glm::mat4 transForm = glm::mat4(1.0f);   // 단위행렬(대각선이 모두 1이고 나머진 0인 행렬) 생성
    //transForm = glm::rotate(transForm, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));   // x로 10도 회전
    //transForm = glm::rotate(transForm, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));   // y로 10도 회전

    //GLuint transformLoc = glGetUniformLocation(shaderProgramID, "transform");
    //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transForm)); // 변환 행렬을 셰이더에 전달
    ////축
    //glDrawArrays(GL_LINES, 0, 4);

    //glDrawArrays(GL_QUADS, 4, 24); //사각형


    glutSwapBuffers();
}

void InitBuffer()
{
    //x축
    //glGenVertexArrays(1, &VAO_x); //--- VAO 를 지정하고 할당하기
    //glBindVertexArray(VAO_x); //--- VAO를 바인드하기
    //glGenBuffers(2, &VBO_x); //--- 1개의 VBO를 지정하고 할당하기   //점만 사용할 거라

    //glBindBuffer(GL_ARRAY_BUFFER, VBO_x);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(X), X, GL_STATIC_DRAW);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(0);


    ////y축
    //glGenVertexArrays(1, &VAO_y); //--- VAO 를 지정하고 할당하기
    //glBindVertexArray(VAO_y); //--- VAO를 바인드하기
    //glGenBuffers(1, &VBO_y); //--- 1개의 VBO를 지정하고 할당하기

    //glBindBuffer(GL_ARRAY_BUFFER, VBO_y);
    //glBufferData(GL_ARRAY_BUFFER,  sizeof(Y), Y, GL_STATIC_DRAW);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(0);
    //x축
    glGenVertexArrays(1, &VAO_x); //--- VAO 를 지정하고 할당하기
    glBindVertexArray(VAO_x); //--- VAO를 바인드하기
    glGenBuffers(1,&VBO_x[0]); //--- 1개의 VBO를 지정하고 할당하기   //점만 사용할 거라

    glBindBuffer(GL_ARRAY_BUFFER, VBO_x[0]);
    glBufferData(GL_ARRAY_BUFFER, 1000, X, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_x[1]);
    glBufferData(GL_ARRAY_BUFFER, 1000, colors2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    //y축
    glGenVertexArrays(1, &VAO_y); //--- VAO 를 지정하고 할당하기
    glBindVertexArray(VAO_y); //--- VAO를 바인드하기
    glGenBuffers(2, &VBO_y[0]); //--- 1개의 VBO를 지정하고 할당하기

    glBindBuffer(GL_ARRAY_BUFFER, VBO_y[0]);
    glBufferData(GL_ARRAY_BUFFER, 1000, Y, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_y[1]);
    glBufferData(GL_ARRAY_BUFFER, 1000, colors2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    //m_vao.resize(models.size());
    //m_vbo.resize(models.size(), vector<GLuint>(2));
    //for (int i = 0; i < models.size(); ++i) {

    //   glGenVertexArrays(1, &m_vao[i]); //--- VAO 를 지정하고 할당하기
    //   glBindVertexArray(m_vao[i]); //--- VAO를 바인드하기

    //   glGenBuffers(2, m_vbo[i].data()); //--- 2개의 VBO를 지정하고 할당하기

    //   //--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
    //   glBindBuffer(GL_ARRAY_BUFFER, m_vbo[i][0]);
    //   glBufferData(GL_ARRAY_BUFFER, models[i].vertices.size()*sizeof(glm::vec3), models[i].vertices.data(), GL_STATIC_DRAW);
    //   //--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
    //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //   //--- attribute 인덱스 0번을 사용가능하게 함
    //   glEnableVertexAttribArray(0);

    //   //--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
    //   glBindBuffer(GL_ARRAY_BUFFER, m_vbo[i][1]);
    //   //--- 변수 colors에서 버텍스 색상을 복사한다.
    //   glBufferData(GL_ARRAY_BUFFER,sizeof(colors), colors, GL_STATIC_DRAW);
    //   //--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
    //   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //   //--- attribute 인덱스 1번을 사용 가능하게 함.
    //   glEnableVertexAttribArray(1);
    //}

}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
    glViewport(0, 0, w, h);
}

void make_vertexShaders()
{
    //--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
    vertexSource = filetobuf("vertexShader.glsl"); //--- 버텍스세이더 읽어오기
    //--- 버텍스 세이더 읽어 저장하고 컴파일 하기

    //vertex shader 생성 해주고 생성한 데이터를 vertexShader라는 변수에 저장
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    //세이더 객체, 1, 세이더 소스 코드, NULL
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    //shader, 문자열들의 갯수, 문자열들의 배열, 문자열들의 길이를 저장할 int배열

    //컴파일 함
    glCompileShader(vertexShader);

    //세이더 결과 어떻게 됐는지? 상태 확인하는 코드
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)   //result == 0이면 오류가 있다.
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        cerr << "ERROR: vertex shader error\n" << errorLog << endl;
        return;
    }
}

void make_fragmentShaders()
{
    fragmentSource = filetobuf("fragmentShader.glsl"); //--- 프래그먼트세이더 읽어오기
    //--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        cerr << "ERROR: fragment shader error\n" << errorLog << endl;
        return;
    }
}

GLuint make_shaderProgram()      //--- 세이더 프로그램 만들고 세이더 객체 링크하기
{
    GLuint shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기

    glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
    glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
    glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기

    glDeleteShader(vertexShader); //--- 세이더 프로그램에 링크하여 세이더 객체 자체는 삭제 가능
    glDeleteShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];

    glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        cerr << "ERROR: shader program 연결 실패\n" << errorLog << endl;
        return false;
    }
    glUseProgram(shaderID);
    return shaderID;
}