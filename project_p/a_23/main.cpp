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
#include "LoadBody.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

void collision_wall_check(Model& model);

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

float speed = 0.5f;

// 팔과 다리 상태를 추적할 구조체
struct LimbState {
    float angle = 0.0f;        // 현재 회전 각도
    bool movingForward = true; // 앞뒤로 움직이는 상태
    int status = 1;            // 현재 방향 (1: 앞으로, -1: 뒤로)

    LimbState(int status) {
        this->status = status;
    }
    LimbState() {}
};

// 팔과 다리의 상태를 저장할 맵
std::unordered_map<std::string, LimbState> limbStates = {
    {"left_arm", LimbState(1)},
    {"right_arm", LimbState(-1)},
    {"left_leg", LimbState(1)},
    {"right_leg", LimbState(-1)}
};

void move_arm_leg(Model& model) {
    const float rotationSpeed = 2.0f;    // 한 프레임당 회전 속도
    const float maxRotationAngle = 6.0f; // 최대 회전 각도 (10도)

    // 현재 모델의 상태 가져오기
    auto& state = limbStates[model.name];

    // 각도 계산: 앞뒤로 움직이게끔 처리
    state.angle += rotationSpeed * state.status; // `status`에 따라 각도 변화
    if (state.angle >= maxRotationAngle) {
        state.angle = maxRotationAngle;  // 최대 각도 제한
        state.status = -1;               // 방향 반전 (앞 -> 뒤)
    }
    else if (state.angle <= -maxRotationAngle) {
        state.angle = -maxRotationAngle; // 최소 각도 제한
        state.status = 1;                // 방향 반전 (뒤 -> 앞)
    }

    glm::vec3 pivot = Body::models[0].modelMatrix[3]; // 어깨의 로컬 좌표

    // 회전 행렬 생성
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::translate(rotationMatrix, pivot);                            // 피벗으로 이동
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(state.angle), glm::vec3(1.0f, 0.0f, 0.0f)); // x축 기준 회전
    rotationMatrix = glm::translate(rotationMatrix, -pivot);                           // 원래 위치로 복귀

    // 모델 매트릭스에 변환 적용
    model.modelMatrix = rotationMatrix * model.modelMatrix;

}

glm::mat4 bodyRo = glm::mat4(1.0f);
bool isGround = false;
int arrow = 0;

void timer(int value) {


    for (auto& model : Body::models) {

        if (!isGround) {
            glm::mat4 grav_matrix = glm::mat4(1.0f);
            grav_matrix = glm::translate(grav_matrix, glm::vec3(0.0, -speed, 0.0));
            model.modelMatrix = grav_matrix * model.modelMatrix;
        }
        
       
        if (arrow == 0) { // w
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, glm::vec3(0.0, 0.0, speed));
            model.modelMatrix = matrix * model.modelMatrix;
            bodyRo = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
        }
        if (arrow == 1) { //a
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, glm::vec3(-speed, 0.0, 0.0));
            model.modelMatrix = matrix * model.modelMatrix;
            bodyRo = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
        }
        if (arrow == 2) { // s
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, glm::vec3(0.0, 0.0, -speed));
            model.modelMatrix = matrix * model.modelMatrix;
            bodyRo = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        }
        if (arrow == 3) { // d
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, glm::vec3(speed, 0.0, 0.0));
            model.modelMatrix = matrix * model.modelMatrix;
            bodyRo = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        }

        collision_wall_check(model);

        if (model.name == "body") {
            if (arrow == 0) { // w
                if (model.modelMatrix[3].z > 20) {
                    arrow = 2;
                }
            }
            if (arrow == 2) { // s
                if (model.modelMatrix[3].z < -20) {
                    arrow = 0;
                }
            }
            if (arrow == 1) { // a
                if (model.modelMatrix[3].x < -20) {
                    arrow = 3;
                }
            }
            if (arrow == 3) { // a
                if (model.modelMatrix[3].x > 20) {
                    arrow = 1;
                }
            }
        }
    }

    for (auto& model : Body::models) {
        if (model.name == "right_arm" || model.name == "left_arm" || model.name == "right_leg" || model.name == "left_leg")
            move_arm_leg(model);
    }




    UpdateRigidBodyTransforms(Body::models, bodyRo);
    
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// 점프 상태를 관리하기 위한 구조체 추가
struct JumpState {
    bool jumping;       // 점프 중인지 여부
    float velocity;     // 현재 점프 속도
    float gravity;      // 중력 가속도
    float maxHeight;    // 최대 점프 높이
};

JumpState jumpState;
const float floorHeight = -18.0f;

void jumpTimer(int value) {
   // 점프 중인지 확인
    if (!jumpState.jumping) {
        return; // 점프 중이 아니면 타이머 종료
    }

    // 현재 Y 좌표 계산 (몸체 기준)
    float currentY = Body::models[0].modelMatrix[3].y;

    // 속도 업데이트 (중력 적용)
    jumpState.velocity -= jumpState.gravity;

    // 새로운 Y 좌표 계산
    float newY = currentY + jumpState.velocity;

    // 충돌 감지 (바닥과의 충돌)
    bool hitGround = false;
    for (auto& wall : Wall::models) {
        if (wall.name != "bottom") continue;
        if (!wall.rigidBody || !Body::models[0].rigidBody) continue;

        CustomContactResultCallback resultCallback;
        dynamicsWorld->contactPairTest(Body::models[1].rigidBody, wall.rigidBody, resultCallback);
        if (resultCallback.hitDetected && newY <= floorHeight) { // 바닥 높이에 닿았는지 확인
            hitGround = true;
            break;
        }
    }

    if (hitGround) {
        // 착지 처리
        jumpState.jumping = false;    // 점프 종료
        jumpState.velocity = 0.0f;    // 속도 초기화
        newY = floorHeight;           // 바닥 높이에 정렬
    }

    // 모든 모델 이동 처리 (캐릭터의 모든 파츠를 동기화)
    for (auto& body : Body::models) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(0.0f, newY - currentY, 0.0f));
        body.modelMatrix = matrix * body.modelMatrix;
    }

    // 점프가 여전히 진행 중이라면 타이머를 다시 설정
    if (jumpState.jumping) {
        glutTimerFunc(16, jumpTimer, 0);
    }

}
int bari_s = 0;

void bariTimer2(int value) {
    for (auto& model : Wall::models) {
        if (model.name == "barigate2") {

            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, glm::vec3(0.0, -0.01, 0.0));
            model.modelMatrix = matrix * model.modelMatrix;
            UpdateRigidBodyTransform(model, bodyRo);
        }
    }
    glutPostRedisplay();
    if (value < 400) {
        glutTimerFunc(16, bariTimer2, ++value);
    }
    else {
        bari_s = 2;
    }
}

void bariTimer1(int value) {
    for (auto& model : Wall::models) {
        if (model.name == "barigate1" || model.name == "barigate2") {

            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, glm::vec3(0.0, -0.01, 0.0));
            model.modelMatrix = matrix * model.modelMatrix;
            UpdateRigidBodyTransform(model, bodyRo);
        }
    }
    glutPostRedisplay();
    if (value < 400) {
        glutTimerFunc(16, bariTimer1, ++value);
    }
    else {
        bari_s = 4;
    }
}

void collision_wall_check(Model& model) {
    for (auto& bodyModel : Body::models) {
        for (auto& wallModel : Wall::models) {
            if (bodyModel.rigidBody && wallModel.rigidBody) {
                CustomContactResultCallback resultCallback;
                dynamicsWorld->contactPairTest(bodyModel.rigidBody, wallModel.rigidBody, resultCallback);
                if (resultCallback.hitDetected) {

                    if (wallModel.name == "bottom" || wallModel.name == "barigate1" || wallModel.name == "barigate2") {
                        // 중력 멈춤 상태로 설정
                        isGround = true;

                        // 위치 조정 (필요시 모델이 바닥 위에 정확히 위치하도록)
                        glm::mat4 grav_matrix = glm::mat4(1.0f);
                        grav_matrix = glm::translate(grav_matrix, glm::vec3(0.0, speed, 0.0));
                        model.modelMatrix = grav_matrix * model.modelMatrix;
                        if (wallModel.name == "barigate2" && bari_s == 0) {
                            bari_s = 1;
                            glutTimerFunc(0, bariTimer2, 0);
                        }
                        if (wallModel.name == "barigate1" && bari_s == 2) {
                            bari_s = 3;
                            glutTimerFunc(0, bariTimer1, 0);
                        }
                        
                    }
                    //else {
                    //    if (arrow == 0) { //w
                    //        glm::mat4 matrix = glm::mat4(1.0f);
                    //        matrix = glm::translate(matrix, glm::vec3(speed, 0.0, 0.0));
                    //        model.modelMatrix = matrix * model.modelMatrix;
                    //        arrow = 2;
                    //    }
                    //    if (arrow == 1) { //a
                    //        glm::mat4 matrix = glm::mat4(1.0f);
                    //        matrix = glm::translate(matrix, glm::vec3(0.0, 0.0, -speed));
                    //        model.modelMatrix = matrix * model.modelMatrix;
                    //    }
                    //    if (arrow == 2) { // s
                    //        glm::mat4 matrix = glm::mat4(1.0f);
                    //        matrix = glm::translate(matrix, glm::vec3(0.0, 0.0, speed));
                    //        model.modelMatrix = matrix * model.modelMatrix;
                    //    }
                    //    if (arrow == 3) { //d
                    //        glm::mat4 matrix = glm::mat4(1.0f);
                    //        matrix = glm::translate(matrix, glm::vec3(-speed, 0.0, 0.0));
                    //        model.modelMatrix = matrix * model.modelMatrix;
                    //    }
                    //}
                }
            }
        }
    }
}

void openDoorTimer(int value) {

    for (auto& model : Wall::models) {
        if (model.name == "front1") {
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, glm::vec3(-0.1, 0.0, 0.0));
            model.modelMatrix = matrix * model.modelMatrix;
        }
        else if (model.name == "front2") {
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, glm::vec3(0.1, 0.0, 0.0));
            model.modelMatrix = matrix * model.modelMatrix;
        }
    }

    glutPostRedisplay();
    if (value < 250) {
        glutTimerFunc(16, openDoorTimer, ++value);
    }
}

void keyDown(unsigned char key, int x, int y) {

    keyDown_s(key);

    switch (key)
    {
    case 'w':
        arrow = 0;
        break;
    case 's':
        arrow = 2;
        break;
    case 'a':
        arrow = 1;
        break;
    case 'd':
        arrow = 3;
        break;
    case 'j':
        // 점프 초기화
        jumpState = { true, 1.0f, 0.05f, 10.0f }; // 초기 속도와 중력 설정
        glutTimerFunc(0, jumpTimer, 0);
        break;
    case 'o':
        glutTimerFunc(0, openDoorTimer, 0);
        glutTimerFunc(0, timer, 0);
        break;
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

   
    Body::load_obj(); // 몸 obj 불러옴
    Wall::load_obj(); // 벽 obj 불러옴

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

    glEnable(GL_DEPTH_TEST);
    Wall::draw(shaderProgramID, isKeyPressed_s);
    Body::draw(shaderProgramID, isKeyPressed_s, bodyRo);

    glDisable(GL_DEPTH_TEST);

    Body::draw_rigidBody(shaderProgramID);
    Wall::draw_rigidBody(shaderProgramID);

    glutSwapBuffers();

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;
    }
}

// 버퍼 초기화 함수
void InitBuffer() {
    //-----------------------------------------------------------------------------------------------------------
    Wall::initBuffer();
    Body::initBuffer();
}