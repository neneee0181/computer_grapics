#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "Camera.h"
#include "Root.h"

//키
std::unordered_map<char, bool> keyState;

void keyDown_s(const char& key) {
    keyState[key] = !keyState[key];
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
    //keyUp_s(key);
    glutPostRedisplay();
}

void keyDown(unsigned char key, int x, int y) {

    keyDown_s(key);

    switch (key)
    {
    case '0':
    {
        models.pop_back();
        SierpinskiModel* sp_model = new SierpinskiModel(0);
        glm::mat4 sp_m = glm::mat4(1.0f);
        sp_m = glm::scale(sp_m, glm::vec3(0.5, 0.5, 0.5));
        sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.0, 0.5));
        sp_m = glm::rotate(sp_m, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
        sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.5, 0.0));
        sp_model->matrix = sp_m * sp_model->matrix;
        sp_model->initBuffer();
        models.push_back(sp_model);
        break;
    }
    case '1':
    {
        models.pop_back();
        SierpinskiModel* sp_model = new SierpinskiModel(1);
        glm::mat4 sp_m = glm::mat4(1.0f);
        sp_m = glm::scale(sp_m, glm::vec3(0.5, 0.5, 0.5));
        sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.0, 0.5));
        sp_m = glm::rotate(sp_m, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
        sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.5, 0.0));
        sp_model->matrix = sp_m * sp_model->matrix;
        sp_model->initBuffer();
        models.push_back(sp_model);
        break;
    }
    case '2':
    {
        models.pop_back();
        SierpinskiModel* sp_model = new SierpinskiModel(2);
        glm::mat4 sp_m = glm::mat4(1.0f);
        sp_m = glm::scale(sp_m, glm::vec3(0.5, 0.5, 0.5));
        sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.0, 0.5));
        sp_m = glm::rotate(sp_m, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
        sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.5, 0.0));
        sp_model->matrix = sp_m * sp_model->matrix;
        sp_model->initBuffer();
        models.push_back(sp_model);
        break;
    }
    case '3':
    {
        models.pop_back();
        SierpinskiModel* sp_model = new SierpinskiModel(3);
        glm::mat4 sp_m = glm::mat4(1.0f);
        sp_m = glm::scale(sp_m, glm::vec3(0.5, 0.5, 0.5));
        sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.0, 0.5));
        sp_m = glm::rotate(sp_m, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
        sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.5, 0.0));
        sp_model->matrix = sp_m * sp_model->matrix;
        sp_model->initBuffer();
        models.push_back(sp_model);
        break;
    }
    case '4':
    {
        models.pop_back();
        SierpinskiModel* sp_model = new SierpinskiModel(4);
        glm::mat4 sp_m = glm::mat4(1.0f);
        sp_m = glm::scale(sp_m, glm::vec3(0.5, 0.5, 0.5));
        sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.0, 0.5));
        sp_m = glm::rotate(sp_m, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
        sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.5, 0.0));
        sp_model->matrix = sp_m * sp_model->matrix;
        sp_model->initBuffer();
        models.push_back(sp_model);
        break;
    }
    case '5':
    {
        models.pop_back();
        SierpinskiModel* sp_model = new SierpinskiModel(5);
        glm::mat4 sp_m = glm::mat4(1.0f);
        sp_m = glm::scale(sp_m, glm::vec3(0.5, 0.5, 0.5));
        sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.0, 0.5));
        sp_m = glm::rotate(sp_m, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
        sp_m = glm::translate(sp_m, glm::vec3(0.0, 0.5, 0.0));
        sp_model->matrix = sp_m * sp_model->matrix;
        sp_model->initBuffer();
        models.push_back(sp_model);
        break;
    }
    case 'q':
        std::cout << " 프로그램 종료 " << std::endl;
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