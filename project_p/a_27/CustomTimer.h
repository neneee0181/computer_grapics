#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>

#include "KeyBoard.h"

std::uniform_real_distribution<> snow_speed_dis(0.2, 1.0);

void timer(int value) {

	if (keyState['s']) { // ´« ³»¸®±â
		std::cout << "1" << std::endl;
	}

	glutPostRedisplay();
	glutTimerFunc(16, timer, ++value);
}