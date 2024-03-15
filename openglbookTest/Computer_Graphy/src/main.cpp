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
	// 一行3个顶点.共36 个顶点，12 个三角形，组成了放置在原点处的 2×2×2 立方体
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
	// 四棱锥有 18 个顶点， 由 6 个三角形组成（侧面 4 个，底面 2 个）
	float pyramidPositions[54] =
	{ -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,    //front
		1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,    //right
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  //back
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  //left
		-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, //LF
		1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f  //RR
	};
	/*glGenVertexArrays()和 glGenBuffers()这两个
		OpenGL 命令分别用于创建 VAO 和 VBO，并返回
		它们的整数型 ID，存进数组 vao 和 vbo*/
	glGenVertexArrays(1, vao);//顶点数组对象,即vao,使用一个顶点缓冲
	glBindVertexArray(vao[0]);//命令的目的是将指定的 VAO 标记为“活跃”，这样生成的缓冲区①就会和这个 VAO相关联
	glGenBuffers(numVBOs, vbo);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);//glBindBuffer()命令将缓冲区（例如，第 0个缓冲区）标记为“活跃”
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
	/*第一个参数定义了fov的值，它表示的是视野(Field of View)，并且设置了观察空间的大小。
	如果想要一个真实的观察效果，它的值通常设置为45.0f，但想要一个末日风格的结果你可以将其设置一个更大的值。
	第二个参数设置了宽高比，由视口的宽除以高所得。第三和第四个参数设置了平截头体的近和远平面。
	我们通常设置近距离为0.1f，而远距离设为100.0f。所有在近平面和远平面内且处于平截头体内的顶点都会被渲染。*/
	
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 12.0f;
	setupVertices();

}
void display(GLFWwindow* window, double currentTime)
{
	/*清空*/
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	/*display()函数所做的事情中包含调用
glUseProgram()，用于将含有两个已编译着色器的程序载入 OpenGL 管线阶段（在 GPU 上！）
glUseProgram()并没有运行着色器，它只是将着色器加载进硬件。*/
	glUseProgram(renderingProgram);//在Cherno Opengl中这个函数在iImgGUI中
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	/*。GLM 函数调用 value_ptr()返回对矩阵数据的引用，
	glUniformMatrix4fv()需要将这些矩阵数据传递给统一变量。*/
	mvStack.push(vMat);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	//----------------------  pyramid == sun,四棱锥 == 太阳
	/*“*=”运算符在 mat4 中被重载，因此可以用于连接矩阵。因此，我们通常将它
用于向矩阵栈顶部的矩阵添加平移、旋转等操作。*/
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // 太阳位置
	//push()压入的完整 MV 矩阵，也包括它的自转。
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0f, 0.0f, 0.0f));// 太阳旋转
	//glm::value_ptr(mvStack.top())获取 MV 矩阵，然后将其发送到 MV 统一变量
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);// 将第 0 个属性关联到缓冲区
	glEnableVertexAttribArray(0);// 启用第 0 个顶点属性
	glEnable(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 18); // 绘制太阳
	mvStack.pop(); // 从栈中移除太阳的自转
	//----------------------- 大立方体 == 地球 --------------------------------------------- 
	mvStack.push(mvStack.top());
	mvStack.top() *=
		glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0, 0.0f, cos((float)
			currentTime) * 4.0));
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	// 地球旋转
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 36); // 绘制地球
	mvStack.pop(); // 从栈中移除地球的自转
	//----------------------- 小立方体 == 月球 ----------------------------------- 
	mvStack.push(mvStack.top());
	mvStack.top() *=
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime) * 2.0,
			cos((float)currentTime) * 2.0));
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
	// 月球旋转
	mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)); // 让月球小一些
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 36); // 绘制月球
	// 从栈中移除月球缩放、旋转、位置矩阵，地球位置矩阵，太阳位置矩阵和视图矩阵
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