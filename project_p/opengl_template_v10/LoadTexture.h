#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <gl/glm/glm/glm.hpp>
#include <gl/glew.h>
#include "stb_image.h"

void load_texture(Material& material) {
    if (!material.map_Kd.empty()) {
        glGenTextures(1, &material.textureID);
        glBindTexture(GL_TEXTURE_2D, material.textureID);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(material.map_Kd.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // 텍스처 필터링 설정
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            material.hasTexture = true; // 텍스처 로드 성공 시 true
            std::cout << "Texture loaded successfully: " << material.map_Kd << std::endl;
        }
        else {
            material.hasTexture = false; // 텍스처 로드 실패 시 false
            std::cerr << "Failed to load texture: " << material.map_Kd << std::endl;
        }
        stbi_image_free(data);
    }
    else {
        material.hasTexture = false; // 텍스처 경로가 없는 경우 false
    }
}