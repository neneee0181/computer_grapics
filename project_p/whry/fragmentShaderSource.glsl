#version 330

uniform vec4 vColor;
in vec3 out_Color; //--- ���ؽ� ���̴����Լ� ���� ����
out vec4 FragColor; //--- ���� ���
void main(void) 
{
	FragColor = vec4 (out_Color, 1.0);
	if(vColor.w > 0){
	FragColor = vColor;
	}
}
