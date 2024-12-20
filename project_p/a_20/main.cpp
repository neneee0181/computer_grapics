#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <vector> 
#include <unordered_map>

#include "LoadObj.h"
#include "shaderMaker.h"

using namespace std;

void InitBuffer();
void reset_All();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

vector<Model> models;
vector<GLuint> vaos;
vector<vector<GLuint>> vbos;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 6.0);
glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

struct KeyState {
    unsigned key;
    bool status = false;
    bool key_s = false;
};

KeyState keyState;

const GLfloat x[2][3] = {
    {-10.0f,0.0f, 0.0f},
    {10.0f,0.0f, 0.0f}
};

const glm::vec3 color_xyz[3]= {
    glm::vec3(1.0,0.0,0.0),
    glm::vec3(0.0,1.0,0.0),
    glm::vec3(0.0,0.0,1.0)
};

const GLfloat y[2][3] = {
    {0.0f,-10.0f, 0.0f},
    {0.0f, 10.0f, 0.0f}
};

const GLfloat z[2][3] = {
    {0.0f, 0.0f, -10.0f},
    {0.0f, 0.0f, 10.0f}
};

GLuint xyz_VBO[6], xyz_VAO[3];

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    width = w;
    height = h; 
}

void timer(int value) {

    float speed = 0.05;
    float r_speed = 0.5;

    for (int i = 1; i < models.size(); ++i) {
        
        switch (keyState.key)
        {
        case 'b':
        {
            if (models[i].modelMatrix[3].x >= models[0].modelMatrix[0].x - 0.35)
                keyState.status = false;
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, glm::vec3(speed, 0.0, 0.0));
            models[i].modelMatrix = matrix * models[i].modelMatrix;
            break;
        }
        case 'B':
        {
            if (models[i].modelMatrix[3].x <= -models[0].modelMatrix[0].x + 0.35)
                keyState.status = false;
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, glm::vec3(-speed, 0.0, 0.0));
            models[i].modelMatrix = matrix * models[i].modelMatrix;
            break;
        }
        case 'm':
        {
            glm::mat4 matrix = glm::mat4(1.0f);
            if (models[i].name == "mid_b") {
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(r_speed), glm::vec3(0.0, 1.0, 0.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));

                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }

            if (models[i].name == "right_a_b" || models[i].name == "left_a_b") {
                matrix = glm::mat4(1.0f);
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(r_speed), glm::vec3(0.0, 1.0, 0.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));

                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }
           
            break;
        }
        case 'M':
        {
            glm::mat4 matrix = glm::mat4(1.0f);
            if (models[i].name == "mid_b") {
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(-r_speed), glm::vec3(0.0, 1.0, 0.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));

                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }

            if (models[i].name == "right_a_b" || models[i].name == "left_a_b") {
                matrix = glm::mat4(1.0f);
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(-r_speed), glm::vec3(0.0, 1.0, 0.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));

                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }

            break;
        }
        case 'f':
        {
            keyState.key_s = true;
            glm::mat4 matrix = glm::mat4(1.0f);
            if (models[i].name == "right_l_b") {
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(r_speed), glm::vec3(0.0, 1.0, 0.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }

            matrix = glm::mat4(1.0f);
            if (models[i].name == "left_l_b") {
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(-r_speed), glm::vec3(0.0, 1.0, 0.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }
            break;
        }
        case 'F':
        {
            keyState.key_s = true;
            glm::mat4 matrix = glm::mat4(1.0f);
            if (models[i].name == "left_l_b") {
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(r_speed), glm::vec3(0.0, 1.0, 0.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }

            matrix = glm::mat4(1.0f);
            if (models[i].name == "right_l_b") {
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(-r_speed), glm::vec3(0.0, 1.0, 0.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }
            break;
        }
        case 'e':
        {
            glm::mat4 matrix = glm::mat4(1.0f);
            if (models[i].name == "right_l_b") {
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(r_speed), glm::vec3(0.0, 1.0, 0.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = matrix * models[i].modelMatrix;

                float le = glm::length(glm::vec3(models[i].modelMatrix[0]) - glm::vec3(glm::mat4(1.0f)[0]));
                // 현재 위치와 초기 위치 비교
                if (le < 0.95001f) {
                    keyState.key = 'w';
                }

            }

            matrix = glm::mat4(1.0f);
            if (models[i].name == "left_l_b") {
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(-r_speed), glm::vec3(0.0, 1.0, 0.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }

            break;
        }
        case 'E':
        {
            if (models[i].name == "right_l_b") {
                models[i].modelMatrix = glm::translate(models[i].modelMatrix, glm::vec3(0.01, 0.0, 0.0));

                float le = glm::length(glm::vec3(models[5].modelMatrix[3]) - glm::vec3(models[6].modelMatrix[3]));
                cout << le << endl;
                // 현재 위치와 초기 위치 비교
                if (le < 0.0010f) {
                    keyState.status = false;
                }
            }

            if (models[i].name == "left_l_b") {
                models[i].modelMatrix = glm::translate(models[i].modelMatrix, glm::vec3(-0.01, 0.0, 0.0));

            }

            break;
        }
        case 't':
        {
            glm::mat4 matrix = glm::mat4(1.0f);

            if (models[i].name == "right_a_b") {
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(r_speed), glm::vec3(0.0, 0.0, 1.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }

            matrix = glm::mat4(1.0f);

            if (models[i].name == "left_a_b") {
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(-r_speed), glm::vec3(0.0, 0.0, 1.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }
            break;
        }
        case 'T':
        {
            glm::mat4 matrix = glm::mat4(1.0f);

            if (models[i].name == "right_a_b") {
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(-r_speed), glm::vec3(0.0, 0.0, 1.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }

            matrix = glm::mat4(1.0f);

            if (models[i].name == "left_a_b") {
                matrix = glm::translate(matrix, glm::vec3(models[1].modelMatrix[3]));
                matrix = glm::rotate(matrix, glm::radians(r_speed), glm::vec3(0.0, 0.0, 1.0));
                matrix = glm::translate(matrix, glm::vec3(-models[1].modelMatrix[3]));
                models[i].modelMatrix = matrix * models[i].modelMatrix;
            }
            break;
        }
        case 'w':
        {
            if (models[i].name == "right_l_b") {
                models[i].modelMatrix = glm::translate(models[i].modelMatrix, glm::vec3(-0.01, 0.0, 0.0));

            }

            if (models[i].name == "left_l_b") {
                models[i].modelMatrix = glm::translate(models[i].modelMatrix, glm::vec3(0.01, 0.0, 0.0));

            }

            float le = glm::length(glm::vec3(models[5].modelMatrix[3]) - glm::vec3(models[6].modelMatrix[3]));

            if (le >= 0.25) {
                keyState.status = false;
            }
            break;
        }
        case 'r':
        {
            // 카메라가 공전할 중심점 (예: 원점)
            glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

            // 카메라 위치와 중심점 사이의 오프셋
            glm::vec3 offset = cameraPos - center;

            // 공전 각도
            float orbitSpeed = glm::radians(r_speed); // r_speed를 라디안으로 변환

            // y축을 기준으로 offset 회전
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), orbitSpeed, glm::vec3(0.0, 1.0, 0.0));
            offset = glm::vec3(rotationMatrix * glm::vec4(offset, 1.0f));

            // 새로운 카메라 위치는 중심점 + 회전된 offset
            cameraPos = center + offset;

            // 카메라가 항상 중심점을 바라보도록 설정
            cameraDirection = center;

            break;
        }
        case 'R':
        {
            // 카메라가 공전할 중심점 (예: 원점)
            glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

            // 카메라 위치와 중심점 사이의 오프셋
            glm::vec3 offset = cameraPos - center;

            // 공전 각도
            float orbitSpeed = glm::radians(-r_speed); // r_speed를 라디안으로 변환

            // y축을 기준으로 offset 회전
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), orbitSpeed, glm::vec3(0.0, 1.0, 0.0));
            offset = glm::vec3(rotationMatrix * glm::vec4(offset, 1.0f));

            // 새로운 카메라 위치는 중심점 + 회전된 offset
            cameraPos = center + offset;

            // 카메라가 항상 중심점을 바라보도록 설정
            cameraDirection = center;

            break;
        }
        default:
            break;
        }
    }

    glutPostRedisplay();
    if (keyState.status)
        glutTimerFunc(16, timer, 0);
}

void keyBoard(unsigned char key, int x, int y) {

    keyState.key = key;
    keyState.status = !keyState.status;

    switch (keyState.key)
    {
    case 'b':
    case 'B': 
    case 'm':
    case 'M':
    case 'f':
    case 'F':
    case 'e':
    case 'E':
    case 't':
    case 'T':
    case 'z':
    case 'r':
    case 'R':
        glutTimerFunc(0, timer, 0);
        break;
    case 'c':
    case 'C':
        reset_All();
        break;
    case 's':
        keyState.status = false;
        break;
    case '+':
    {
        glm::mat4 rotationMatrix;
        rotationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.1f));
        cameraPos = glm::vec3(rotationMatrix * glm::vec4(cameraPos, 1.0f));
        break;
    }
    case '-':
    {
        glm::mat4 rotationMatrix;
        rotationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.1f));
        cameraPos = glm::vec3(rotationMatrix * glm::vec4(cameraPos, 1.0f));
        break;
    }
    case 'q':
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

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        std::cout << "x = " << x << " y = " << y << std::endl;
    glutPostRedisplay();
}

void reset_All() {

    models.clear();

    Model modelBoard, modelBottomBox, modelMiddleBox, modelArmRight, modelArmLeft, modelLegRight, modelLegLeft;

    read_obj_file("obj/board.obj", modelBoard, "board");
    modelBoard.modelMatrix = glm::mat4(1.0f);
    modelBoard.modelMatrix = glm::translate(modelBoard.modelMatrix, glm::vec3(0.0, 0.0, 0.0));
    modelBoard.modelMatrix = glm::scale(modelBoard.modelMatrix, glm::vec3(2.5, 2.5, 2.5));
    modelBoard.initialRotation = modelBoard.modelMatrix;
    modelBoard.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelBoard);

    read_obj_file("obj/box_bottom.obj", modelBottomBox, "bottom_b");
    modelBottomBox.modelMatrix = glm::mat4(1.0f);
    modelBottomBox.modelMatrix = glm::translate(modelBottomBox.modelMatrix, glm::vec3(0.0, 0.0, 0.0));
    modelBottomBox.modelMatrix = glm::scale(modelBottomBox.modelMatrix, glm::vec3(0.05, 0.05, 0.05));
    modelBottomBox.initialRotation = modelBottomBox.modelMatrix;
    modelBottomBox.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelBottomBox);

    read_obj_file("obj/box_middle.obj", modelMiddleBox, "mid_b");
    modelMiddleBox.modelMatrix = glm::mat4(1.0f);
    modelMiddleBox.modelMatrix = glm::translate(modelMiddleBox.modelMatrix, glm::vec3(0.0, 0.0, 0.0));
    modelMiddleBox.modelMatrix = glm::scale(modelMiddleBox.modelMatrix, glm::vec3(0.05, 0.05, 0.05));
    modelMiddleBox.initialRotation = modelMiddleBox.modelMatrix;
    modelMiddleBox.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelMiddleBox);

    read_obj_file("obj/box_arm_right.obj", modelArmRight, "right_a_b");
    modelArmRight.modelMatrix = glm::mat4(1.0f);
    modelArmRight.modelMatrix = glm::translate(modelArmRight.modelMatrix, glm::vec3(0.0, 0.0, 0.0));
    modelArmRight.modelMatrix = glm::scale(modelArmRight.modelMatrix, glm::vec3(0.05, 0.05, 0.05));
    modelArmRight.initialRotation = modelArmRight.modelMatrix;
    modelArmRight.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelArmRight);

    read_obj_file("obj/box_arm_left.obj", modelArmLeft, "left_a_b");
    modelArmLeft.modelMatrix = glm::mat4(1.0f);
    modelArmLeft.modelMatrix = glm::translate(modelArmLeft.modelMatrix, glm::vec3(0.0, 0.0, 0.0));
    modelArmLeft.modelMatrix = glm::scale(modelArmLeft.modelMatrix, glm::vec3(0.05, 0.05, 0.05));
    modelArmLeft.initialRotation = modelArmLeft.modelMatrix;
    modelArmLeft.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelArmLeft);

    read_obj_file("obj/box_leg_right.obj", modelLegRight, "right_l_b");
    modelLegRight.modelMatrix = glm::mat4(1.0f);
    modelLegRight.modelMatrix = glm::translate(modelLegRight.modelMatrix, glm::vec3(0.0, 0.0, 0.0));
    modelLegRight.modelMatrix = glm::scale(modelLegRight.modelMatrix, glm::vec3(0.05, 0.05, 0.05));
    modelLegRight.initialRotation = modelLegRight.modelMatrix;
    modelLegRight.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelLegRight);

    read_obj_file("obj/box_leg_left.obj", modelLegLeft, "left_l_b");
    modelLegLeft.modelMatrix = glm::mat4(1.0f);
    modelLegLeft.modelMatrix = glm::translate(modelLegLeft.modelMatrix, glm::vec3(0.0, 0.0, 0.0));
    modelLegLeft.modelMatrix = glm::scale(modelLegLeft.modelMatrix, glm::vec3(0.05, 0.05, 0.05));
    modelLegLeft.initialRotation = modelLegLeft.modelMatrix;
    modelLegLeft.colors.push_back(glm::vec3(0.0, 0.0, 0.0));
    models.push_back(modelLegLeft);

    InitBuffer();

}

int main(int argc, char** argv) {

    width = 1200;
    height = 750;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(50, 5);
    glutInitWindowSize(width, height);
    glutCreateWindow("19번");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else
        cout << "GLEW Initialized\n";

    make_shaderProgram();

    reset_All();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyBoard);
    glutSpecialFunc(keySpecial);
    glutMouseFunc(mouse);
    glutMainLoop();

    return 0;
}

void renderModel() {
    glEnable(GL_DEPTH_TEST);

    GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");

    glUniform1i(glGetUniformLocation(shaderProgramID, "useCustomColor"), GL_FALSE);

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

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i].modelMatrix));

        glUniform1i(modelStatus, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, models[i].faces.size() * 3, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }

    glUniform1i(glGetUniformLocation(shaderProgramID, "useCustomColor"), GL_TRUE);

    for (int i = 0; i < 3; ++i) {
        glUniform3fv(glGetUniformLocation(shaderProgramID, "customColor"), 1, glm::value_ptr(color_xyz[i]));
        glBindVertexArray(xyz_VAO[i]);
        glUniform1i(modelStatus, 0);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
    }

    glDisable(GL_DEPTH_TEST);
}

GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);

   /* glm::mat4 view = glm::mat4(1.0f);
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
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);*/


    GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
    GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(-cameraPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(1.0f, 0.95f, 0.9f)));

    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");

    glViewport(0, 0, width / 2, height / 2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    renderModel();

    glViewport(width / 2, height / 2, width / 2, height / 2);
    glm::mat4 view2 = glm::lookAt(glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
    glm::mat4 projection2 = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view2));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection2));
    renderModel();

    // 뷰포트 3: 직각 투영(XY 평면)
    glViewport(width / 2, 0, width / 2, height / 2);  // 우측 하단
    glm::mat4 view3 = glm::lookAt(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 projection3 = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view3));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection3));
    renderModel();



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

    glGenVertexArrays(1, &xyz_VAO[1]);
    glBindVertexArray(xyz_VAO[1]);
    glGenBuffers(1, &xyz_VBO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, xyz_VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(y), y, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &xyz_VAO[2]);
    glBindVertexArray(xyz_VAO[2]);
    glGenBuffers(1, &xyz_VBO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, xyz_VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(z), z, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // 색상 버퍼 설정
        //glBindBuffer(GL_ARRAY_BUFFER, vbos[i][1]);
        //glBufferData(GL_ARRAY_BUFFER, models[i].colors.size() * sizeof(glm::vec3), models[i].colors.data(), GL_STATIC_DRAW);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][2]);  // 법선용 VBO
        glBufferData(GL_ARRAY_BUFFER, models[i].normals.size() * sizeof(glm::vec3), models[i].normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
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
        glBindVertexArray(0);
    }
}
