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
#include"Sphere.h"

#define numVAOs 1
#define numVBOs 3
float cameraX, cameraY, cameraZ;
float sphLocX, sphLocY, sphLocZ;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

//������ʾ�ı���
GLuint mvLoc,projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;
//
GLuint earthTexture;
float rotAmt = 0.0f;
Sphere mysphere = Sphere(48);
void setupVertices(void)
{
	std::vector<int> ind = mysphere.getIndices();
	std::vector<glm::vec3> vert = mysphere.getVertices();
	std::vector<glm::vec2> tex = mysphere.getTexCoords();
	std::vector<glm::vec3> norm = mysphere.getNormals();

	std::vector<float> pvalues;// ����λ��
	std::vector<float> tvalues;// ��������
	std::vector<float> nvalues;// ������

	int numIndices = mysphere.getNumIndices();//��ȡ��������

	for (int i = 0; i < numIndices; i++)
	{
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);
		tvalues.push_back((tex[ind[i]]).s);
		tvalues.push_back((tex[ind[i]]).t);
		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
	}
	glGenVertexArrays(1, vao);// �����������, ��vao, ʹ��һ�����㻺��.�����ĵ�һ������������Ҫ���ɵ�VAO���������ڶ���������һ��ָ��
	glBindVertexArray(vao[0]);//֮ǰ���ɵ�VAO�󶨵���ǰ��OpenGL�������С�����ζ���������ڿ��Կ�ʼ�����VAO�����������
	
	glGenBuffers(numVBOs, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() *4, &nvalues[0], GL_STATIC_DRAW);
}

void init(GLFWwindow* window)
{
	renderingProgram = Utils::createShaderProgram("res/Shaders/vertShader.glsl", "res/Shaders/fragShader.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 2.0f;
	sphLocX = 0.0f; sphLocY = 0.0f; sphLocZ = -1.0f;

	glfwGetFramebufferSize (window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	setupVertices();

	earthTexture = Utils::loadTexture("res/Textures/earth.jpg");

}

void display(GLFWwindow* window, double currentTiem)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(sphLocX, sphLocY, sphLocZ));

	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTexture);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);//����OpenGL��������ƹ���GL_CCW ��ʾ��ʱ�루Counter Clock-Wise������˳�������Ϊ���档
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mysphere.getNumIndices());
}
//һ�����ڴ�С�ı�Ļص������������ڴ�С�ı�ʱ�����ᱻ����
void window_size_callback(GLFWwindow* win, int newWidth, int newHeight)
{
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void)
{
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 6 - program 1", NULL, NULL);//������������ָ����ȫ��ģʽ��Ҫʹ�õ���ʾ����Ҫ������Դ���������ڡ����ﶼ����ΪNULL����ʾ���ڲ���ȫ���ģ��Ҳ����������ڹ�����Դ��
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);
	glfwSetWindowSizeCallback(window, window_size_callback);
	init(window);
	while (!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

