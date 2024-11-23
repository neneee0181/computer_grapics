#include <iostream>
#include <vector> 

#include "shaderMaker.h"
#include "DefaultModel.h"
#include "KeyBoard.h"
#include "Camera.h"
#include "Light.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

//모델
vector<Model*> models;

int main(int argc, char** argv) {

    width = 800;
    height = 600;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 200);
    glutInitWindowSize(width, height);
    glutCreateWindow("opengl template v8");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    make_shaderProgram();

    initPhysics(); // Bullet 초기화 함수 호출

    DefaultModel* default_model1 = new DefaultModel("obj/board.obj", "board", "plane", glm::scale(glm::mat4(1.0f), glm::vec3(40.0, 40.0, 40.0)));
    models.push_back(default_model1);

    DefaultModel* default_model2 = new DefaultModel("obj/box_middle.obj", "box_middle", "box", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), glm::vec3(0.0, 0.0, 0.0)));
    models.push_back(default_model2);

    DefaultModel* default_model3 = new DefaultModel("obj/box_leg_right.obj", "box_leg_right", "box", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), glm::vec3(0.0, 0.0, 0.0)));
    models.push_back(default_model3);

    DefaultModel* default_model4 = new DefaultModel("obj/box_leg_left.obj", "box_leg_left", "box", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), glm::vec3(0.0, 0.0, 0.0)));
    models.push_back(default_model4);

    DefaultModel* default_model5 = new DefaultModel("obj/box_bottom.obj", "box_bottom", "box", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), glm::vec3(0.0, 0.0, 0.0)));
    models.push_back(default_model5);

    DefaultModel* default_model6 = new DefaultModel("obj/box_arm_right.obj", "box_arm_right", "box", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), glm::vec3(0.0, 0.0, 0.0)));
    models.push_back(default_model6);

    DefaultModel* default_model7 = new DefaultModel("obj/box_arm_left.obj", "box_arm_left", "box", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), glm::vec3(0.0, 0.0, 0.0)));
    models.push_back(default_model7);

    DefaultModel* light_model = new DefaultModel("obj/sphere.obj", "sphere", "sphere", glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2)), lightPos));
    light_model->material.Ka = lightColor;
    models.push_back(light_model);

    initializeModelsWithPhysics(models); // 모든 모델 Bullet world에 추가

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

    glClearColor(0.5, 0.5, 0.5, 1.0f);
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
   
    if (keyState['m']) {
        GLint lightEnabledLoc = glGetUniformLocation(shaderProgramID, "lightEnabled");
        glUniform1i(lightEnabledLoc, 1); // true: 1, false: 0으로 전달

    }
    else {
        GLint lightEnabledLoc = glGetUniformLocation(shaderProgramID, "lightEnabled");
        glUniform1i(lightEnabledLoc, 0); // true: 1, false: 0으로 전달
    }

    glEnable(GL_DEPTH_TEST);
    for (const auto& model : models) { // 실제 모델 draw
        if (model->name == "sphere") {
            if(!keyState['m'])
                model->draw(shaderProgramID, isKeyPressed_s);
            continue;
        }
        model->draw(shaderProgramID, isKeyPressed_s);
    }
    glDisable(GL_DEPTH_TEST);

    //for (const auto& model : models) { // 모델 bb draw
    //    model->draw_rigidBody(shaderProgramID);
    //}

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