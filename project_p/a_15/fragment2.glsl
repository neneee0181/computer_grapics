#version 330 core

struct Material {
    vec3 Ka;  // ȯ�汤 ����
    vec3 Kd;  // ���ݻ籤 ����
    vec3 Ks;  // �ݻ籤 ����
    float Ns; // ��¦�� ����
};

uniform Material material;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

in vec2 TexCoords;  // �ؽ�ó ��ǥ �Է�
uniform sampler2D texture1;  // �ؽ�ó ������

void main() {
    // �ؽ�ó ���� ���ø�
    vec3 texColor = texture(texture1, TexCoords).rgb;

    // ���� ��� (Phong ���� ��)
    vec3 ambient = material.Ka * texColor;  // �ؽ�ó�� ȯ�汤 ����

    // Diffuse (���ݻ籤 ���)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.Kd * diff * texColor;  // �ؽ�ó�� ���ݻ籤 ����

    // Specular (�ݻ籤 ���)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Ns);
    vec3 specular = material.Ks * spec;  // �ݻ籤�� �ؽ�ó�� �����ϰ� ����

    // ���� ���� ���
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);  // ���İ��� 1.0���� ����
}
