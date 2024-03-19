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

//用于显示窗口的参数，包括长宽高,MV和透视
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

GLuint brickTexture;
void setupVertices(void)
{
	// 四棱锥有 18 个顶点， 由 6 个三角形组成（侧面 4 个，底面 2 个）
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
	glGenVertexArrays(numVAOs, vao); // 顶点数组对象, 即vao, 使用一个顶点缓冲
	glBindVertexArray(vao[0]);

	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);//
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);//注意glBindBuffer(GL_ARRAY_BUFFER, vbo[1])写错为glBindBuffer(numVBOs, vbo[1])不显示图案
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
}
void init(GLFWwindow* window)
{//
	renderingProgram = Utils::createShaderProgram("res/Shaders/vertShader.glsl", "res/Shaders/fragShader.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 4.0f;//注意不是cameraZ = 0.0f
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
	//把 mv 和投影矩阵发送
	//到两个统一变量（即 mv_matrix 和 proj_matrix）中
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");//// 获取着色器程序中统一变量的位置
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(pyrLocX, pyrLocY, pyrLocZ));
	//glm::rotate(mat4, θ, x, y, z)，用于构建绕轴(x, y, z)旋转 θ 度的矩阵；
	mMat = glm::rotate(mMat, -0.45f, glm::vec3(1.0f, 0.0f, 0.0f));
	mMat = glm::rotate(mMat, 0.61f, glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::rotate(mMat, 0.00f, glm::vec3(0.0f, 0.0f, 1.0f));

	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	//https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/
	/*第一个参数指定我们要配置的顶点属性。还记得我们在顶点着色器中使用layout(location = 0)定义了position顶点属性的位置值(Location)吗？
	它可以把顶点属性的位置值设置为0。因为我们希望把数据传递到这一个顶点属性中，所以这里我们传入0。
第二个参数指定顶点属性的大小。顶点属性是一个vec3，它由3个值组成，所以大小是3。
第三个参数指定数据的类型，这里是GL_FLOAT(GLSL中vec*都是由浮点数值组成的)。
下个参数定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，
所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE。
第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔。
由于下个组位置数据在3个float之后，我们把步长设置为3 * sizeof(float)。
要注意的是由于我们知道这个数组是紧密排列的（在两个顶点属性之间没有空隙）我们也可以设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）。
一旦我们有更多的顶点属性，我们就必须更小心地定义每个顶点属性之间的间隔，我们在后面会看到更多的例子（译注: 这个参数的意思简单说就是从这个属性第二次出现的地方到整个数组0位置之间有多少字节）。
最后一个参数的类型是void*，所以需要我们进行这个奇怪的强制类型转换。它表示位置数据在缓冲中起始位置的偏移量(Offset)。由于位置数据在数组的开头，所以这里是0。我们会在后面详细解释这个参数。*/
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);//启用顶点属性

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);//纹理是vec2
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