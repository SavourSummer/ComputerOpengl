#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Utils.h"

#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
float pyrLocX, pyrLocY, pyrLocZ;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

//������ʾ���ڵĲ��������������,MV��͸��
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

GLuint brickTexture;
void setupVertices(void)
{
	// ����׶�� 18 �����㣬 �� 6 ����������ɣ����� 4 �������� 2 ����
	float pyramidPositions[54] = {
		-1.0f, -1.0f, 1.0f,		 1.0f,-1.0f, 1.0f,		0.0f, 1.0f, 0.0f,    //front
		1.0f, -1.0f, 1.0f,		1.0f,-1.0f, -1.0f,		0.0f, 1.0f, 0.0f,    //right
		1.0f, -1.0f, -1.0f,		-1.0f,-1.0f, -1.0f,		0.0f, 1.0f, 0.0f,  //back
		-1.0f, -1.0f, -1.0f,	-1.0f,-1.0f, 1.0f,		0.0f, 1.0f, 0.0f,  //left
		-1.0f, -1.0f, -1.0f,	 1.0f,-1.0f, 1.0f,		-1.0f, -1.0f, 1.0f, //LF
		1.0f, -1.0f, 1.0f,		-1.0f,-1.0f, -1.0f,		 1.0f, -1.0f, -1.0f  //RR
	};
	float textureCoordinates[36] = {
		0.0f, 0.0f,					1.0f,0.0f,				0.5f, 1.0f,
		0.0f, 0.0f,					1.0f,0.0f,				0.5f, 1.0f,
		0.0f, 0.0f,					1.0f,0.0f,				0.5f, 1.0f,
		0.0f, 0.0f,					1.0f,0.0f,				0.5f, 1.0f,
		0.0f, 0.0f,					1.0f,1.0f,				0.0f, 1.0f,
		1.0f, 1.0f,					0.0f,0.0f,				1.0f, 0.0f
	};
	glGenVertexArrays(numVAOs, vao); // �����������, ��vao, ʹ��һ�����㻺��
	glBindVertexArray(vao[0]);

	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);//
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);//ע��glBindBuffer(GL_ARRAY_BUFFER, vbo[1])д��ΪglBindBuffer(numVBOs, vbo[1])����ʾͼ��
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
}
void init(GLFWwindow* window)
{//
	renderingProgram = Utils::createShaderProgram("res/Shaders/vertShader.glsl", "res/Shaders/fragShader.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 4.0f;//ע�ⲻ��cameraZ = 0.0f
	pyrLocX = 0.0f; pyrLocY = 0.0f; pyrLocZ = 0.0f;
	setupVertices();

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);//1.0472f=60degree

	brickTexture = Utils::loadTexture("res/Textures/brick1.jpg");
}
void display(GLFWwindow* window, double currenTime)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);
	//�� mv ��ͶӰ������
	//������ͳһ�������� mv_matrix �� proj_matrix����
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");//// ��ȡ��ɫ��������ͳһ������λ��
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(pyrLocX, pyrLocY, pyrLocZ));
	//glm::rotate(mat4, ��, x, y, z)�����ڹ�������(x, y, z)��ת �� �ȵľ���
	mMat = glm::rotate(mMat, -0.45f, glm::vec3(1.0f, 0.0f, 0.0f));
	mMat = glm::rotate(mMat, 0.61f, glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::rotate(mMat, 0.00f, glm::vec3(0.0f, 0.0f, 1.0f));

	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	//https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/
	/*��һ������ָ������Ҫ���õĶ������ԡ����ǵ������ڶ�����ɫ����ʹ��layout(location = 0)������position�������Ե�λ��ֵ(Location)��
	�����԰Ѷ������Ե�λ��ֵ����Ϊ0����Ϊ����ϣ�������ݴ��ݵ���һ�����������У������������Ǵ���0��
�ڶ�������ָ���������ԵĴ�С������������һ��vec3������3��ֵ��ɣ����Դ�С��3��
����������ָ�����ݵ����ͣ�������GL_FLOAT(GLSL��vec*�����ɸ�����ֵ��ɵ�)��
�¸��������������Ƿ�ϣ�����ݱ���׼��(Normalize)�������������ΪGL_TRUE��
�������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮�䡣���ǰ�������ΪGL_FALSE��
�����������������(Stride)�������������������Ķ���������֮��ļ����
�����¸���λ��������3��float֮�����ǰѲ�������Ϊ3 * sizeof(float)��
Ҫע�������������֪����������ǽ������еģ���������������֮��û�п�϶������Ҳ��������Ϊ0����OpenGL�������岽���Ƕ��٣�ֻ�е���ֵ�ǽ�������ʱ�ſ��ã���
һ�������и���Ķ������ԣ����Ǿͱ����С�ĵض���ÿ����������֮��ļ���������ں���ῴ����������ӣ���ע: �����������˼��˵���Ǵ�������Եڶ��γ��ֵĵط�����������0λ��֮���ж����ֽڣ���
���һ��������������void*��������Ҫ���ǽ��������ֵ�ǿ������ת��������ʾλ�������ڻ�������ʼλ�õ�ƫ����(Offset)������λ������������Ŀ�ͷ������������0�����ǻ��ں�����ϸ�������������*/
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);//���ö�������

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);//������vec2
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brickTexture);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, 18);
}
void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 5 - program 1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}