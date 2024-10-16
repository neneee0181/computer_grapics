#include <iostream>   // 표준 입출력 라이브러리 포함
#include <gl/glew.h>  // GLEW 라이브러리 포함 (OpenGL 확장 기능을 사용하기 위해 필요)
#include <gl/freeglut.h>  // GLUT 라이브러리 포함 (윈도우 창 및 사용자 입력 처리를 위해 사용)
#include <gl/freeglut_ext.h>  // GLUT 확장 기능 포함
#include <gl/glm/glm/glm.hpp>  // GLM 라이브러리 포함 (수학적인 계산을 돕기 위한 라이브러리)

#include <vector>  // 표준 벡터 라이브러리 포함 (동적 배열을 사용하기 위해 필요)

#include "filetobuf.h"  // 쉐이더 소스 파일을 버퍼로 읽어오는 함수를 정의한 헤더파일
#include "ReadObj.h"

using namespace std;  // 네임스페이스 std 사용으로 코드 내에서 std:: 생략 가능

// 함수 선언부
void make_vertexShaders();  // 버텍스 쉐이더를 만드는 함수 선언
void make_fragmentShaders();  // 프래그먼트 쉐이더를 만드는 함수 선언
void InitBuffer();  // 버퍼 초기화 함수 선언
void make_shaderProgram();  // 쉐이더 프로그램을 만드는 함수 선언
GLvoid drawScene(GLvoid);  // 화면을 그리는 함수 선언
GLvoid Reshape(int w, int h);  // 화면 크기가 변경될 때 호출되는 함수 선언

// 전역 변수 선언부
GLint width, height;  // 창의 너비와 높이를 저장하는 변수
GLuint shaderProgramID;  // 쉐이더 프로그램의 ID를 저장하는 변수
GLuint vertexShader;  // 버텍스 쉐이더 ID를 저장하는 변수
GLuint fragmentShader;  // 프래그먼트 쉐이더 ID를 저장하는 변수

GLchar* vertexSource, * fragmentSource;  // 쉐이더 소스 코드를 저장할 변수들

GLuint vao, vbo[3];  // VAO와 VBO ID를 저장할 변수들

// 삼각형의 좌표를 정의한 배열
const GLfloat triShape[3][3] = {
	{-0.5,-0.5,0.0},{0.5,-0.5,0.0},{0.0,0.5,0.0}
};

// 삼각형의 각 점에 해당하는 색상 배열
const GLfloat colors[3][3] = {
	{1.0,0.0,0.0}, {0.0,1.0,0.0},{0.0,0.0,1.0}
};

Model modelSqu;

const GLfloat modelSquColors[4][3] = {
	{1.0,0.0,0.0}, {1.0,1.0, 0.0}, {0.0,1.0,0.0}, {0.0,0.0,0.0}
};

// 키보드 입력을 처리하는 함수
void keyBoard(unsigned char key, int x, int y) {
	switch (key)
	{
	default:
		break;
	}
	glutPostRedisplay();  // 키보드 입력 후 화면을 다시 그리도록 요청
}

// 마우스 입력을 처리하는 함수
void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		std::cout << "x = " << x << " y = " << y << std::endl;  // 마우스 클릭 좌표 출력
}

// 메인 함수
int main(int argc, char** argv) {

	width = 800;  // 창의 기본 너비 설정
	height = 600;  // 창의 기본 높이 설정

	// 윈도우 생성
	glutInit(&argc, argv);  // GLUT 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);  // 화면 모드 설정 (더블 버퍼링, RGBA 컬러 모드)
	glutInitWindowPosition(100, 100);  // 창의 시작 위치 설정
	glutInitWindowSize(width, height);  // 창의 크기 설정
	glutCreateWindow("실습 13번");  // 창 생성 및 제목 설정

	// GLEW 초기화
	glewExperimental = GL_TRUE;  // GLEW 실험적 기능 활성화
	if (glewInit() != GLEW_OK) {  // GLEW 초기화 및 오류 체크
		cerr << "Unable to initialize GLEW" << endl;  // GLEW 초기화 실패 시 오류 메시지 출력
		exit(EXIT_FAILURE);  // 프로그램 종료
	}
	else
		cout << "GLEW Initialized\n";  // 초기화 성공 시 메시지 출력

	make_shaderProgram();  // 쉐이더 프로그램 생성

	read_obj_file("box.obj", modelSqu); //obj 파일 가져오기

	try {
		std::cout << "OBJ 파일 로딩 성공!" << std::endl;
		std::cout << "정점 개수: " << modelSqu.vertices.size() << std::endl;
		std::cout << "텍스처 좌표 개수: " << modelSqu.texCoords.size() << std::endl;
		std::cout << "법선 벡터 개수: " << modelSqu.normals.size() << std::endl;
		std::cout << "면 개수: " << modelSqu.faces.size() << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;  // 오류 메시지 출력
	}

	InitBuffer();  // 버퍼 초기화

	// 콜백 함수 등록
	glutDisplayFunc(drawScene);  // 화면을 그리는 함수 등록
	glutReshapeFunc(Reshape);  // 화면 크기 변경 시 호출되는 함수 등록
	glutKeyboardFunc(keyBoard);  // 키보드 입력 처리 함수 등록
	glutMouseFunc(mouse);  // 마우스 입력 처리 함수 등록
	glutMainLoop();  // 이벤트 루프 시작

	return 0;  // 프로그램 정상 종료
}

// 화면을 그리는 함수
GLvoid drawScene() {

	glClearColor(1.0, 1.0, 1.0, 1.0f);  // 화면을 흰색으로 초기화
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 컬러 버퍼와 깊이 버퍼 초기화

	glUseProgram(shaderProgramID);  // 생성한 쉐이더 프로그램 사용

	glBindVertexArray(vao);  // VAO 바인딩

	// 인덱스 버퍼를 사용해 면 그리기
	glDrawElements(GL_QUADS, modelSqu.faces.size() * 4, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);  // VAO 바인딩 해제

	glutSwapBuffers();  // 더블 버퍼링 사용, 화면 갱신

	// OpenGL 오류 체크 루프
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error: " << err << endl;  // 오류 메시지 출력
	}
}

// 화면 크기가 변경될 때 호출되는 함수
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);  // 뷰포트 크기 설정
}

// 버텍스 쉐이더를 만드는 함수
void make_vertexShaders() {

	vertexSource = filetobuf("vertex.glsl");  // 버텍스 쉐이더 파일 읽기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);  // 버텍스 쉐이더 생성
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);  // 쉐이더 소스 코드 설정
	glCompileShader(vertexShader);  // 쉐이더 컴파일

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);  // 컴파일 성공 여부 확인

	if (!result) {  // 컴파일 실패 시
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);  // 오류 로그 받기
		cerr << "ERROR: vertex shader 컴파일 실패 \n" << errorLog << endl;  // 오류 메시지 출력
		return;
	}
}

// 프래그먼트 쉐이더를 만드는 함수
void make_fragmentShaders() {

	fragmentSource = filetobuf("fragment.glsl");  // 프래그먼트 쉐이더 파일 읽기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  // 프래그먼트 쉐이더 생성
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);  // 쉐이더 소스 코드 설정
	glCompileShader(fragmentShader);  // 쉐이더 컴파일

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);  // 컴파일 성공 여부 확인

	if (!result) {  // 컴파일 실패 시
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);  // 오류 로그 받기
		cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << endl;  // 오류 메시지 출력
		return;
	}
}

// 쉐이더 프로그램을 만드는 함수
void make_shaderProgram() {

	make_vertexShaders();  // 버텍스 쉐이더 생성
	make_fragmentShaders();  // 프래그먼트 쉐이더 생성

	shaderProgramID = glCreateProgram();  // 쉐이더 프로그램 생성
	glAttachShader(shaderProgramID, vertexShader);  // 버텍스 쉐이더 프로그램에 첨부
	glAttachShader(shaderProgramID, fragmentShader);  // 프래그먼트 쉐이더 프로그램에 첨부
	glLinkProgram(shaderProgramID);  // 쉐이더 프로그램 링크

	glDeleteShader(vertexShader);  // 버텍스 쉐이더 삭제 (이미 프로그램에 포함되었으므로)
	glDeleteShader(fragmentShader);  // 프래그먼트 쉐이더 삭제

	glUseProgram(shaderProgramID);  // 쉐이더 프로그램 사용
}

// 버퍼 초기화 함수
void InitBuffer() {
	glGenVertexArrays(1, &vao);  // VAO 생성
	glBindVertexArray(vao);  // VAO 바인딩

	glGenBuffers(3, vbo);  // VBO 생성 (2개)

	 // 1. 정점 데이터 전송
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, modelSqu.vertices.size() * sizeof(Vertex), &modelSqu.vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // 정점 속성 포인터 설정
	glEnableVertexAttribArray(0);  // 정점 속성 활성화

	// 2. 텍스처 좌표 데이터 전송 (텍스처 좌표가 있을 때만)
	if (!modelSqu.texCoords.empty()) {

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);  // 두 번째 VBO 바인딩 (색상 데이터 저장)
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat), modelSquColors, GL_STATIC_DRAW);  // 색상 데이터를 VBO에 복사
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);  // 색상 속성 포인터 설정
		glEnableVertexAttribArray(1);  // 속성 배열 활성화
	}

	// 3. 법선 벡터 데이터 전송 (법선 벡터가 있을 때만)
	if (!modelSqu.normals.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, modelSqu.normals.size() * sizeof(Normal), &modelSqu.normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);  // 법선 속성 포인터 설정
		glEnableVertexAttribArray(2);  // 법선 속성 활성화
	}

}
