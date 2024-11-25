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

	if (keyState['s']) { // �� ������
		for (auto& model : models) {
			if (model->name != "snow") continue;
			glm::mat4 matrix = glm::mat4(1.0f);
			matrix = glm::translate(matrix, glm::vec3(0.0, -(model->speed), 0.0));

			// ����(ȸ��)
			glm::vec3 center = glm::vec3(0.0, 0.0, 0.0); // ȸ�� �߽� ���� (���� ���� �߽�)
			glm::mat4 rotation = glm::mat4(1.0f);
			rotation = glm::translate(rotation, center); // �߽����� �̵�
			rotation = glm::rotate(rotation, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0)); // Y�� ���� ȸ��
			rotation = glm::translate(rotation, -center); // ���� ��ġ�� ����

			model->matrix = matrix * rotation * model->matrix; // �̵� + ȸ�� ����

		}
		checkSnowBoard();
	}

	if (keyState['r']) { // ������ ȭ�� �߾��� ���� ����
		//��ü �̵�
		for (auto& model : models) {
			if (model->name != "light_m") continue;

			glm::vec3 center = glm::vec3(0.0, 0.0, 0.0); // ȸ�� �߽� ���� (���� ���� �߽�)
			glm::mat4 rotation = glm::mat4(1.0f);
			rotation = glm::translate(rotation, center); // �߽����� �̵�
			rotation = glm::rotate(rotation, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0)); // Y�� ���� ȸ��
			rotation = glm::translate(rotation, -center); // ���� ��ġ�� ����
			model->matrix = rotation * model->matrix; // �̵� + ȸ�� ����

			// `lightPos`�� ȸ�� ����
			glm::vec4 lightPos4 = glm::vec4(lightPos, 1.0f); // `vec3`�� `vec4`�� Ȯ��
			lightPos4 = rotation * lightPos4; // ȸ�� ����
			lightPos = glm::vec3(lightPos4); // �ٽ� `vec3`�� ��ȯ
		}
	}


	UpdateRigidBodyTransforms(models);
	glutPostRedisplay();
	glutTimerFunc(16, timer, ++value);
}

