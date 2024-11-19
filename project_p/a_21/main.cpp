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

#include "BulletPhysics.h"
#include "LoadObj.h"
#include "shaderMaker.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void make_model();

vector<Model> models;
vector<GLuint> vaos;
vector<vector<GLuint>> vbos;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 100);
glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

std::unordered_map<char, bool> keyState;

float speed = 0.2f;
float radi_speed = 0.1f;
float radi_l = 0.1;
int radi_status = 1;

void checkBoxCollisions(std::vector<Model>& models) {
    for (size_t i = 0; i < models.size(); ++i) {
        if (models[i].type != "box") continue;

        for (size_t j = 0; j < models.size(); ++j) {
            if (models[j].type != "body") continue;


            if (models[i].rigidBody == nullptr || models[j].rigidBody == nullptr)
                continue;

            // 충돌 검사
            CustomContactResultCallback resultCallback;
            dynamicsWorld->contactPairTest(models[i].rigidBody, models[j].rigidBody, resultCallback);

            glm::mat4 matrixRollBack = glm::mat4(1.0f);
            // 충돌 여부 확인
            if (resultCallback.hitDetected) {
                if (models[i].name == "box_left" && models[j].name == "body") {
                    //좌측
                    keyState['a'] = false;
                    keyState['d'] = true;
                    for (auto& model : models) {
                        if (model.type == "body") {
                            model.rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                    }
                }
                if (models[i].name == "box_right" && models[j].name == "body") {
                    //우측 박스
                    keyState['d'] = false;
                    keyState['a'] = true;
                    for (auto& model : models) {
                        if (model.type == "body") {
                            model.rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                    }
                }
                if (models[i].name == "box_back" && models[j].name == "body") {
                    // 뒤쪽 박스
                    keyState['s'] = false;
                    keyState['w'] = true;
                    for (auto& model : models) {
                        if (model.type == "body") {
                            model.rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
                        }
                    }
                }
            }
        }
    }
}

// Bullet Physics 초기화
void InitPhysics() {
    initPhysics(); // Bullet 초기화 함수 호출
    initializeModelsWithPhysics(models); // 모든 모델을 물리 세계에 추가
}

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

void openTimer(int value) {

    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(speed, 0.0, 0.0));
    models[9].modelMatrix = matrix * models[9].modelMatrix;

    matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(-speed, 0.0, 0.0));
    models[10].modelMatrix = matrix * models[10].modelMatrix;

    glutPostRedisplay();
    if (value <= 98) {
        glutTimerFunc(16, openTimer, ++value);
    }
}

void keyDown(unsigned char key, int x, int y) {

    switch (key)
    {
    case 'q':
        cout << " 프로그램 종료 " << endl;
        exit(0);
        break;
    case '+':
        speed += 0.01f;
        radi_speed += 0.01f;
        break;
    case '-':
        speed -= 0.01f;
        radi_speed -= 0.01f;
        break;
    case 'i':
        models.clear();
        make_model();
        InitBuffer();
        break;
    case 'o':
    case 'O':
        glutTimerFunc(0, openTimer, 0);
        break;
    case 'w':
    case 'a':
    case 's':
    case 'd':
        if (!keyState[key]) {
            for (auto& model : models) {
                if (model.type == "body") {
                    switch (key)
                    {
                    case 'w':
                        model.rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
                        break;
                    case 'a':
                        /*matrix = glm::translate(matrix, glm::vec3(0.0, 0.0, 0.0));
                        matrix = glm::rotate(matrix, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
                        model.modelMatrix = matrix * model.modelMatrix;*/
                        model.rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
                        break;
                    case 's':
                        model.rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                        break;
                    case 'd':
                        model.rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        break;
    default:
        break;
    }

    keyDown_s(key);

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

void timer(int value) {
    glm::mat4 matrix = glm::mat4(1.0f);
    glm::mat4 angle = glm::mat4(1.0f);

    // 이동 처리 (w, a, s, d 키에 따라)
    if (keyState['w']) {
        matrix = glm::translate(matrix, glm::vec3(0.0, 0.0, speed));
    }
    else if (keyState['a']) {
        matrix = glm::translate(matrix, glm::vec3(-speed, 0.0, 0.0));
    }
    else if (keyState['d']) {
        matrix = glm::translate(matrix, glm::vec3(speed, 0.0, 0.0));
    }
    else if (keyState['s']) {
        matrix = glm::translate(matrix, glm::vec3(0.0, 0.0, -speed));
    }

    if (radi_status == 1) {
        radi_l -= 0.01;
    }
    else {
        radi_l += 0.01;
    }
    if (radi_l <= 0) {
        radi_status = -1;
    }
    if (radi_l >= 0.1) {
        radi_status = 1;
    }

    if (keyState['w'] || keyState['s']) {
        //왼쪽다리
        angle = glm::translate(angle, glm::vec3(0,0,0));
        angle = glm::translate(angle, glm::vec3(0.0, 10.0, 0.0));
        angle = glm::rotate(angle, radi_speed * radi_status, glm::vec3(1.0, 0.0, 0.0));
        angle = glm::translate(angle, glm::vec3(0.0, -10.0, 0.0));
        angle = glm::translate(angle, -glm::vec3(0, 0, 0));
        models[3].rotationMatrix = angle * models[3].rotationMatrix;

        //우측다리
        angle = glm::mat4(1.0f);
        angle = glm::translate(angle, glm::vec3(0, 0, 0));
        angle = glm::translate(angle, glm::vec3(0.0, 10.0, 0.0));
        angle = glm::rotate(angle, radi_speed * -radi_status, glm::vec3(1.0, 0.0, 0.0));
        angle = glm::translate(angle, glm::vec3(0.0, -10.0, 0.0));
        angle = glm::translate(angle, -glm::vec3(0, 0, 0));
        models[4].rotationMatrix = angle * models[4].rotationMatrix;

        angle = glm::mat4(1.0f);
        angle = glm::translate(angle, glm::vec3(0, 0, 0));
        angle = glm::translate(angle, glm::vec3(0.0, 10.0, 0.0));
        angle = glm::rotate(angle, radi_speed * radi_status, glm::vec3(1.0, 0.0, 0.0));
        angle = glm::translate(angle, glm::vec3(0.0, -10.0, 0.0));
        angle = glm::translate(angle, -glm::vec3(0, 0, 0));
        models[2].rotationMatrix = angle * models[2].rotationMatrix;

        angle = glm::mat4(1.0f);
        angle = glm::translate(angle, glm::vec3(0, 0, 0));
        angle = glm::translate(angle, glm::vec3(0.0, 10.0, 0.0));
        angle = glm::rotate(angle, radi_speed * -radi_status, glm::vec3(1.0, 0.0, 0.0));
        angle = glm::translate(angle, glm::vec3(0.0, -10.0, 0.0));
        angle = glm::translate(angle, -glm::vec3(0, 0, 0));
        models[1].rotationMatrix = angle * models[1].rotationMatrix;
    }
    else if (keyState['a'] || keyState['d']) {
        //왼쪽다리
        angle = glm::translate(angle, glm::vec3(0, 0, 0));
        angle = glm::translate(angle, glm::vec3(0.0, 20.0, 0.0));
        angle = glm::rotate(angle, radi_speed * radi_status, glm::vec3(0.0, 0.0, 1.0));
        angle = glm::translate(angle, glm::vec3(0.0, -20.0, 0.0));
        angle = glm::translate(angle, -glm::vec3(0, 0, 0));
        models[3].rotationMatrix = angle * models[3].rotationMatrix;

        //우측다리
        angle = glm::mat4(1.0f);
        angle = glm::translate(angle, glm::vec3(0, 0, 0));
        angle = glm::translate(angle, glm::vec3(0.0, 20.0, 0.0));
        angle = glm::rotate(angle, radi_speed * -radi_status, glm::vec3(0.0, 0.0, 1.0));
        angle = glm::translate(angle, glm::vec3(0.0, -20.0, 0.0));
        angle = glm::translate(angle, -glm::vec3(0, 0, 0));
        models[4].rotationMatrix = angle * models[4].rotationMatrix;

        angle = glm::mat4(1.0f);
        angle = glm::translate(angle, glm::vec3(0, 0, 0));
        angle = glm::translate(angle, glm::vec3(0.0, 20.0, 0.0));
        angle = glm::rotate(angle, radi_speed * radi_status, glm::vec3(0.0, 0.0, 1.0));
        angle = glm::translate(angle, glm::vec3(0.0, -20.0, 0.0));
        angle = glm::translate(angle, -glm::vec3(0, 0, 0));
        models[2].rotationMatrix = angle * models[2].rotationMatrix;

        angle = glm::mat4(1.0f);
        angle = glm::translate(angle, glm::vec3(0, 0, 0));
        angle = glm::translate(angle, glm::vec3(0.0, 20.0, 0.0));
        angle = glm::rotate(angle, radi_speed * -radi_status, glm::vec3(0.0, 0.0, 1.0));
        angle = glm::translate(angle, glm::vec3(0.0, -20.0, 0.0));
        angle = glm::translate(angle, -glm::vec3(0, 0, 0));
        models[1].rotationMatrix = angle * models[1].rotationMatrix;
    }

    for (auto& model : models) {
        if (model.type == "body") {
            model.modelMatrix = matrix * model.modelMatrix;
        }
        UpdateRigidBodyTransform(model);
    }

    checkBoxCollisions(models);

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void make_model() {

    Model model_box_front1, model_box_front2, model_box_back, model_box_top, model_box_bottom, model_box_left, model_box_right,
        model_left_a, model_left_l, model_right_a, model_right_l, model_body, model_bbox;

    read_obj_file("obj/box_back.obj", model_box_back, "box_back", "box");
    read_obj_file("obj/box_bottom.obj", model_box_bottom, "box_bottom", "box");
    read_obj_file("obj/box_right.obj", model_box_right, "box_right", "box");
    read_obj_file("obj/box_left.obj", model_box_left, "box_left", "box");
    read_obj_file("obj/box_front_1.obj", model_box_front1, "box_front", "box");
    read_obj_file("obj/box_front_2.obj", model_box_front2, "box_front", "box");
    read_obj_file("obj/box_top.obj", model_box_top, "box_top", "box");

    read_obj_file("obj/body.obj", model_body, "body", "body");
    read_obj_file("obj/left_a.obj", model_left_a, "left_a", "body");
    read_obj_file("obj/right_a.obj", model_right_a, "right_a", "body");
    read_obj_file("obj/left_l.obj", model_left_l, "left_l", "body");
    read_obj_file("obj/right_l.obj", model_right_l, "right_l", "body");

    read_obj_file("obj/box3.obj", model_bbox, "bbox", "bbox");


    glm::mat4 matrix_body = glm::mat4(1.0f);
    matrix_body = glm::translate(matrix_body, glm::vec3(0.0, -20.0, 0.0));
    matrix_body = glm::scale(matrix_body, glm::vec3(0.4, 0.4, 0.4));
    model_body.modelMatrix = matrix_body * model_body.modelMatrix;
    model_left_a.modelMatrix = matrix_body * model_left_a.modelMatrix;
    model_right_a.modelMatrix = matrix_body * model_right_a.modelMatrix;
    model_left_l.modelMatrix = matrix_body * model_left_l.modelMatrix;
    model_right_l.modelMatrix = matrix_body * model_right_l.modelMatrix;

    models.push_back(model_body);
    models.push_back(model_left_a);
    models.push_back(model_right_a);
    models.push_back(model_left_l);
    models.push_back(model_right_l);

    glm::mat4 matrix_box = glm::mat4(1.0f);
    matrix_box = glm::translate(matrix_box, glm::vec3(0.0, -20.0, 0.0));
    model_box_back.modelMatrix = matrix_box * model_box_back.modelMatrix;
    model_box_bottom.modelMatrix = matrix_box * model_box_bottom.modelMatrix;
    model_box_right.modelMatrix = matrix_box * model_box_right.modelMatrix;
    model_box_left.modelMatrix = matrix_box * model_box_left.modelMatrix;
    model_box_front1.modelMatrix = matrix_box * model_box_front1.modelMatrix;
    model_box_front2.modelMatrix = matrix_box * model_box_front2.modelMatrix;
    model_box_top.modelMatrix = matrix_box * model_box_top.modelMatrix;

    models.push_back(model_box_back);
    models.push_back(model_box_bottom);
    models.push_back(model_box_right);
    models.push_back(model_box_left);
    models.push_back(model_box_front1); //9
    models.push_back(model_box_front2); //10
    models.push_back(model_box_top);

    glm::mat4 matrix_bbox = glm::mat4(1.0f);
    matrix_bbox = glm::translate(matrix_bbox, glm::vec3(0.0, 0.0, 0.0));
    model_bbox.modelMatrix = matrix_bbox * model_bbox.modelMatrix;

    models.push_back(model_bbox);

    for (auto& model : models) {
        //alignModelToOrigin(model); // 모델의 중심을 정렬
        addModelToPhysicsWorld(model);
        UpdateRigidBodyTransform(model);  // 초기 위치 동기화
    }

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
    InitPhysics();
    make_model();

    for (auto& model : models) {
        if (!model.material.map_Kd.empty()) {
            model.material.textureID = loadTexture(model.material.map_Kd);
            model.material.hasTexture = true; // 텍스처가 있음을 표시
        }
        else {
            model.material.hasTexture = false;
        }
    }

    InitBuffer();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(keySpecial);
    glutTimerFunc(0, timer, 0);
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

    glEnable(GL_DEPTH_TEST);

    GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");

    for (size_t i = 0; i < models.size(); ++i) {

        if (models[i].model_status) {
            glBindVertexArray(vaos[i]);
            glLineWidth(1.0f);
            if (models[i].material.hasTexture) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, models[i].material.textureID);
                glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
                glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 1);
            }
            else {
                glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 0);

                GLint KaLoc = glGetUniformLocation(shaderProgramID, "Ka");
                GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
                GLint KsLoc = glGetUniformLocation(shaderProgramID, "Ks");
                GLint NsLoc = glGetUniformLocation(shaderProgramID, "Ns");

                glUniform3fv(KaLoc, 1, glm::value_ptr(models[i].material.Ka));
                glUniform3fv(KdLoc, 1, glm::value_ptr(models[i].material.Kd));
                glUniform3fv(KsLoc, 1, glm::value_ptr(models[i].material.Ks));
                glUniform1f(NsLoc, models[i].material.Ns);
            }

            if (models[i].name == "body") {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix * models[i].rotationMatrix));
            }
            else if (models[i].name == "left_a" || models[i].name == "right_a"|| models[i].name == "left_l" || models[i].name == "right_l") {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix * models[i].rotationMatrix));
            }
            else if (models[i].type == "box") {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            }
            else {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            }
           /* else if (models[i].type == "bbox") {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));
            }*/

            glUniform1i(modelStatus, 0);
            if (isKeyPressed_s('1'))
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
        }
    }


    // 충돌 박스 렌더링
    glDisable(GL_DEPTH_TEST); // 충돌 박스가 모든 객체 위에 그려지도록 설정
    for (const auto& model : models) {
        if (model.rigidBody) {
            RenderCollisionBox(model); // 충돌 박스 그리기
        }
    }
    glEnable(GL_DEPTH_TEST); // 깊이 테스트 다시 활성화

    glutSwapBuffers();
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;
    }
}

// 버퍼 초기화 함수
void InitBuffer() {
    //-----------------------------------------------------------------------------------------------------------
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

        // 텍스처 좌표 VBO 설정
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][3]);  // 4번째 VBO가 텍스처 좌표용이라고 가정
        glBufferData(GL_ARRAY_BUFFER, models[i].texCoords.size() * sizeof(glm::vec2), models[i].texCoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2에 텍스처 좌표 할당
        glEnableVertexAttribArray(2);

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

void AddModelBuffer(const Model& model) {
    GLuint vao;
    vector<GLuint> vbo(4);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // VBO 생성
    glGenBuffers(4, vbo.data());

    // 정점 버퍼 설정
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::vec3), model.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 0에 정점 할당
    glEnableVertexAttribArray(0);

    // 법선 버퍼 설정
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(glm::vec3), model.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // location 1에 법선 할당
    glEnableVertexAttribArray(1);

    // 텍스처 좌표 버퍼 설정
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, model.texCoords.size() * sizeof(glm::vec2), model.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);  // location 2에 텍스처 좌표 할당
    glEnableVertexAttribArray(2);

    // 면 인덱스 데이터 (EBO) 설정
    vector<unsigned int> indices;
    for (const Face& face : model.faces) {
        indices.push_back(face.v1);
        indices.push_back(face.v2);
        indices.push_back(face.v3);
    }

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 설정이 끝난 VAO와 VBO를 저장
    vaos.push_back(vao);
    vbos.push_back(vbo);

    glBindVertexArray(0); // VAO unbind
}