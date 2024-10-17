#version 330 core

layout (location = 0) in vec3 in_Position;  // ���� ��ġ
layout (location = 1) in vec3 in_Color;     // ���� ����

out vec3 out_Color;  // �����׸�Ʈ ���̴��� ������ ����
uniform mat4 modelV;   // ��ȯ ���
uniform mat4 view;      // �� ��
uniform mat4 projection; // ����

uniform mat4 tre;
uniform mat4 xLine;
uniform mat4 yLine;
uniform int isLine;  // ���� �׸� ���� 1, ���� �׸� ���� 0

void main() {
     if (isLine == 0 || isLine == 1) {
        // ������ü�� ���� ��ȯ ����� ����
        gl_Position = projection * view * modelV * vec4(in_Position, 1.0);
    } else if (isLine == 2) {
        // X�� ���� ���� ��ȯ ����� ����
        gl_Position = xLine * vec4(in_Position, 1.0);
    } else if(isLine == 3){
        gl_Position = yLine * vec4(in_Position, 1.0);
    }

    // ���� �׸� ���� ������, ���� �׸� ���� ���� ���� ���
    if (isLine == 1)
        out_Color = vec3(1.0, 1.0, 1.0);  // ���� ���� ������
    else if (isLine == 0)
        out_Color = in_Color;  // ���� ���� �Էµ� ����
    else if (isLine == 2)
        out_Color = vec3(0.0, 1.0, 0.0);
    else if(isLine == 3)
        out_Color = vec3(1.0, 0.0, 0.0);

}
