#include <iostream>   // ǥ�� ����� ���̺귯�� ����
#include <gl/glew.h>  // GLEW ���̺귯�� ���� (OpenGL Ȯ�� ����� ����ϱ� ���� �ʿ�)
#include <gl/freeglut.h>  // GLUT ���̺귯�� ���� (������ â �� ����� �Է� ó���� ���� ���)
#include <gl/freeglut_ext.h>  // GLUT Ȯ�� ��� ����
#include <gl/glm/glm/glm.hpp>  // GLM ���̺귯�� ���� (�������� ����� ���� ���� ���̺귯��)
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector>  // ǥ�� ���� ���̺귯�� ���� (���� �迭�� ����ϱ� ���� �ʿ�)
#include <random>

#include "filetobuf.h"  // ���̴� �ҽ� ������ ���۷� �о���� �Լ��� ������ �������
#include "ReadObj.h"

using namespace std;  // ���ӽ����̽� std ������� �ڵ� ������ std:: ���� ����

random_device rd;
mt19937 gen(rd());

// �Լ� �����
void make_vertexShaders();  // ���ؽ� ���̴��� ����� �Լ� ����
void make_fragmentShaders();  // �����׸�Ʈ ���̴��� ����� �Լ� ����
void InitBuffer();  // ���� �ʱ�ȭ �Լ� ����
void make_shaderProgram();  // ���̴� ���α׷��� ����� �Լ� ����
GLvoid drawScene(GLvoid);  // ȭ���� �׸��� �Լ� ����
GLvoid Reshape(int w, int h);  // ȭ�� ũ�Ⱑ ����� �� ȣ��Ǵ� �Լ� ����

// ���� ���� �����
GLint width, height;  // â�� �ʺ�� ���̸� �����ϴ� ����
GLuint shaderProgramID;  // ���̴� ���α׷��� ID�� �����ϴ� ����
GLuint vertexShader;  // ���ؽ� ���̴� ID�� �����ϴ� ����
GLuint fragmentShader;  // �����׸�Ʈ ���̴� ID�� �����ϴ� ����

GLchar* vertexSource, * fragmentSource;  // ���̴� �ҽ� �ڵ带 ������ ������

GLuint xVBO, xVAO;
GLuint yVBO, yVAO;

// ���� 3D ���� �����ϴ� ����
vector<Model> models;
vector<GLuint> vaos;
vector<vector<GLuint>> vbos;

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

const glm::vec3 x[2] = {
    glm::vec3(-1.0f,0.0f, 0.5f), glm::vec3(1.0f,0.0f,0.5f)
};

const glm::vec3 y[2] = {
      glm::vec3(0.0f,-1.0f,0.5f), glm::vec3(0.0f,1.0f,0.5f)
};
char key_result = ' ';
// Ű���� �Է��� ó���ϴ� �Լ�
void keyBoard(unsigned char key, int x, int y) {
    switch (key)
    {
    case '1':
        key_result = '1';
        break;
    case '2':
        key_result = '2';
        break;
    case '3':
        key_result = '3';
        break;
    case '4':
        key_result = '4';
        break;
    case '5':
        key_result = '5';
        break;
    case '6':
        key_result = '6';
        break;
    case '7':
        key_result = '7';
        break;
    case '8':
        key_result = '8';
        break;
    case '9':
        key_result = '9';
        break;
    case '0':
        key_result = '0';
        break;
    case 'c':
        key_result = 'c';
        break;
    case 't':
        key_result = 't';
        break;
    default:
        break;
    }
    glutPostRedisplay();  // Ű���� �Է� �� ȭ���� �ٽ� �׸����� ��û
}

// ���콺 �Է��� ó���ϴ� �Լ�
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        std::cout << "x = " << x << " y = " << y << std::endl;  // ���콺 Ŭ�� ��ǥ ���
    glutPostRedisplay();  // Ű���� �Է� �� ȭ���� �ٽ� �׸����� ��û
}

// ���� �Լ�
int main(int argc, char** argv) {

    width = 800;  // â�� �⺻ �ʺ� ����
    height = 600;  // â�� �⺻ ���� ����

    // ������ ����
    glutInit(&argc, argv);  // GLUT �ʱ�ȭ
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);  // ȭ�� ��� ���� (���� ���۸�, RGBA �÷� ���)
    glutInitWindowPosition(100, 100);  // â�� ���� ��ġ ����
    glutInitWindowSize(width, height);  // â�� ũ�� ����
    glutCreateWindow("�ǽ� 13��");  // â ���� �� ���� ����

    // GLEW �ʱ�ȭ
    glewExperimental = GL_TRUE;  // GLEW ������ ��� Ȱ��ȭ
    if (glewInit() != GLEW_OK) {  // GLEW �ʱ�ȭ �� ���� üũ
        cerr << "Unable to initialize GLEW" << endl;  // GLEW �ʱ�ȭ ���� �� ���� �޽��� ���
        exit(EXIT_FAILURE);  // ���α׷� ����
    }
    else
        cout << "GLEW Initialized\n";  // �ʱ�ȭ ���� �� �޽��� ���

    make_shaderProgram();  // ���̴� ���α׷� ����
        
    // �𵨵��� �ε��ϰ� ���Ϳ� �߰�
    Model modelSqu, modelTre;
    read_obj_file("box3.obj", modelSqu, "cube");
    read_obj_file("tetrahedron.obj", modelTre, "tre");

    models.push_back(modelSqu);
    models.push_back(modelTre);

    //try {
    //    std::cout << "OBJ ���� �ε� ����!" << std::endl;
    //    std::cout << "���� ����: " << modelSqu.vertices.size() << std::endl;
    //    for (size_t i = 0; i < modelSqu.vertices.size(); ++i) {
    //        std::cout << "v[" << i << "] = ("
    //            << modelSqu.vertices[i].x << ", "
    //            << modelSqu.vertices[i].y << ", "
    //            << modelSqu.vertices[i].z << ")" << std::endl;
    //    }
    //    std::cout << "�ؽ�ó ��ǥ ����: " << modelSqu.texCoords.size() << std::endl;
    //    std::cout << "���� ���� ����: " << modelSqu.normals.size() << std::endl;
    //    std::cout << "�� ����: " << modelSqu.faces.size() << std::endl;
    //    for (size_t i = 0; i < modelSqu.faces.size(); ++i) {
    //        const Face& face = modelSqu.faces[i];
    //        std::cout << "Face[" << i << "] = ("
    //            << face.v1 << ", " << face.v2 << ", " << face.v3 << ")"
    //            << std::endl;
    //    }
    //}
    //catch (const std::exception& e) {
    //    std::cerr << e.what() << std::endl;  // ���� �޽��� ���
    //}

    InitBuffer();  // ���� �ʱ�ȭ

    // �ݹ� �Լ� ���
    glutDisplayFunc(drawScene);  // ȭ���� �׸��� �Լ� ���
    glutReshapeFunc(Reshape);  // ȭ�� ũ�� ���� �� ȣ��Ǵ� �Լ� ���
    glutKeyboardFunc(keyBoard);  // Ű���� �Է� ó�� �Լ� ���
    glutMouseFunc(mouse);  // ���콺 �Է� ó�� �Լ� ���
    glutMainLoop();  // �̺�Ʈ ���� ����

    return 0;  // ���α׷� ���� ����
}
glm::mat4 modelTreMatrix = glm::mat4(1.0f);  // ���� ��ķ� �ʱ�ȭ
glm::mat4 modelSquMatrix = glm::mat4(1.0f);  // ���� ��ķ� �ʱ�ȭ
glm::mat4 xLineMatrix = glm::mat4(1.0f);
glm::mat4 yLineMatrix = glm::mat4(1.0f);

// ȭ���� �׸��� �Լ�
GLvoid drawScene() {

    glClearColor(0.0, 0.0, 0.0, 1.0f);  // ȭ���� ������� �ʱ�ȭ
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // �÷� ���ۿ� ���� ���� �ʱ�ȭ

    glUseProgram(shaderProgramID);  // ���̴� ���α׷� ���

    // ī�޶� ���� (�� ��ȯ)
    glm::mat4 view = glm::lookAt(
        glm::vec3(2.0f, 1.0f, 2.0f),  // ī�޶� ��ġ
        glm::vec3(0.0f, 0.0f, 0.0f),  // ī�޶� �ٶ󺸴� ����
        glm::vec3(0.0f, 1.0f, 0.0f)   // ���� �� ����
    );

    // ���� ���� ���� (���� ��ȯ)
    glm::mat4 projection = glm::perspective(
        glm::radians(52.0f),  // �þ߰�(FOV)
        (float)width / (float)height,  // ȭ�� ���� (��Ⱦ��)
        0.1f, 100.0f  // �ٰŸ� �� ���Ÿ� Ŭ�� ���
    );

    // ���̴��� ���� �� �� ��ȯ ����
    GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgramID, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint isLineLoc = glGetUniformLocation(shaderProgramID, "isLine");

    // �� ���� �׸���
    for (size_t i = 0; i < models.size(); ++i) {
        glBindVertexArray(vaos[i]);
        glEnable(GL_DEPTH_TEST);
         
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        /*modelMatrix = glm::rotate(modelMatrix, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));*/

        GLint modelLoc = glGetUniformLocation(shaderProgramID, "modelV");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        // �� �׸���
        glUniform1i(isLineLoc, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);

        if (models[i].name == "cube") {
            switch (key_result)
            {
            case '1':
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                break;
            case '2':
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));
                break;
            case '3':
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(unsigned int)));
                break;
            case '4':
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(unsigned int)));
                break;
            case '5':
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(unsigned int)));
                break;
            case '6':
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(unsigned int)));
                break;
            case 'c':
            {
                uniform_int_distribution<> rand_c(0, 5);
                int rand_i1 = rand_c(gen);
                int rand_i2 = rand_c(gen);
                while (true) {
                    if (rand_i1 == rand_i2)
                        rand_i2 = rand_c(gen);
                    else
                        break;
                }
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((rand_i1 * 6) * sizeof(unsigned int)));
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((rand_i2 * 6) * sizeof(unsigned int)));

                break;
            }
            default:
                break;
            }
            //if (key_result > '0' && key_result < '7') {
            //    // �ܰ��� �׸���
            //    glUniform1i(isLineLoc, 1);
            //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //    glLineWidth(1.0f);
            //    glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
            //}
        }

        if (models[i].name == "tre") {
            switch (key_result)
            {
            case '7':
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
                break;
            case '8':
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3 * sizeof(unsigned int)));
                break;
            case '9':
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));
                break;
            case '0':
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(9 * sizeof(unsigned int)));
                break;
            case 't':
            {
                uniform_int_distribution<> rand_c(0, 3);
                int rand_i1 = rand_c(gen);
                int rand_i2 = rand_c(gen);
                while (true) {
                    if (rand_i1 == rand_i2)
                        rand_i2 = rand_c(gen);
                    else
                        break;
                }
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)((rand_i1 * 3) * sizeof(unsigned int)));
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)((rand_i2 * 3) * sizeof(unsigned int)));

                break;
            }
            default:
                break;
            }

            //if (key_result >= '7' && key_result <= '9' || key_result == '0') {
            //    // �ܰ��� �׸���
            //    glUniform1i(isLineLoc, 1);
            //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //    glLineWidth(1.0f);
            //    glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
            //}
        }

        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(0);
    }

    // 1. X�� �׸���
    //glDisable(GL_DEPTH_TEST);
    glBindVertexArray(xVAO);  // X�� VAO ���ε�
    glEnable(GL_DEPTH_TEST);
    GLint xLineLoc = glGetUniformLocation(shaderProgramID, "xLine");
    glUniformMatrix4fv(xLineLoc, 1, GL_FALSE, glm::value_ptr(xLineMatrix));  // �ʱ�ȭ�� ��ȯ ��� ����
    glUniform1i(isLineLoc, 2);  // isLine ���� 2�� ���� (�� �׸��� ��)
    glLineWidth(2.0f);  // �� �β� ����
    glDrawArrays(GL_LINES, 0, 2);  // X�� �׸���
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(0);  // VAO ���ε� ����
    //glEnable(GL_DEPTH_TEST);

    glBindVertexArray(yVAO);  // X�� VAO ���ε�
    glEnable(GL_DEPTH_TEST);
    GLint yLineLoc = glGetUniformLocation(shaderProgramID, "yLine");
    glUniformMatrix4fv(yLineLoc, 1, GL_FALSE, glm::value_ptr(yLineMatrix));  // �ʱ�ȭ�� ��ȯ ��� ����
    glUniform1i(isLineLoc, 3);  // isLine ���� 2�� ���� (�� �׸��� ��)
    glLineWidth(2.0f);  // �� �β� ����
    glDrawArrays(GL_LINES, 0, 2);  // X�� �׸���
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(0);  // VAO ���ε� ����

    glutSwapBuffers();  // ���� ���۸� ���, ȭ�� ����

    // OpenGL ���� üũ ����
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;  // ���� �޽��� ���
    }
}


// ȭ�� ũ�Ⱑ ����� �� ȣ��Ǵ� �Լ�
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);  // ����Ʈ ũ�� ����
}

// ���ؽ� ���̴��� ����� �Լ�
void make_vertexShaders() {

    vertexSource = filetobuf("vertex.glsl");  // ���ؽ� ���̴� ���� �б�
    vertexShader = glCreateShader(GL_VERTEX_SHADER);  // ���ؽ� ���̴� ����
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);  // ���̴� �ҽ� �ڵ� ����
    glCompileShader(vertexShader);  // ���̴� ������

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);  // ������ ���� ���� Ȯ��

    if (!result) {  // ������ ���� ��
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);  // ���� �α� �ޱ�
        cerr << "ERROR: vertex shader ������ ���� \n" << errorLog << endl;  // ���� �޽��� ���
        return;
    }
}

// �����׸�Ʈ ���̴��� ����� �Լ�
void make_fragmentShaders() {

    fragmentSource = filetobuf("fragment.glsl");  // �����׸�Ʈ ���̴� ���� �б�
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  // �����׸�Ʈ ���̴� ����
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);  // ���̴� �ҽ� �ڵ� ����
    glCompileShader(fragmentShader);  // ���̴� ������

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);  // ������ ���� ���� Ȯ��

    if (!result) {  // ������ ���� ��
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);  // ���� �α� �ޱ�
        cerr << "ERROR: frag_shader ������ ����\n" << errorLog << endl;  // ���� �޽��� ���
        return;
    }
}

// ���̴� ���α׷��� ����� �Լ�
void make_shaderProgram() {

    make_vertexShaders();  // ���ؽ� ���̴� ����
    make_fragmentShaders();  // �����׸�Ʈ ���̴� ����

    shaderProgramID = glCreateProgram();  // ���̴� ���α׷� ����
    glAttachShader(shaderProgramID, vertexShader);  // ���ؽ� ���̴� ���α׷��� ÷��
    glAttachShader(shaderProgramID, fragmentShader);  // �����׸�Ʈ ���̴� ���α׷��� ÷��
    glLinkProgram(shaderProgramID);  // ���̴� ���α׷� ��ũ

    glDeleteShader(vertexShader);  // ���ؽ� ���̴� ���� (�̹� ���α׷��� ���ԵǾ����Ƿ�)
    glDeleteShader(fragmentShader);  // �����׸�Ʈ ���̴� ����

    glUseProgram(shaderProgramID);  // ���̴� ���α׷� ���
}

// ���� �ʱ�ȭ �Լ�
void InitBuffer() {


    // X�� ��ǥ�� ���� ���� VAO�� VBO ����
    glGenVertexArrays(1, &xVAO);  // X�� VAO ����
    glBindVertexArray(xVAO);  // VAO ���ε�
    glGenBuffers(1, &xVBO);  // VBO ����
    glBindBuffer(GL_ARRAY_BUFFER, xVBO);  // ���� ���ε�
    glBufferData(GL_ARRAY_BUFFER, sizeof(x), x, GL_STATIC_DRAW);  // X�� ��ǥ ������ ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // ���� �Ӽ� ������ ����
    glEnableVertexAttribArray(0);  // ���� �Ӽ� ���

    glGenVertexArrays(1, &yVAO); 
    glBindVertexArray(yVAO);  // VAO ���ε�
    glGenBuffers(1, &yVBO);  // VBO ����
    glBindBuffer(GL_ARRAY_BUFFER, yVBO);  // ���� ���ε�
    glBufferData(GL_ARRAY_BUFFER, sizeof(y), y, GL_STATIC_DRAW);  // X�� ��ǥ ������ ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // ���� �Ӽ� ������ ����
    glEnableVertexAttribArray(0);  // ���� �Ӽ� ���


    // -----------------------------------------------------------------------------------------------------
    // �� �𵨿� ���� VAO, VBO, EBO ����
    vaos.resize(models.size());
    vbos.resize(models.size(), vector<GLuint>(2)); // �𵨸��� 2���� VBO�� �ʿ�

    for (size_t i = 0; i < models.size(); ++i) {
        glGenVertexArrays(1, &vaos[i]);
        glBindVertexArray(vaos[i]);

        glGenBuffers(2, vbos[i].data());

        // ���� ���� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][0]);
        glBufferData(GL_ARRAY_BUFFER, models[i].vertices.size() * sizeof(glm::vec3), models[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // ���� ���� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        // �� �ε��� ������ (EBO) ����
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