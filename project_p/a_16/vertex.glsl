#version 330 core

layout(location = 0) in vec3 in_Position;  // ���� ��ġ
layout(location = 1) in vec3 in_Color;     // ���� ����

out vec3 out_Color;  // �����׸�Ʈ ���̴��� ������ ����
uniform mat4 model;  // �� ��ȯ ���
uniform mat4 Line;   // ��ǥ ��� ��ȯ ���
uniform int modelStatus;  // 0 -> ��, 1 -> ��ǥ��

// X�� ȸ�� ��� ���� �Լ�
mat4 rotateX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, c,   -s,  0.0,
        0.0, s,   c,   0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

// Y�� ȸ�� ��� ���� �Լ�
mat4 rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        c,   0.0, s,   0.0,
        0.0, 1.0, 0.0, 0.0,
        -s,  0.0, c,   0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

void main() {
    // �߰� ȸ�� ��ȯ ��� ���� (�� ��ǥ�迡�� ȸ��)
    mat4 rotationMatrix = mat4(1.0);  // ���� ��ķ� ����
    
    // 35�� X�� ȸ��
    float angleX = radians(35.0);
    rotationMatrix = rotateX(angleX);  // X�� ȸ��

    // -35�� Y�� ȸ��
    float angleY = radians(-35.0);
    rotationMatrix = rotationMatrix * rotateY(angleY);  // Y�� ȸ��

    // �𵨿� ȸ�� ��ȯ�� ����

    if (modelStatus == 0) {
        // �� ��ȯ ����
        gl_Position = model * vec4(in_Position, 1.0);
        out_Color = in_Color;  // �� ���� ����
    } else if (modelStatus == 1) {
        // ��ǥ�� ��ȯ ����
        mat4 newLine = Line * rotationMatrix;
        gl_Position = newLine * vec4(in_Position, 1.0);
        out_Color = in_Color;  // ��ǥ�� ���� ����
    }
}
