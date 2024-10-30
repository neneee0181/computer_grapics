#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector> 

#include "LoadObj.h"
#include "shaderMaker.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

vector<Model> models;
vector<GLuint> vaos;
vector<vector<GLuint>> vbos;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 6.0);
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

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    width = w;
    height = h;
}

void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyBoard(unsigned char key, int x, int y) {
    glutPostRedisplay(); 
}

void keySpecial(int key, int x, int y) {

    glm::mat4 rotationMatrix;
    if (key == GLUT_KEY_LEFT) {
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (key == GLUT_KEY_RIGHT) {
       rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (key == GLUT_KEY_UP) {
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    else if (key == GLUT_KEY_DOWN) {
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    else {
        return;
    }

    cameraPos = glm::vec3(rotationMatrix * glm::vec4(cameraPos, 1.0f));

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        std::cout << "x = " << x << " y = " << y << std::endl;
    glutPostRedisplay();
}

int main(int argc, char** argv) {

    width = 800;
    height = 600;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("template");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    make_shaderProgram();
        
    Model modelBoard, modelBottomBox;

    read_obj_file("obj/board.obj", modelBoard, "board");
    modelBoard.initialRotation = glm::mat4(1.0f);
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

    InitBuffer();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyBoard); 
    glutSpecialFunc(keySpecial);
    glutMouseFunc(mouse);
    glutMainLoop();

    return 0; 
}

GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(
        cameraPos,  
        cameraDirection,  
        cameraUp   
    );
    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
    GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(-cameraPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(1.0f, 0.95f, 0.9f)));

    glEnable(GL_DEPTH_TEST);

    GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");

    for (size_t i = 0; i < models.size(); ++i) {
        glBindVertexArray(vaos[i]);

        GLint KaLoc = glGetUniformLocation(shaderProgramID, "Ka");
        GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
        GLint KsLoc = glGetUniformLocation(shaderProgramID, "Ks");
        GLint NsLoc = glGetUniformLocation(shaderProgramID, "Ns");

        glUniform3fv(KaLoc, 1, glm::value_ptr(models[i].material.Ka));
        glUniform3fv(KdLoc, 1, glm::value_ptr(models[i].material.Kd));
        glUniform3fv(KsLoc, 1, glm::value_ptr(models[i].material.Ks));
        glUniform1f(NsLoc, models[i].material.Ns);

        if (models[i].name == "board" || models[i].name == "bottom_b") {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));

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

    glutSwapBuffers();

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;
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

        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][2]);  // 법선용 VBO
        glBufferData(GL_ARRAY_BUFFER, models[i].normals.size() * sizeof(glm::vec3), models[i].normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 법선 할당
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
