#include <iostream>
#include <vector> 
#include <random>

#include "shaderMaker.h"
#include "DefaultModel.h"
#include "SierpinskiModel.h"
#include "KeyBoard.h"
#include "Camera.h"
#include "Light.h"
#include "CustomTimer.h"
#include "Root.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

int main(int argc, char** argv) {

    width = 800;
    height = 600;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 200);
    glutInitWindowSize(width, height);
    glutCreateWindow("27번");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    make_shaderProgram();

    initPhysics(); // Bullet 초기화 함수 호출

    DefaultModel* board_model = new DefaultModel("obj/board.obj", "board", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0))); // 실제 모델 가져오기
    board_model->material.Ka = glm::vec3(0.4, 0.4, 0.4);
    models.push_back(board_model);

    DefaultModel* pira_model = new DefaultModel("obj/piramid.obj", "pira", "sphere", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.05, 0.043, 0.05)), glm::vec3(0.0, 5.0, 0.0)));
    pira_model->material.Ka = glm::vec3(0.1,0.4,0.7);
    pira_model->rigid_status = false;
    models.push_back(pira_model);

    DefaultModel* planet1 = new DefaultModel("obj/sphere.obj", "planet1", "sphere", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.6, 0.4, 0.0)), glm::vec3(0.01, 0.01, 0.01)));
    planet1->material.Ka = glm::vec3(1.0, 0.0, 0.0);
    planet1->rigid_status = false;
    models.push_back(planet1);

    DefaultModel* planet2 = new DefaultModel("obj/sphere.obj", "planet2", "sphere", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.6, 0.7, 0.4)), glm::vec3(0.01, 0.01, 0.01)));
    planet2->material.Ka = glm::vec3(0.0, 1.0, 0.0);
    planet2->rigid_status = false;
    models.push_back(planet2);

    DefaultModel* planet3 = new DefaultModel("obj/sphere.obj", "planet3", "sphere", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.6, 0.5, -0.4)), glm::vec3(0.01, 0.01, 0.01)));
    planet3->material.Ka = glm::vec3(0.0, 0.0, 1.0);
    planet3->rigid_status = false;
    models.push_back(planet3);

    DefaultModel* light_sphere = new DefaultModel("obj/sphere.obj", "light_m", "sphere", glm::scale(glm::translate(glm::mat4(1.0f), lightPos), glm::vec3(0.003, 0.003, 0.003)));
    light_sphere->material.Ka = lightColor;
    light_sphere->rigid_status = false; 
    models.push_back(light_sphere);

    std::uniform_real_distribution<> snow_speed_dis(0.005, 0.01);

    DefaultModel* snow = new DefaultModel("obj/Snowflake.obj", "snow", "sphere", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(snow_location_dis_x(gen), 1.5, snow_location_dis_x(gen))), glm::vec3(0.1, 0.1, 0.1)));
    snow->material.Ka = glm::vec3(1.0, 1.0, 1.0);
    snow->speed = snow_speed_dis(gen);
    models.push_back(snow);

    for (int i = 0; i < 4; ++i) {
        DefaultModel* snow_ = new DefaultModel(*snow); // snow 객체를 복사하여 새로운 객체 생성
        snow_->matrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(snow_location_dis_x(gen), 1.5, snow_location_dis_x(gen))), glm::vec3(0.1, 0.1, 0.1));
        snow->speed = snow_speed_dis(gen);
        models.push_back(snow_);
    }

    SierpinskiModel* sp_model = new SierpinskiModel(1);
    glm::mat4 sp_m = glm::mat4(1.0f);
    sp_m = glm::scale(sp_m, glm::vec3(0.5, 0.5, 0.5));
    sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.0, 0.5));
    sp_m = glm::rotate(sp_m, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
    sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.5, 0.0));
    sp_model->matrix = sp_m * sp_model->matrix;
    models.push_back(sp_model);

    initializeModelsWithPhysics(models); // 모든 모델 Bullet world에 추가

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

    glClearColor(0.0, 0.0, 0.0, 1.0f);
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
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
   
    glEnable(GL_DEPTH_TEST);
    for (const auto& model : models) { // 실제 모델 draw
        model->draw(shaderProgramID, isKeyPressed_s);
    }
    glDisable(GL_DEPTH_TEST);

    for (const auto& model : models) { // 모델 bb draw
        model->draw_rigidBody(shaderProgramID);
    }

    glutSwapBuffers();

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cout << "OpenGL error: " << err << endl;
    }
}

// 버퍼 초기화 함수
void InitBuffer() {
    //-----------------------------------------------------------------------------------------------------------
    for (const auto& model : models) { // 모든 모델 initBuffer
        model->initBuffer();
    }
}