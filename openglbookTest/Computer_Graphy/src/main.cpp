#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Utils.h"
#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
// variable allocation for display
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;
std::stack<glm::mat4>mvStack;

void setupVertices(void) {
	// һ��3������.��36 �����㣬12 �������Σ�����˷�����ԭ�㴦�� 2��2��2 ������
	float vertexPositions[108] =
	{ -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};
	// ����׶�� 18 �����㣬 �� 6 ����������ɣ����� 4 �������� 2 ����
	float pyramidPositions[54] =
	{ -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,    //front
		1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,    //right
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  //back
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  //left
		-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, //LF
		1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f  //RR
	};
	/*glGenVertexArrays()�� glGenBuffers()������
		OpenGL ����ֱ����ڴ��� VAO �� VBO��������
		���ǵ������� ID��������� vao �� vbo*/
	glGenVertexArrays(1, vao);//�����������,��vao,ʹ��һ�����㻺��
	glBindVertexArray(vao[0]);//�����Ŀ���ǽ�ָ���� VAO ���Ϊ����Ծ�����������ɵĻ������پͻ����� VAO�����
	glGenBuffers(numVBOs, vbo);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);//glBindBuffer()��������������磬�� 0�������������Ϊ����Ծ��
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions),vertexPositions,GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);

}
void init(GLFWwindow* window)
{
	renderingProgram = Utils::createShaderProgram("res/vertShader.glsl", "res/fragShader.glsl");
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	//glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
	/*��һ������������fov��ֵ������ʾ������Ұ(Field of View)�����������˹۲�ռ�Ĵ�С��
	�����Ҫһ����ʵ�Ĺ۲�Ч��������ֵͨ������Ϊ45.0f������Ҫһ��ĩ�շ��Ľ������Խ�������һ�������ֵ��
	�ڶ������������˿�߱ȣ����ӿڵĿ���Ը����á������͵��ĸ�����������ƽ��ͷ��Ľ���Զƽ�档
	����ͨ�����ý�����Ϊ0.1f����Զ������Ϊ100.0f�������ڽ�ƽ���Զƽ�����Ҵ���ƽ��ͷ���ڵĶ��㶼�ᱻ��Ⱦ��*/
	
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 12.0f;
	setupVertices();

}
void display(GLFWwindow* window, double currentTime)
{
	/*���*/
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	/*display()���������������а�������
glUseProgram()�����ڽ����������ѱ�����ɫ���ĳ������� OpenGL ���߽׶Σ��� GPU �ϣ���
glUseProgram()��û��������ɫ������ֻ�ǽ���ɫ�����ؽ�Ӳ����*/
	glUseProgram(renderingProgram);//��Cherno Opengl�����������iImgGUI��
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	/*��GLM �������� value_ptr()���ضԾ������ݵ����ã�
	glUniformMatrix4fv()��Ҫ����Щ�������ݴ��ݸ�ͳһ������*/
	mvStack.push(vMat);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	//----------------------  pyramid == sun,����׶ == ̫��
	/*��*=��������� mat4 �б����أ���˿����������Ӿ�����ˣ�����ͨ������
���������ջ�����ľ������ƽ�ơ���ת�Ȳ�����*/
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // ̫��λ��
	//push()ѹ������� MV ����Ҳ����������ת��
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0f, 0.0f, 0.0f));// ̫����ת
	//glm::value_ptr(mvStack.top())��ȡ MV ����Ȼ���䷢�͵� MV ͳһ����
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);// ���� 0 �����Թ�����������
	glEnableVertexAttribArray(0);// ���õ� 0 ����������
	glEnable(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 18); // ����̫��
	mvStack.pop(); // ��ջ���Ƴ�̫������ת
	//----------------------- �������� == ���� --------------------------------------------- 
	mvStack.push(mvStack.top());
	mvStack.top() *=
		glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0, 0.0f, cos((float)
			currentTime) * 4.0));
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	// ������ת
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 36); // ���Ƶ���
	mvStack.pop(); // ��ջ���Ƴ��������ת
	//----------------------- С������ == ���� ----------------------------------- 
	mvStack.push(mvStack.top());
	mvStack.top() *=
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime) * 2.0,
			cos((float)currentTime) * 2.0));
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
	// ������ת
	mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)); // ������СһЩ
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 36); // ��������
	// ��ջ���Ƴ��������š���ת��λ�þ��󣬵���λ�þ���̫��λ�þ������ͼ����
	mvStack.pop(); mvStack.pop(); mvStack.pop(); mvStack.pop();
}
void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}
int main(void)
{
	if (!glfwInit()) { return -1; }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 4 - program 4", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { return -1; }
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
	return 0;
}