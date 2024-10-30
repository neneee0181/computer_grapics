#version 330 core

layout (location = 0) in vec3 in_Position;  // ���� ��ġ
layout (location = 1) in vec3 in_Normal;    // ���� ����

out vec3 FragPos;      // �����׸�Ʈ ��ġ (world space)
out vec3 Normal;       // �����׸�Ʈ�� ���� ���� (world space)

uniform mat4 model;              // �� ��ȯ ���
uniform mat4 viewTransform;      // �� ��ȯ ���
uniform mat4 projectionTransform; // ���� ��ȯ ���

void main() {
    FragPos = vec3(model * vec4(in_Position, 1.0));      // �� ��ȯ ���� ��ġ
    Normal = mat3(transpose(inverse(model))) * in_Normal; // ���� ���� ��ȯ
    gl_Position = projectionTransform * viewTransform * vec4(FragPos, 1.0);
}
