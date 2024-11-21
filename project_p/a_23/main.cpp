#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector> 
#include <unordered_map>
#include <random>

#include "shaderMaker.h"
#include "LoadWall.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

//카메라
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 100);
glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

//키
unordered_map<char, bool> keyState;

void keyDown_s(const char& key) {
    keyState[key] = true;
}

void keyUp_s(const char& key) {
    keyState[key] = false;
}

bool isKeyPressed_s(const char& key) {
    return keyState[key];
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    width = w;
    height = h;
}

void keyUp(unsigned char key, int x, int y) {
    keyUp_s(key);
}

void keyDown(unsigned char key, int x, int y) {

    keyDown_s(key);

    switch (key)
    {
    case 'q':
        cout << " 프로그램 종료 " << endl;
        exit(0);
        break;
    default:
        break;
    }
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

int main(int argc, char** argv) {

    width = 800;
    height = 600;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("23번실습");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    make_shaderProgram();

    initPhysics(); // Bullet 초기화 함수 호출

    load_wall_obj(); // 벽 obj 불러옴

    InitBuffer();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(keySpecial);
    glutMainLoop();

    return 0;
}

GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glUseProgram(shaderProgramID);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error in glUseProgram: " << error << std::endl;
    }

    view = glm::mat4(1.0f);
    view = glm::lookAt(
        cameraPos,
        cameraDirection,
        cameraUp
    );
    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 300.0f);
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
    GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(-glm::vec3(900.0, -900.0, -900.0)));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(0.6f, 0.65f, 0.6f)));

    draw_wall(shaderProgramID, isKeyPressed_s);

    draw_rigidBody(shaderProgramID);

    glutSwapBuffers();
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;
    }
}

// 버퍼 초기화 함수
void InitBuffer() {
    //-----------------------------------------------------------------------------------------------------------
    initBuffer_wall();

   
}

//void AddModelBuffer(const Model& model) {
//    GLuint vao;
//    vector<GLuint> vbo(4);
//
//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
//
//    // VBO 생성
//    glGenBuffers(4, vbo.data());
//
//    // 정점 버퍼 설정
//    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
//    glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::vec3), model.vertices.data(), GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0에 정점 할당
//    glEnableVertexAttribArray(0);
//
//    // 법선 버퍼 설정
//    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
//    glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(glm::vec3), model.normals.data(), GL_STATIC_DRAW);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 법선 할당
//    glEnableVertexAttribArray(1);
//
//    // 텍스처 좌표 버퍼 설정
//    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
//    glBufferData(GL_ARRAY_BUFFER, model.texCoords.size() * sizeof(glm::vec2), model.texCoords.data(), GL_STATIC_DRAW);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2에 텍스처 좌표 할당
//    glEnableVertexAttribArray(2);
//
//    // 면 인덱스 데이터 (EBO) 설정
//    vector<unsigned int> indices;
//    for (const Face& face : model.faces) {
//        indices.push_back(face.v1);
//        indices.push_back(face.v2);
//        indices.push_back(face.v3);
//    }
//
//    GLuint ebo;
//    glGenBuffers(1, &ebo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
//
//    // 설정이 끝난 VAO와 VBO를 저장
//    vaos.push_back(vao);
//    vbos.push_back(vbo);
//
//    glBindVertexArray(0); // VAO unbind
//}