#version 330 core

layout (location = 0) in vec3 in_Position;  // ���� ��ġ
layout (location = 1) in vec3 in_Color;     // ���� ����
layout (location = 2) in vec3 in_Normal;    // ���� ����
layout (location = 3) in vec2 in_TexCoord;  // �ؽ�ó ��ǥ �߰�

out vec3 out_Color;    // �����׸�Ʈ ���̴��� ������ ����
out vec3 FragPos;      // �����׸�Ʈ ��ġ
out vec3 Normal;       // �����׸�Ʈ ���� ����
out vec2 TexCoords;    // �����׸�Ʈ ���̴��� ������ �ؽ�ó ��ǥ �߰�

uniform mat4 model;       // �� ��ȯ ���

uniform mat4 tre;
uniform mat4 xLine;
uniform mat4 yLine;
uniform mat4 zLine;

uniform mat4 Line;
uniform int modelStatus;  // ���� �׸� ���� 1, ���� �׸� ���� 0

void main() {
    if (modelStatus == 0) {
        gl_Position = model * vec4(in_Position, 1.0);
        out_Color = in_Color;  // ���� ���� �Էµ� ����
    }else if(modelStatus == 1){
        gl_Position = Line * vec4(in_Position, 1.0);
        out_Color = in_Color;  // ���� ���� �Էµ� ����
    }
}
