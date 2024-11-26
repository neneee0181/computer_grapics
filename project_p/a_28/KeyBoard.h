#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "Camera.h"
#include "Root.h"
#include "Light.h"
#include "SierpinskiModel.h"

//키
std::unordered_map<char, bool> keyState;
std::uniform_real_distribution<> random_color_dis(0.0, 1.0);

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

int lightIndex = 0; // p 키로 이동할 때 사용할 위치 인덱스


void keyDown(unsigned char key, int x, int y) {

    keyDown_s(key);

    switch (key)
    {
    case 'p':
    {
        static glm::vec3 lightPositions[4] = {
            glm::vec3(1.0f, lightPos.y, 0.0f),   // (1, y, 0)
            glm::vec3(-1.0f, lightPos.y, 0.0f),  // (-1, y, 0)
            glm::vec3(0.0f, lightPos.y, 1.0f),   // (0, y, 1)
            glm::vec3(0.0f, lightPos.y, -1.0f)   // (0, y, -1)
        };

        // 조명 위치 업데이트
        lightPos = lightPositions[lightIndex];

        // 조명 모델 위치도 이동
        for (auto& model : models) {
            if (model->name != "light_m") continue;
            model->matrix[3] = glm::vec4(lightPos, 1.0);
            //model->matrix = glm::translate(glm::mat4(1.0f), lightPos);
        }

        // 다음 위치로 인덱스 순환
        lightIndex = (lightIndex + 1) % 4;

        break;
    }
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
    case 'n': // 조명을 중심으로 가까워지게 이동
    {
        const float minDistance = 0.5f; // 최소 거리
        glm::vec3 direction = glm::normalize(-lightPos); // 중심 방향 벡터
        float currentDistance = glm::length(lightPos); // 현재 중심까지의 거리

        if (currentDistance > minDistance) { // 최소 거리 이상일 때만 이동
            lightPos += direction * 0.1f; // 방향으로 0.1씩 이동
            for (auto& model : models) {
                if (model->name != "light_m") continue;
                model->matrix = glm::translate(glm::mat4(1.0f), direction * 0.1f) * model->matrix; // 모델 이동
            }
        }
        break;
    }
    case 'f': // 조명을 중심에서 멀어지게 이동
    {
        const float maxDistance = 10.0f; // 최대 거리
        glm::vec3 direction = glm::normalize(-lightPos); // 중심 방향 벡터
        float currentDistance = glm::length(lightPos); // 현재 중심까지의 거리

        if (currentDistance < maxDistance) { // 최대 거리 이하일 때만 이동
            lightPos -= direction * 0.1f; // 방향의 반대 방향으로 0.1씩 이동
            for (auto& model : models) {
                if (model->name != "light_m") continue;
                model->matrix = glm::translate(glm::mat4(1.0f), -direction * 0.1f) * model->matrix; // 모델 이동
            }
        }
        break;
    }
    case '+': // 조명 세기 증가
    {
        lightColor += glm::vec3(0.1f, 0.1f, 0.1f); // 조명 세기 증가
        if (lightColor.x > 1.0f) lightColor = glm::vec3(1.0f); // 최대값 제한
        break;
    }
    case '-': // 조명 세기 감소
    {
        lightColor -= glm::vec3(0.1f, 0.1f, 0.1f); // 조명 세기 감소
        if (lightColor.x < 0.1f) lightColor = glm::vec3(0.1f); // 최소값 제한
        break;
    }
    case 'c':
        lightColor = glm::vec3(random_color_dis(gen), random_color_dis(gen), random_color_dis(gen));
        break;
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