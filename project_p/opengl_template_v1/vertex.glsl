#version 330 core

layout (location = 0) in vec3 in_Position;  // ���� ��ġ
layout (location = 1) in vec3 in_Color;     // ���� ����

out vec3 out_Color;    // �����׸�Ʈ ���̴��� ������ ����
uniform mat4 model;       // �� ��ȯ ���

uniform mat4 Line;
uniform int modelStatus;  // 0 -> �� , 1 -> ��ǥ��

void main() {
    if (modelStatus == 0) {
        gl_Position = model * vec4(in_Position, 1.0);
        out_Color = in_Color;  // ��
    }else if(modelStatus == 1){
        gl_Position = Line * vec4(in_Position, 1.0);
        out_Color = in_Color;   // x,y,z ��
    }
}