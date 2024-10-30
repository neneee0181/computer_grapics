#include <iostream>   // ǥ�� ����� ���̺귯�� ����
#include <gl/glew.h>  // GLEW ���̺귯�� ���� (OpenGL Ȯ�� ����� ����ϱ� ���� �ʿ�)
#include <gl/freeglut.h>  // GLUT ���̺귯�� ���� (������ â �� ����� �Է� ó���� ���� ���)
#include <gl/freeglut_ext.h>  // GLUT Ȯ�� ��� ����
#include <gl/glm/glm/glm.hpp>  // GLM ���̺귯�� ���� (�������� ����� ���� ���� ���̺귯��)
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector>  // ǥ�� ���� ���̺귯�� ���� (���� �迭�� ����ϱ� ���� �ʿ�)

#include "LoadObj.h"
#include "shaderMaker.h"

using namespace std;  // ���ӽ����̽� std ������� �ڵ� ������ std:: ���� ����

// �Լ� �����
void InitBuffer();  // ���� �ʱ�ȭ �Լ� ����
GLvoid drawScene(GLvoid);  // ȭ���� �׸��� �Լ� ����
GLvoid Reshape(int w, int h);  // ȭ�� ũ�Ⱑ ����� �� ȣ��Ǵ� �Լ� ����

// ���� 3D ���� �����ϴ� ����
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

// ȭ�� ũ�Ⱑ ����� �� ȣ��Ǵ� �Լ�
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);  // ����Ʈ ũ�� ����
    width = w;
    height = h;
}

void timer(int value) {
    glutPostRedisplay();  // ȭ�� �ٽ� �׸��� ��û
    glutTimerFunc(16, timer, 0);
}

void keyBoard(unsigned char key, int x, int y) {
    glutPostRedisplay();  // ȭ�� �ٽ� �׸��� ��û
}

// ���콺 �Է��� ó���ϴ� �Լ�
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        std::cout << "x = " << x << " y = " << y << std::endl;  // ���콺 Ŭ�� ��ǥ ���
    glutPostRedisplay();  // Ű���� �Է� �� ȭ���� �ٽ� �׸����� ��û
}

void rotation_xyz(Model& model, float radians, glm::vec3 xyz) {
    model.modelMatrix = glm::rotate(model.modelMatrix, glm::radians(radians), xyz);
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
    glutCreateWindow("template");  // â ���� �� ���� ����

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
    Model modelBoard, modelBottomBox;

    // �ٴ�
    read_obj_file("obj/board.obj", modelBoard, "board");
    // �ʱ� ȸ�� ���¸� ���� (�ʱ� ȸ�� ���´� ������)
    modelBoard.initialRotation = glm::mat4(1.0f);  // �ʱ� ȸ�� ����� ���� ��ķ� ����
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

    InitBuffer();  // ���� �ʱ�ȭ

    // �ݹ� �Լ� ���
    glutDisplayFunc(drawScene);  // ȭ���� �׸��� �Լ� ���
    glutReshapeFunc(Reshape);  // ȭ�� ũ�� ���� �� ȣ��Ǵ� �Լ� ���
    glutKeyboardFunc(keyBoard);  // Ű���� �Է� ó�� �Լ� ���
    glutMouseFunc(mouse);  // ���콺 �Է� ó�� �Լ� ���
    glutMainLoop();  // �̺�Ʈ ���� ����

    return 0;  // ���α׷� ���� ����
}

// ȭ���� �׸��� �Լ�
GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);  // ȭ���� ������� �ʱ�ȭ
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // �÷� ���ۿ� ���� ���� �ʱ�ȭ

    glUseProgram(shaderProgramID);  // ���̴� ���α׷� ���

    // ī�޶�
    cameraPos = glm::vec3(0.0, 0.0, 6.0);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 rotationMatrix_x = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationMatrix_y = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cameraPos = glm::vec3(rotationMatrix_x * rotationMatrix_y * glm::vec4(cameraPos, 1.0f));
    view = glm::lookAt(
        cameraPos,  //--- ī�޶���ġ
        cameraDirection,  //--- ī�޶�ٶ󺸴¹���
        cameraUp   //--- ī�޶����ʹ���
    );
    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    // ����
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    glEnable(GL_DEPTH_TEST);

    GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    // �� ���� �׸���
    for (size_t i = 0; i < models.size(); ++i) {
        glBindVertexArray(vaos[i]);

        // �� ���� ���� ���� ����
        GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
        glUniform3fv(KdLoc, 1, glm::value_ptr(models[i].material.Kd));

        if (models[i].name == "board" || models[i].name == "bottom_b") {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            // �� �׸���
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

    glutSwapBuffers();  // ���� ���۸� ���, ȭ�� ����

    // OpenGL ���� üũ ����
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;  // ���� �޽��� ���
    }
}

// ���� �ʱ�ȭ �Լ�
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

    // �� �𵨿� ���� VAO, VBO, EBO ����
    vaos.resize(models.size());
    vbos.resize(models.size(), vector<GLuint>(4)); // �𵨸��� 4���� VBO�� �ʿ� (����, ����, ����, �ؽ�ó ��ǥ)

    for (size_t i = 0; i < models.size(); ++i) {
        glGenVertexArrays(1, &vaos[i]);
        glBindVertexArray(vaos[i]);

        // VBO ����
        glGenBuffers(4, vbos[i].data());

        // ���� ���� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][0]);
        glBufferData(GL_ARRAY_BUFFER, models[i].vertices.size() * sizeof(glm::vec3), models[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0�� ���� �Ҵ�
        glEnableVertexAttribArray(0);

        // ���� ���� ����
        //glBindBuffer(GL_ARRAY_BUFFER, vbos[i][1]);
        //glBufferData(GL_ARRAY_BUFFER, models[i].colors.size() * sizeof(glm::vec3), models[i].colors.data(), GL_STATIC_DRAW);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1�� ���� �Ҵ�
        //glEnableVertexAttribArray(1);

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
