#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "Light.h"
#include "Camera.h"

#include <random>

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

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist(0.0, 1.0);

void keyDown(unsigned char key, int x, int y) {
    float cameraSpeed = 0.5f; // 카메라 이동 속도
    float rotationSpeed = 2.0f; // 회전 속도

    keyDown_s(key);

    switch (key)
    {
    case 'c':
        lightColor = glm::vec3(dist(gen), dist(gen), dist(gen));
        break;
    case 'z':
        cameraPos.z -= cameraSpeed;
        break;
    case 'Z':
        cameraPos.z += cameraSpeed;
        break;
    case 'x':
        cameraPos.x += cameraSpeed;
        break;
    case 'X':
        cameraPos.x -= cameraSpeed;
        break;
    case 'r': {
        // y축을 기준으로 양방향 회전
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
        cameraPos = glm::vec3(rotationMatrix * glm::vec4(cameraPos, 1.0f));
        break;
    }
    case 'R': {
        // y축을 기준으로 음방향 회전
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
        cameraPos = glm::vec3(rotationMatrix * glm::vec4(cameraPos, 1.0f));
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