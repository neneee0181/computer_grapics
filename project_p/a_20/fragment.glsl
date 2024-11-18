#version 330 core
in vec3 FragPos;   // �����׸�Ʈ ��ġ (vertex shader���� ����)
in vec3 Normal;    // ������ ���� ���� (vertex shader���� ����)

out vec4 FragColor;

uniform vec3 viewPos;      // ī�޶�(���) ��ġ
uniform vec3 lightPos;     // ������ ��ġ
uniform vec3 lightColor;   // ������ ����

uniform vec3 Ka;           // ȯ�汤 ����
uniform vec3 Kd;           // ���ݻ� ����
uniform vec3 Ks;           // �ݻ籤 ����
uniform float Ns;          // ��¦�� ���� (���̴ϴϽ�)

uniform bool useCustomColor;  // Ŀ���� ���� ��� �÷���
uniform vec3 customColor;     // Ŀ���� ���� (��ǥ�� ����)

void main() {
    if (useCustomColor) {
        // �÷��װ� true�� ���, ���� ��� ���� ���� ���� ���
        FragColor = vec4(customColor, 1.0);
        return;
    }

    // ���� ���
    vec3 ambient = Ka * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = Kd * diff * lightColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns);
    vec3 specular = Ks * spec * lightColor;

    // ���� ���� ���
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}