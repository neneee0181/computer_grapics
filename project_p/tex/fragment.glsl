// fragment.glsl
#version 330 core
out vec4 FragColor;

uniform vec3 Kd;  // ���ݻ� ����
uniform vec3 Ka;  // ȯ�汤 ����
uniform vec3 Ks;  // �ݻ籤 ����

void main() {
    vec3 color = Kd;  // ������ ���ݻ� ���� ��� (Phong �� Ȯ�� ����)
    FragColor = vec4(color, 1.0);
}