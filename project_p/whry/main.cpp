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

GLvoid drawScene();
GLvoid KeyBoard(unsigned char key, int x, int y);
GLvoid SpecialKeyBoard(int key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid TimerFunc(int x);

void InitBuffer();
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();

//glsl파일을 읽어들이는 함수
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
//초기 상자의 좌표
float vertexPosition[] = {
    //앞면
    -1.0f,2.0f,1.0f,
  -1.0f,0.0f,1.0f,
   1.0f,0.0f,1.0f,
   1.0f,2.0f,1.0f,
   //윗면
   -1.0f,2.0f,-1.0f,
  -1.0f,2.0f,1.0f,
   1.0f,2.0f,1.0f,
   1.0f,2.0f,-1.0f,
   //옆면 1
   -1.0f,2.0f,-1.0f,
   -1.0f,0.0f,-1.0f,
   -1.0f,0.0f,1.0f,
   -1.0f,2.0f,1.0f,
   //뒷면
   1.0f,2.0f,-1.0f,
   1.0f,0.0f,-1.0f,
   -1.0f,0.0f,-1.0f,
   -1.0f,2.0f,-1.0f,
   //아랫면
   -1.0f,0.0f,1.0f,
   -1.0f,0.0f,-1.0f,
   1.0f,0.0f,-1.0f,
   1.0f,0.0f,1.0f,
   //옆면 2
   1.0f,2.0f,1.0f,
   1.0f,0.0f,1.0f,
   1.0f,0.0f,-1.0f,
   1.0f,2.0f,-1.0f,
};
float vertexColor[] = {
   0
};
GLchar* vertexSource, * fragmentSource;
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;

void make_vertexShaders()
{
    vertexSource = filetobuf("vertexShaderSource.glsl");
    //--- 버텍스 세이더 객체 만들기
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //--- 세이더 코드를 세이더 객체에 넣기
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    //--- 버텍스 세이더 컴파일하기
    glCompileShader(vertexShader);
    //--- 컴파일이 제대로 되지 않은 경우: 에러 체크
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}
void make_fragmentShaders()
{
    fragmentSource = filetobuf("fragmentShaderSource.glsl");
    //--- 프래그먼트 세이더 객체 만들기
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //--- 세이더 코드를 세이더 객체에 넣기
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    //--- 프래그먼트 세이더 컴파일
    glCompileShader(fragmentShader);
    //--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}
GLuint make_shaderProgram()
{
    GLuint ShaderProgramID;

    ShaderProgramID = glCreateProgram();
    glAttachShader(ShaderProgramID, vertexShader);
    glAttachShader(ShaderProgramID, fragmentShader);
    glLinkProgram(ShaderProgramID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];

    glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
        exit(-1);
    }
    glUseProgram(ShaderProgramID);

    return ShaderProgramID;
}
GLuint VAO, VBO[2];
void InitBuffer()
{

    glGenVertexArrays(1, &VAO); //--- VAO 를 지정하고 할당하기
    glBindVertexArray(VAO); //--- VAO를 바인드하기
    glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기
    //--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    //--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
    //--- triShape 배열의 사이즈: 9 * float
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);
    //--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute 인덱스 0번을 사용가능하게 함
    glEnableVertexAttribArray(0);
    //--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    //--- 변수 colors에서 버텍스 색상을 복사한다.
    //--- colors 배열의 사이즈: 9 *float 
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexColor), vertexColor, GL_STATIC_DRAW);
    //--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute 인덱스 1번을 사용 가능하게 함.
    glEnableVertexAttribArray(1);
}

typedef struct wall
{
    float x{}, y{}, z{};
    float max_height{};
    float speed{};
    float start_Animation{};
    bool is_Route = false;
    bool y_Plus = true;
}WALL;
//초기 생성용 변수
float maze_Width{10}, maze_Height{25};//미로의 가로, 세로 길이
bool in_Range = false;
int animation = 0;
//벽 관련 변수
float global_Speed = 0;
WALL walls[625];
//키보드 입력
bool start_Flag = true;
bool projection_Flag = true;
bool m_Flag = false;
bool v_Flag = true;
//미로 생성용 변수
int maze_StartX, maze_StartZ;
int randomize;
//카메라 변환용 변수
float cameraZ = 6;
float y_Angle = 0;
bool third_View = true;
//객체 전용 변수
float boxX{}, boxZ{};
bool s_Flag = false;
int boxWay = 1;
bool isMoving[4]{};
bool isColliding()
{
    for (int i = 0; i < maze_Width * maze_Height; i++)
    {
        if(walls[i].x - 0.1f <= boxX + 0.05f && walls[i].x + 0.1f >= boxX - 0.05f && walls[i].z - 0.1f <= boxZ + 0.05f && walls[i].z + 0.1f >= boxZ - 0.05f&&!walls[i].is_Route)return true;
    }
    return false;
}
void clear()
{
    s_Flag = false;
    for (int i = 0; i < maze_Width * maze_Height; i++)
    {
        walls[i].y = 1.f;
        walls[i].is_Route = false;
    }
    m_Flag = false;
    v_Flag = true;
    cameraZ = 3;
    y_Angle = 0;
    projection_Flag = true;
}
bool isWide(int x,int z)
{
    int count = 0;
    if (walls[x - 1 + (z * (int)maze_Width)].is_Route)count++;
    if (walls[x - 1+((z-1) * (int)maze_Width)].is_Route)count++;
    if (walls[x - 1+((z + 1) *(int)maze_Width)].is_Route)count++;
    if (walls[x + 1+(z*(int)maze_Width)].is_Route)count++;
    if (walls[x + 1+((z + 1) *(int)maze_Width)].is_Route)count++;
    if (walls[x + 1+((z - 1) *(int)maze_Width)].is_Route)count++;
    if (walls[x+((z+1)*(int)maze_Width)].is_Route)count++;
    if (walls[x+((z-1)*(int)maze_Width)].is_Route)count++;
    if (count > 3)return true;
    else return false;
}
void make_Maze()
{
    if ((int)maze_Width % 2 == 0)boxX = 0;
    else boxX = -0.1f;
    boxZ = -(maze_Height / 2) * 0.2f;
    boxWay = 1;
    for (int i = 0; i < maze_Width * maze_Height; i++)
    {
        walls[i].is_Route = false;
    }
    maze_StartX = maze_Width / 2;
    maze_StartZ = 0;
    walls[maze_StartX].is_Route = true;
    while (maze_StartZ!=maze_Height-1)
    {
        randomize = rand() % 4;
        switch (randomize)
        {
        case 0:
            maze_StartX--;
            if (maze_StartX < 1)maze_StartX = 1;   
            if (isWide(maze_StartX, maze_StartZ))continue;
            break;
        case 1:
            maze_StartX++;
            if (maze_StartX == maze_Width-1)maze_StartX = maze_Width - 2;
            if (isWide(maze_StartX, maze_StartZ))continue;
            break;
        case 2:
            maze_StartZ--;
            if (maze_StartZ < 1)maze_StartZ = 1;
            if (isWide(maze_StartX, maze_StartZ))continue;
            break;
        case 3:
            maze_StartZ++;
            if (isWide(maze_StartX, maze_StartZ))continue;
            break;
        }
        walls[(maze_StartZ * (int)maze_Width)+maze_StartX].is_Route = true;
        if (maze_StartZ == maze_Height - 1)break;

    }
}
void draw()
{
    unsigned int vColorLocation = glGetUniformLocation(shaderID, "vColor");//색상변경
    unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//모델링 변환 설정
    glUniform4f(vColorLocation, 0.8f, 0.8f, 0.8f, 1.f);

    //바닥
    glm::mat4 floorTransform = glm::mat4(1.0f);
    floorTransform = glm::scale(floorTransform, glm::vec3(2.8f,0.01f,2.8f));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(floorTransform));
    glDrawArrays(GL_QUADS, 0, 24);

    glm::mat4 wallTransform = glm::mat4(1.0f);
    
    glDrawArrays(GL_QUADS, 0, 24);
    walls[0].x = -(maze_Width/2)*0.2f;
    walls[0].z = -(maze_Height/2)*0.2f;
    for (int i = 0; i < maze_Height; i++)
    {
        for (int j = 0; j < maze_Width; j++)
        {            
            walls[(i *(int)maze_Width) + j].x = walls[0].x+j*0.2f;
            walls[(i *(int)maze_Width) + j].z = walls[0].z+i*0.2f;
            wallTransform = glm::mat4(1.0f);
            wallTransform = glm::translate(wallTransform, glm::vec3(walls[(i * (int)maze_Width) + j].x, walls[(i * (int)maze_Width) + j].start_Animation, walls[(i * (int)maze_Width) + j].z));
            if(v_Flag)wallTransform = glm::scale(wallTransform, glm::vec3(1.f, walls[(i * (int)maze_Width) + j].y, 1.f));
            wallTransform = glm::scale(wallTransform, glm::vec3(0.1f, 0.05f, 0.1f));
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(wallTransform));
            glUniform4f(vColorLocation, 0.5f, 0.5f, 0.5f, 1.f);
            if(!walls[(i * (int)maze_Width) + j].is_Route)glDrawArrays(GL_QUADS, 0, 24);
        }
    }
    //객체 그리기
    glm::mat4 boxTransform = glm::mat4(1.0f);
    boxTransform = glm::translate(boxTransform, glm::vec3(boxX, 0.f, boxZ));
    boxTransform = glm::scale(boxTransform, glm::vec3(0.05f, 0.05f, 0.05f));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(boxTransform));
    glUniform4f(vColorLocation, 1.0f, 0.f, 0.f, 1.f);
    if (s_Flag)glDrawArrays(GL_QUADS, 0, 24);
}
GLvoid drawScene()
{
    glUseProgram(shaderID);
    glBindVertexArray(VAO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

   
    unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform"); //뷰잉 변환 설정
    unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform"); //투영 변환 값 설정
    glViewport(0, 0, 800, 800);
    //투영 변환
    glm::mat4 pTransform = glm::mat4(1.0f);
    if (projection_Flag)
    {
        pTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);//원근 투영
    }
    else
    {
        pTransform = glm::ortho(-8.0f, 8.0f, -8.0f, 8.0f, -8.0f, 8.0f);
    }
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &pTransform[0][0]);
    //탑뷰포트
    glViewport(650, 650, 150, 150);
    glm::mat4 topViewTransform = glm::mat4(1.0f);
    glm::vec3 topCameraPos = glm::vec3(0.f, 8.f, 0.f);;
    glm::vec3 topCameraDirection = glm::vec3(0.f, 0.0f, 0.f); //--- 카메라 바라보는 방향
    glm::vec3 topCameraUp = glm::vec3(0.0f, 0.0f, -1.0f); //--- 카메라 위쪽 방향

    topViewTransform = glm::lookAt(topCameraPos, topCameraDirection, topCameraUp);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &topViewTransform[0][0]);
    draw();
    glViewport(0, 0, 650, 650);
    //뷰잉 변환
    glm::mat4 vTransform = glm::mat4(1.0f);
    glm::vec3 cameraPos;
    glm::vec3 cameraDirection;  
    glm::vec3 cameraUp = glm::vec3(0.f, 1.0f, 0.f);;
    if (third_View)
    {
        cameraPos = glm::vec3(sin(y_Angle / 10) * cameraZ, 8.0f, cos(y_Angle / 10) * cameraZ); //--- 카메라 위치
        cameraDirection = glm::vec3(0.f, 0.0f, 0.f); //--- 카메라 바라보는 방향
       
    }
    else
    {
        cameraPos = glm::vec3(boxX, 0.2f, boxZ); //--- 카메라 위치
        switch (boxWay)
        {
        case 1:
            cameraDirection = glm::vec3(boxX, 0.f, boxZ+10.f); //--- 카메라 바라보는 방향
            break;
        case 2:
            cameraDirection = glm::vec3(boxX-10.f, 0.f, boxZ); //--- 카메라 바라보는 방향
            break;
        case 3:
            cameraDirection = glm::vec3(boxX, 0.f, boxZ-10.f); //--- 카메라 바라보는 방향
            break;
        case 4:
            cameraDirection = glm::vec3(boxX+10.f, 0.f, boxZ); //--- 카메라 바라보는 방향
            break;
        }
    }

    vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);
    draw();
    glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}
GLvoid KeyBoard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'o'://직각투영
        projection_Flag = false;
        break;
    case 'p'://원근 투영
        projection_Flag = true;
        break;
    case 'z'://카메라 z축이동
        if(projection_Flag)cameraZ += 0.1f;
        break;
    case 'Z'://카메라 z축이동
        if(projection_Flag)cameraZ-=0.1f;
        break;
    case 'y'://카메라 음의 방향 회전
        y_Angle--;
        break;
    case 'Y':
        y_Angle++;
        break;
    case 'm':
    case 'M'://미로 위아래
        m_Flag = !m_Flag;
        break;
    case 'v':
    case 'V':
        v_Flag = !v_Flag;
        break;
    case 'r':
    case 'R':
        make_Maze();
        break;
    case 's':
    case 'S':
        s_Flag = true;
        break;
    case '=':
    case '+':
        global_Speed += 0.1f;
        break;
    case '-':
    case '_':
        global_Speed -= 0.1f;
        break;
    case 'c'://모든 값 초기화
        clear();
        break;
    case '1':
        third_View = false;
        break;
    case '3':
        third_View = true;
        break;
    case 'q':
        exit(-1);
    default:
        break;
    }
    glutPostRedisplay();
}
GLvoid SpecialKeyBoard(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_DOWN:
        isMoving[0] = true;
        boxWay = 1;
        break;
    case  GLUT_KEY_LEFT:
        isMoving[1] = true;
        boxWay = 2;
        break;
    case GLUT_KEY_UP:
        isMoving[2] = true;
        boxWay = 3;
        break;
    case GLUT_KEY_RIGHT:
        isMoving[3] = true;
        boxWay = 4;
        break;
    }
    glutPostRedisplay();
}
GLvoid SpecialKeyBoardUp(int key, int x, int y)
{
    switch (key)
    {
    default:
    case GLUT_KEY_DOWN:
        isMoving[0] = false;
        break;
    case  GLUT_KEY_LEFT:
        isMoving[1] = false;
        break;
    case GLUT_KEY_UP:
        isMoving[2] = false;
        break;
    case GLUT_KEY_RIGHT:
        isMoving[3] = false;
        break;
    }
    glutPostRedisplay();
}
GLvoid TimerFunc(int x)
{
    switch (x)
    {
    default:
        if (start_Flag)
        {
            animation+=3;
            for (int i = 0; i < maze_Width * maze_Height; i++)
            {
                if (animation >= i&&walls[i].start_Animation>0.f)walls[i].start_Animation -= 0.3f;
                if(walls[i].start_Animation <= 0.f)walls[i].start_Animation = 0.f;
                
            }
            if (walls[(int)maze_Width * (int)maze_Height-1].start_Animation ==0.f)start_Flag = false;
        }
        if (m_Flag)
        {
            for (int i = 0; i < maze_Width * maze_Height; i++)
            {
                if (walls[i].y_Plus) walls[i].y += walls[i].speed+global_Speed;
                else walls[i].y -= walls[i].speed + global_Speed;
                if (walls[i].y <= 0.05f)walls[i].y == 0.05f;
                if (walls[i].y<=0.05f || walls[i].y>walls[i].max_height)walls[i].y_Plus = !walls[i].y_Plus;
            }
        }
        if (isMoving[0])
        {
            boxZ += 0.01f;
            if (isColliding())boxZ -= 0.01f;
        }
        if (isMoving[1])
        {
            boxX -= 0.01f;
            if (isColliding())boxX += 0.01f;
        }
        if (isMoving[2])
        {
            boxZ -= 0.01f;
            if (isColliding())boxZ += 0.01f;
        }
        if (isMoving[3])
        {
            boxX += 0.01f;
            if (isColliding())boxX -= 0.01f;
        }
        break;
    }
    glutTimerFunc(10, TimerFunc, 1);
    glutPostRedisplay();
}
int main(int argc, char** argv)
{
    srand(time(NULL));
    while (1)
    {
        std::cout << "가로, 세로 입력( 5 ≤ ? ≤ 25) : ";
        std::cin >> maze_Width >> maze_Height;
        if (maze_Width >= 5 && maze_Width <= 25 && maze_Height >= 5 && maze_Height <= 25)break;
        system("cls");
        std::cout << "5와 25 사이의 숫자를 입력해주세요" << std::endl;
    }
    std::cout << "o/p : 투영 방식 선택" << std::endl;
    std::cout << "z/Z : 카메라를 z 축 방향으로 이동(대소문자 방향 다름)" << std::endl;
    std::cout << "m/M : 육면체들이 위 아래로 움직인다/멈춘다(토글)" << std::endl;
    std::cout << "y/Y : 바닥의 y축을 기준으로 양/음의 방향으로 회전(대 소문자 방향 다름)" << std::endl;
    std::cout << "r : 미로 제작" << std::endl;
    std::cout << "v : 육면체들 움직임을 멈추고 낮은 높이로 변환" << std::endl;
    std::cout << "s : 미로 내부에 객체 생성" << std::endl;
    std::cout << "방향키 : 객체를 앞/뒤/좌/우 이동" << std::endl;
    std::cout << "+/- : 육면체 이동하는 속도 증가/감소" << std::endl;
    std::cout << "1/3 : 카메라 시점 1인칭/3인칭 변환" << std::endl;
    std::cout << "c : 모든 값 초기화" << std::endl;
    std::cout << "q : 프로그램 종료" << std::endl;

    for (int i = 0; i < maze_Width * maze_Height; i++)
    {
        walls[i].speed = (float)(rand() % 100) / 100;
        walls[i].max_height = (rand() % 25)+1;
        walls[i].y = 1.f;
        walls[i].start_Animation = 10.f;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Example1");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";

    make_vertexShaders();
    make_fragmentShaders();
    shaderID = make_shaderProgram();
    InitBuffer();
    glutKeyboardFunc(KeyBoard);
    glutSpecialFunc(SpecialKeyBoard);
    glutSpecialUpFunc(SpecialKeyBoardUp);
    glutTimerFunc(10, TimerFunc, 1);
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMainLoop();
}