#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <gl/glm/glm/glm.hpp>
#include <gl/glew.h>
#include <gl/stb_image.h>

// �ؽ�ó �ε� �Լ�
GLuint load_texture(const std::string& path) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0); // �̹��� �ε�

    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0; // 0�� OpenGL���� ��ȿ�� �ؽ�ó ID�� �ǹ�
    }

    // OpenGL�� �ؽ�ó ����
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // �ؽ�ó �Ķ���� ���� (���͸�, �ݺ� ���)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // �ؽ�ó ������ ���ε�
    GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA; // ä�� ���� ���� ���� ����
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); // Mipmap ����

    stbi_image_free(data); // �޸� ����

    return textureID; // �ؽ�ó ID ��ȯ
}