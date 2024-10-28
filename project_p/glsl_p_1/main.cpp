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

GLchar* vertexSource, * fragmentSource;   // ���̴� �ҽ� �ڵ带 ������ ������
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü

vector<Model> models;

const GLfloat colors[12][3] = {
   {1.0, 0.0, 0.0},   // ����
   {0.0, 1.0, 0.0},   // �ʷ�
   {0.0, 0.0, 1.0},   // �Ķ�
   {1.0, 1.0, 0.0},   // ���
   {1.0, 0.0, 1.0},   // ����Ÿ
   {0.0, 1.0, 1.0},   // �þ�
   {0.5, 0.5, 0.5},   // ȸ��
   {1.0, 0.5, 0.0},   // ��Ȳ
   {0.0, 0.5, 0.5},   // û��
   {0.5, 0.0, 0.5},   // ����
   {0.5, 1.0, 0.5},   // ����
   {1.0, 0.5, 0.5}    // ��ũ
};

const GLfloat colors2[4][3] = {
   {1.0, 0.0, 0.0},   // ����
   {0.0, 1.0, 0.0},   // �ʷ�
   {0.0, 0.0, 1.0},   // �Ķ�
   {1.0, 1.0, 0.0},   // ���
};


GLuint VAO_x, VBO_x[2];
GLuint VAO_y, VBO_y[2];
vector<GLuint > m_vao;
vector<vector<GLuint>> m_vbo;

const glm::vec3 X[2]{ glm::vec3(-1.0f,0.0f,0.5f),glm::vec3(1.0f,0.0f,0.5f) };      //z�� ���� 0.0���� �ֵ� ������ �� �� ����� ���̸� �ְ� ������ 0.5�� ����. ���ڰ� Ŀ������ ���̰��� �������
const glm::vec3 Y[2]{ glm::vec3(0.0f,-1.0f,0.5f),glm::vec3(0.0f,1.0f,0.5f) };      //z�� ���� 0.0���� �ֵ� ������ �� �� ����� ���̸� �ְ� ������ 0.5�� ����. ���ڰ� Ŀ������ ���̰��� �������

//--- ���� �Լ�
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
    //--- ������ �����ϱ�
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("�ǽ� 13");
    //--- GLEW �ʱ�ȭ�ϱ�

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";
    Model model_Sq, model_Tr;
    read_obj_file("box3.obj", model_Sq, "cube");         //����ü
    read_obj_file("tetrahedron.obj", model_Sq, "tetrahedron");      //���ü

    models.push_back(model_Sq);
    models.push_back(model_Tr);

    //--- ���̴� �о�ͼ� ���̴� ���α׷� �����
    make_vertexShaders(); //--- ���ؽ� ���̴� �����
    make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
    shaderProgramID = make_shaderProgram();
    InitBuffer();
    //--- ���̴� ���α׷� �����
    glutKeyboardFunc(Keyboard);
    glutDisplayFunc(DrawScene); //--- ��� �ݹ� �Լ�
    glutReshapeFunc(Reshape);
    glutMainLoop();
}

GLvoid Keyboard(unsigned char key, int x, int y) {

    switch (key) {
        //����ü�� �� �� �׸���
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
        //���ü�� �� �� �׸���
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

void DrawScene() //--- glutDisplayFunc()�Լ��� ����� �׸��� �ݹ� �Լ�
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    glBindVertexArray(VAO_x);
    glDrawArrays(GL_LINES, 0, 2);   //x��

    glBindVertexArray(VAO_y);
    glDrawArrays(GL_LINES, 0, 2);   //y��

    //glm::mat4 transForm = glm::mat4(1.0f);   // �������(�밢���� ��� 1�̰� ������ 0�� ���) ����
    //transForm = glm::rotate(transForm, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));   // x�� 10�� ȸ��
    //transForm = glm::rotate(transForm, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));   // y�� 10�� ȸ��

    //GLuint transformLoc = glGetUniformLocation(shaderProgramID, "transform");
    //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transForm)); // ��ȯ ����� ���̴��� ����
    ////��
    //glDrawArrays(GL_LINES, 0, 4);

    //glDrawArrays(GL_QUADS, 4, 24); //�簢��


    glutSwapBuffers();
}

void InitBuffer()
{
    //x��
    //glGenVertexArrays(1, &VAO_x); //--- VAO �� �����ϰ� �Ҵ��ϱ�
    //glBindVertexArray(VAO_x); //--- VAO�� ���ε��ϱ�
    //glGenBuffers(2, &VBO_x); //--- 1���� VBO�� �����ϰ� �Ҵ��ϱ�   //���� ����� �Ŷ�

    //glBindBuffer(GL_ARRAY_BUFFER, VBO_x);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(X), X, GL_STATIC_DRAW);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(0);


    ////y��
    //glGenVertexArrays(1, &VAO_y); //--- VAO �� �����ϰ� �Ҵ��ϱ�
    //glBindVertexArray(VAO_y); //--- VAO�� ���ε��ϱ�
    //glGenBuffers(1, &VBO_y); //--- 1���� VBO�� �����ϰ� �Ҵ��ϱ�

    //glBindBuffer(GL_ARRAY_BUFFER, VBO_y);
    //glBufferData(GL_ARRAY_BUFFER,  sizeof(Y), Y, GL_STATIC_DRAW);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(0);
    //x��
    glGenVertexArrays(1, &VAO_x); //--- VAO �� �����ϰ� �Ҵ��ϱ�
    glBindVertexArray(VAO_x); //--- VAO�� ���ε��ϱ�
    glGenBuffers(1,&VBO_x[0]); //--- 1���� VBO�� �����ϰ� �Ҵ��ϱ�   //���� ����� �Ŷ�

    glBindBuffer(GL_ARRAY_BUFFER, VBO_x[0]);
    glBufferData(GL_ARRAY_BUFFER, 1000, X, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_x[1]);
    glBufferData(GL_ARRAY_BUFFER, 1000, colors2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    //y��
    glGenVertexArrays(1, &VAO_y); //--- VAO �� �����ϰ� �Ҵ��ϱ�
    glBindVertexArray(VAO_y); //--- VAO�� ���ε��ϱ�
    glGenBuffers(2, &VBO_y[0]); //--- 1���� VBO�� �����ϰ� �Ҵ��ϱ�

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

    //   glGenVertexArrays(1, &m_vao[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
    //   glBindVertexArray(m_vao[i]); //--- VAO�� ���ε��ϱ�

    //   glGenBuffers(2, m_vbo[i].data()); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

    //   //--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
    //   glBindBuffer(GL_ARRAY_BUFFER, m_vbo[i][0]);
    //   glBufferData(GL_ARRAY_BUFFER, models[i].vertices.size()*sizeof(glm::vec3), models[i].vertices.data(), GL_STATIC_DRAW);
    //   //--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
    //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //   //--- attribute �ε��� 0���� ��밡���ϰ� ��
    //   glEnableVertexAttribArray(0);

    //   //--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
    //   glBindBuffer(GL_ARRAY_BUFFER, m_vbo[i][1]);
    //   //--- ���� colors���� ���ؽ� ������ �����Ѵ�.
    //   glBufferData(GL_ARRAY_BUFFER,sizeof(colors), colors, GL_STATIC_DRAW);
    //   //--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
    //   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //   //--- attribute �ε��� 1���� ��� �����ϰ� ��.
    //   glEnableVertexAttribArray(1);
    //}

}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
    glViewport(0, 0, w, h);
}

void make_vertexShaders()
{
    //--- filetobuf: ��������� �Լ��� �ؽ�Ʈ�� �о ���ڿ��� �����ϴ� �Լ�
    vertexSource = filetobuf("vertexShader.glsl"); //--- ���ؽ����̴� �о����
    //--- ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�

    //vertex shader ���� ���ְ� ������ �����͸� vertexShader��� ������ ����
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    //���̴� ��ü, 1, ���̴� �ҽ� �ڵ�, NULL
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    //shader, ���ڿ����� ����, ���ڿ����� �迭, ���ڿ����� ���̸� ������ int�迭

    //������ ��
    glCompileShader(vertexShader);

    //���̴� ��� ��� �ƴ���? ���� Ȯ���ϴ� �ڵ�
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)   //result == 0�̸� ������ �ִ�.
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        cerr << "ERROR: vertex shader error\n" << errorLog << endl;
        return;
    }
}

void make_fragmentShaders()
{
    fragmentSource = filetobuf("fragmentShader.glsl"); //--- �����׸�Ʈ���̴� �о����
    //--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
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

GLuint make_shaderProgram()      //--- ���̴� ���α׷� ����� ���̴� ��ü ��ũ�ϱ�
{
    GLuint shaderID = glCreateProgram(); //--- ���̴� ���α׷� �����

    glAttachShader(shaderID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
    glAttachShader(shaderID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
    glLinkProgram(shaderID); //--- ���̴� ���α׷� ��ũ�ϱ�

    glDeleteShader(vertexShader); //--- ���̴� ���α׷��� ��ũ�Ͽ� ���̴� ��ü ��ü�� ���� ����
    glDeleteShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];

    glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        cerr << "ERROR: shader program ���� ����\n" << errorLog << endl;
        return false;
    }
    glUseProgram(shaderID);
    return shaderID;
}