#pragma once
#include <iostream>   // ǥ�� ����� ���̺귯�� ����
#include <gl/glew.h>  // GLEW ���̺귯�� ���� (OpenGL Ȯ�� ����� ����ϱ� ���� �ʿ�)

#define STB_IMAGE_IMPLEMENTATION
#include <gl/stb_image.h>

// �ؽ�ó �ε� �Լ�
GLuint loadTexture(const char* texturePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);  // �ؽ�ó ID ����
    glBindTexture(GL_TEXTURE_2D, textureID);  // 2D �ؽ�ó ���ε�

    // �ؽ�ó �ݺ� ��� �� ���͸� ����
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);  // �̹��� �ε�
    if (data) {
        // �̹��� �����͸� �ؽ�ó�� ���ε� (RGBA �Ǵ� RGB ���� ó��)
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);  // Mipmap ����
    }
    else {
        std::cerr << "Failed to load texture: " << texturePath << std::endl;
    }
    stbi_image_free(data);  // �̹��� �޸� ����

    return textureID;
}