#pragma once
#include <iostream>
#include <gl/glew.h>  // GLEW ���̺귯�� ���� (OpenGL Ȯ�� ����� ����ϱ� ���� �ʿ�)

#include "filetobuf.h"

using namespace std;  // ���ӽ����̽� std ������� �ڵ� ������ std:: ���� ����

// ���� ���� �����
GLint width, height;  // â�� �ʺ�� ���̸� �����ϴ� ����
GLuint shaderProgramID;  // ���̴� ���α׷��� ID�� �����ϴ� ����
GLuint vertexShader;  // ���ؽ� ���̴� ID�� �����ϴ� ����
GLuint fragmentShader;  // �����׸�Ʈ ���̴� ID�� �����ϴ� ����

GLchar* vertexSource, * fragmentSource;  // ���̴� �ҽ� �ڵ带 ������ ������

// ���ؽ� ���̴��� ����� �Լ�
void make_vertexShaders() {
    vertexSource = filetobuf("vertex.glsl");  // ���ؽ� ���̴� ���� �б�
    vertexShader = glCreateShader(GL_VERTEX_SHADER);  // ���ؽ� ���̴� ����
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);  // ���̴� �ҽ� �ڵ� ����
    glCompileShader(vertexShader);  // ���̴� ������

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);  // ������ ���� ���� Ȯ��

    if (!result) {  // ������ ���� ��
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);  // ���� �α� �ޱ�
        cerr << "ERROR: vertex shader ������ ���� \n" << errorLog << endl;  // ���� �޽��� ���
        return;
    }
}

// �����׸�Ʈ ���̴��� ����� �Լ�
void make_fragmentShaders() {
    fragmentSource = filetobuf("fragment.glsl");  // �����׸�Ʈ ���̴� ���� �б�
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  // �����׸�Ʈ ���̴� ����
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);  // ���̴� �ҽ� �ڵ� ����
    glCompileShader(fragmentShader);  // ���̴� ������

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);  // ������ ���� ���� Ȯ��

    if (!result) {  // ������ ���� ��
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);  // ���� �α� �ޱ�
        cerr << "ERROR: frag_shader ������ ����\n" << errorLog << endl;  // ���� �޽��� ���
        return;
    }
}

// ���̴� ���α׷��� ����� �Լ�
void make_shaderProgram() {
    make_vertexShaders();  // ���ؽ� ���̴� ����
    make_fragmentShaders();  // �����׸�Ʈ ���̴� ����

    shaderProgramID = glCreateProgram();  // ���̴� ���α׷� ����
    glAttachShader(shaderProgramID, vertexShader);  // ���ؽ� ���̴� ���α׷��� ÷��
    glAttachShader(shaderProgramID, fragmentShader);  // �����׸�Ʈ ���̴� ���α׷��� ÷��
    glLinkProgram(shaderProgramID);  // ���̴� ���α׷� ��ũ

    glDeleteShader(vertexShader);  // ���ؽ� ���̴� ���� (�̹� ���α׷��� ���ԵǾ����Ƿ�)
    glDeleteShader(fragmentShader);  // �����׸�Ʈ ���̴� ����

    glUseProgram(shaderProgramID);  // ���̴� ���α׷� ���
}