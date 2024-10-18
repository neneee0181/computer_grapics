#version 330 core

layout (location = 0) in vec3 in_Position;  // ���� ��ġ
layout (location = 1) in vec3 in_Color;     // ���� ����
layout (location = 2) in vec3 in_Normal;    // ���� ����
layout (location = 3) in vec2 in_TexCoord;  // �ؽ�ó ��ǥ �߰�

out vec3 out_Color;    // �����׸�Ʈ ���̴��� ������ ����
out vec3 FragPos;      // �����׸�Ʈ ��ġ
out vec3 Normal;       // �����׸�Ʈ ���� ����
out vec2 TexCoords;    // �����׸�Ʈ ���̴��� ������ �ؽ�ó ��ǥ �߰�

uniform mat4 modelV;       // �� ��ȯ ���

uniform mat4 tre;
uniform mat4 xLine;
uniform mat4 yLine;
uniform mat4 zLine;
uniform int isLine;  // ���� �׸� ���� 1, ���� �׸� ���� 0

void main() {
    if (isLine == 0 || isLine == 1) {
        // ������ü�� ���� ��ȯ ����� ����
        gl_Position = modelV * vec4(in_Position, 1.0);

    } else if (isLine == 2) {
        // X�� ���� ���� ��ȯ ��� ����
        gl_Position = xLine * vec4(in_Position, 1.0);
    } else if (isLine == 3) {
        // Y�� ���� ���� ��ȯ ��� ����
        gl_Position = yLine * vec4(in_Position, 1.0);
    }  else if (isLine == 4) {
        // Y�� ���� ���� ��ȯ ��� ����
        gl_Position = zLine * vec4(in_Position, 1.0);
    } 

    // ���� �׸� ���� ���, ���� �׸� ���� ���� ���� ���
    if (isLine == 1)
        out_Color = vec3(1.0, 1.0, 1.0);  // ���� ���� ���
    else if (isLine == 0)
        out_Color = in_Color;  // ���� ���� �Էµ� ����
    else if (isLine == 2)
        out_Color = vec3(0.0, 1.0, 0.0);  // X���� ���� ���
    else if (isLine == 3)
        out_Color = vec3(1.0, 0.0, 0.0);  // Y���� ���� ������
    else if (isLine == 4)
        out_Color = vec3(0.0, 0.0, 1.0);  // z���� ���� �Ķ���
}
