#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Model.h"

// MTL ������ �о�ͼ� ���� ������ �Ľ��ϴ� �Լ�
void read_mtl_file(const std::string& filename, Material& material) {
    std::ifstream file("obj/" + filename);  // MTL ���� ����
    if (!file.is_open()) {  // ���� ���� ���� �� ���� ó��
        //throw std::runtime_error("Error opening MTL file: " + filename);
        return;
    }

    std::string line;
    bool hasTexture = false;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "Ka") {  // ȯ�汤 ����
            ss >> material.Ka.x >> material.Ka.y >> material.Ka.z;
        }
        else if (prefix == "Kd") {  // ���ݻ籤 ����
            ss >> material.Kd.x >> material.Kd.y >> material.Kd.z;
        }
        else if (prefix == "Ks") {  // �ݻ籤 ����
            ss >> material.Ks.x >> material.Ks.y >> material.Ks.z;
        }
        else if (prefix == "Ns") {  // ��¦�� ����
            ss >> material.Ns;
        }
        else if (prefix == "map_Kd") {  // �ؽ�ó ���� ���
            ss >> material.map_Kd;
            hasTexture = true;
        }
    }

    material.hasTexture = hasTexture;  // �ؽ�ó�� �ִ��� ���� �÷��� ����
    file.close();  // ���� �ݱ�
}