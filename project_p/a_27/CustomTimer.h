#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>

#include "KeyBoard.h"
#include "Root.h"

void checkSnowBoard() {
	for (const auto& model_1 : models) {
		if (model_1->name != "board") continue;
		for (auto& model_2 : models) {
			if (model_2->name != "snow") continue;

			CustomContactResultCallback resultCallback;
			dynamicsWorld->contactPairTest(model_1->rigidBody, model_2->rigidBody, resultCallback);
			if (resultCallback.hitDetected) {
				model_2->matrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(snow_location_dis_x(gen), 1.5, snow_location_dis_x(gen))), glm::vec3(0.1, 0.1, 0.1));
			}
		}
	}
}

void timer(int value) {

	if (keyState['s']) { // 눈 내리기
		for (auto& model : models) {
			if (model->name != "snow") continue;
			glm::mat4 matrix = glm::mat4(1.0f);
			matrix = glm::translate(matrix, glm::vec3(0.0, -(model->speed), 0.0));

			// 자전(회전)
			glm::vec3 center = glm::vec3(0.0, 0.0, 0.0); // 회전 중심 설정 (모델의 로컬 중심)
			glm::mat4 rotation = glm::mat4(1.0f);
			rotation = glm::translate(rotation, center); // 중심으로 이동
			rotation = glm::rotate(rotation, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0)); // Y축 기준 회전
			rotation = glm::translate(rotation, -center); // 원래 위치로 복귀

			model->matrix = matrix * rotation * model->matrix; // 이동 + 회전 적용

		}
		checkSnowBoard();
	}

	if (keyState['r']) { // 조명이 화면 중앙을 기준 공전
		//물체 이동
		for (auto& model : models) {
			if (model->name != "light_m") continue;

			glm::vec3 center = glm::vec3(0.0, 0.0, 0.0); // 회전 중심 설정 (모델의 로컬 중심)
			glm::mat4 rotation = glm::mat4(1.0f);
			rotation = glm::translate(rotation, center); // 중심으로 이동
			rotation = glm::rotate(rotation, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0)); // Y축 기준 회전
			rotation = glm::translate(rotation, -center); // 원래 위치로 복귀
			model->matrix = rotation * model->matrix; // 이동 + 회전 적용

			// `lightPos`에 회전 적용
			glm::vec4 lightPos4 = glm::vec4(lightPos, 1.0f); // `vec3`를 `vec4`로 확장
			lightPos4 = rotation * lightPos4; // 회전 적용
			lightPos = glm::vec3(lightPos4); // 다시 `vec3`로 변환
		}
	}


	UpdateRigidBodyTransforms(models);
	glutPostRedisplay();
	glutTimerFunc(16, timer, ++value);
}

