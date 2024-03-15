#define GLM_ENABLE_EXPERIMENTAL
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/soil2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtx/euler_angles.hpp>
#include "Utils.h"
Utils::Utils()
{

}

void Utils::displayComputeShaderLimits()
{
	int work_grp_cnt[3];
	int work_grp_siz[3];
	int work_grp_inv;
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_siz[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_siz[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_siz[2]);
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("maximum number of workgroups is: %i  %i  %i\n", work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);
	printf("maximum size of workgroups is: %i  %i  %i\n", work_grp_siz[0], work_grp_siz[1], work_grp_siz[2]);
	printf("max local work group invocations %i\n", work_grp_inv);
}
std::string Utils::readShaderFile(const std::string& filePath)
  {
	//std::string content;
	//std::ifstream fileStream(filePath);

	//std::string line ;

	//while (getline(fileStream, line))
	//{
	//	//getline(fileStream, line);
	//	content.append(line + "\n");
	//}
	//fileStream.close();
	//return content;
	std::string content;

	std::ifstream fileStream(filePath, std::ios::in);
	if (!fileStream.is_open())
	{
		throw "Unable to open file.";
	}
	std::string line = "";
	while (!fileStream.eof())
	{
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}
bool Utils::checkOpenGLError()
{
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		std::cout << "glError:" << glErr << std::endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

//GLSL 函数 glGetShaderiv()和
//glGetProgramiv()用于提供有关编译过的 GLSL 着色器和程序的信息。程序 2.3 还使用了程序 2.2
//中的 createShaderProgram()函数，不过加入了错误检测的调用。
//程序 2.3 中实现了如下 3 个实用功能。
//printShaderLog()：当 GLSL 代码编译失败时，显示 OpenGL 日志内容。
//printProgramLog()：当 GLSL 链接失败时，显示 OpenGL 日志内容。
//checkOpenGLError()：检查 OpenGL 错误标志，即是否发生 OpenGL 错误。
//checkOpenGLError()既用于检测 GLSL 代码编译错误，又用于检测 OpenGL 运行时的错误，
//因此我们强烈建议在整个 C++ / OpenGL 应用程序开发过程中使用它。例如，对于程序 2.2 中
//glCompileShader()和 glLinkProgram()的调用，我们可以很方便地使用程序 2.3 中的代码进行加强，
//以确保捕获到所有拼写错误和编译错误，同时知道错误原因。
void Utils::printShaderLog(GLuint shader)
{
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH,&len);
	if (len > 0)
	{
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		std::cout << "Shader info Log: " << log << std::endl;
		free(log);
	}
}
GLuint Utils::prepareShader(int shaderTYPE, const std::string& shaderPath)
{
	GLint shaderCompiled;
	std::string shaderStr = readShaderFile(shaderPath);
	const char* shaderSrc = shaderStr.c_str();
	GLint shaderRef = glCreateShader(shaderTYPE);/* 创建对应类型的着色器 shaderTYPE=NONE = -1, VERTEX = 0, FRAGMENT = 1。shaderRef=id*/
	if (shaderRef == 0 || shaderRef == GL_INVALID_ENUM)
	{
		printf("Error: Could not create shader \"%s\" of type:%d\n", shaderPath, shaderTYPE);
		return 0;
	}
	glShaderSource(shaderRef, 1, &shaderSrc, NULL);/* 设置着色器源码 */
	glCompileShader(shaderRef);
	checkOpenGLError();
	glGetShaderiv(shaderRef, GL_COMPILE_STATUS, &shaderCompiled);
	if (shaderCompiled != GL_TRUE)
	{
		if (shaderTYPE == GL_VERTEX_SHADER) std::cout << "Vertex ";
		if (shaderTYPE == GL_TESS_CONTROL_SHADER) std::cout << "Tess Control ";
		if (shaderTYPE == GL_TESS_EVALUATION_SHADER) std::cout << "Tess Eval ";
		if (shaderTYPE == GL_GEOMETRY_SHADER) std::cout << "Geometry ";
		if (shaderTYPE == GL_FRAGMENT_SHADER) std::cout << "Fragment ";
		if (shaderTYPE == GL_COMPUTE_SHADER) std::cout << "Compute ";
		std::cout << "shader compilation error for shader: '" << shaderPath << "'." << std::endl;
		printShaderLog(shaderRef);
	}


	//====================================
	// Custom Compilation Error Checking
	//====================================
	//checkOpenGLError();
	/*GLint compiled;
	glGetShaderiv(shaderRef, GL_COMPILE_STATUS, &compiled);
	if (compiled != GL_TRUE)
	{
		printf("Error: Failed to compile shader \"%s\".\n", shaderPath);

		GLint log_size = 0;
		glGetShaderiv(shaderRef, GL_INFO_LOG_LENGTH, &log_size);

		printf("Shader log length: %d\n", log_size);

		GLchar* info_log = (GLchar*)malloc(sizeof(GLchar)*log_size);
		glGetShaderInfoLog(shaderRef, log_size, &log_size, info_log);
		printf("Compilation Log: '%s'\n", info_log);
		printf("First 5 chars: %d %d %d %d %d\n", info_log[0], info_log[1], info_log[2], info_log[3], info_log[4]);
		free(info_log);
		glDeleteShader(shaderRef);
		return 0;

	}*/
	//====================================
	return shaderRef;
}

void Utils::printProgramLog(int prog)
{
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		std::cout << "Program Info Log: " << log << std::endl;
		free(log);
	}
}
int Utils::finalizeShaderProgram(GLuint sprogram)
{
	GLint linked;
	glLinkProgram(sprogram);//glLinkProgram()请求 GLSL 编译器
	checkOpenGLError();
	glGetProgramiv(sprogram, GL_LINK_STATUS, &linked);
	if (linked != 1)
	{
		std::cout << "linking failed" << std::endl;
		printProgramLog(sprogram);
	}
	return sprogram;
}
//glCreateProgram()创建程序对象，glAttachShader()将着色器加入程序对象，
//接着 glLinkProgram()请求 GLSL 编译器，以确保它们的兼容性。
GLuint Utils::createShaderProgram(const std::string& cp)
{
	GLuint cShader = prepareShader(GL_COMPUTE_SHADER, cp);
	GLuint cprogram = glCreateProgram();
	glAttachShader(cprogram, cShader);
	finalizeShaderProgram(cprogram);
	return cprogram;
}

GLuint Utils::createShaderProgram(const std::string& vp, const std::string& fp)
{
	GLuint vShader = prepareShader(GL_VERTEX_SHADER, vp);
	GLuint fShader = prepareShader(GL_FRAGMENT_SHADER, fp);
	GLuint vfprogram = glCreateProgram();
	glAttachShader(vfprogram, vShader);
	glAttachShader(vfprogram, fShader);
	finalizeShaderProgram(vfprogram);
	return vfprogram;
}

GLuint Utils::createShaderProgram(const char* vp, const char* gp, const char* fp)
{
	GLuint vShader = prepareShader(GL_VERTEX_SHADER, vp);
	GLuint gShader = prepareShader(GL_GEOMETRY_SHADER, gp);
	GLuint fShader = prepareShader(GL_FRAGMENT_SHADER, fp);
	GLuint vgfprogram = glCreateProgram();
	glAttachShader(vgfprogram, vShader);
	glAttachShader(vgfprogram, gShader);
	glAttachShader(vgfprogram, fShader);
	finalizeShaderProgram(vgfprogram);
	return vgfprogram;
}

GLuint Utils::createShaderProgram(const char* vp, const char* tCS, const char* tES, const char* fp)
{
	GLuint vShader = prepareShader(GL_VERTEX_SHADER, vp);
	GLuint tcShader = prepareShader(GL_TESS_CONTROL_SHADER, tCS);
	GLuint teShader = prepareShader(GL_TESS_EVALUATION_SHADER, tES);
	GLuint fShader = prepareShader(GL_FRAGMENT_SHADER, fp);
	GLuint vtfprogram = glCreateProgram();
	glAttachShader(vtfprogram, vShader);
	glAttachShader(vtfprogram, tcShader);
	glAttachShader(vtfprogram, teShader);
	glAttachShader(vtfprogram, fShader);
	finalizeShaderProgram(vtfprogram);
	return vtfprogram;
}

GLuint Utils::createShaderProgram(const char* vp, const char* tCS, const char* tES, char* gp, const char* fp)
{
	GLuint vShader = prepareShader(GL_VERTEX_SHADER, vp);
	GLuint tcShader = prepareShader(GL_TESS_CONTROL_SHADER, tCS);
	GLuint teShader = prepareShader(GL_TESS_EVALUATION_SHADER, tES);
	GLuint gShader = prepareShader(GL_GEOMETRY_SHADER, gp);
	GLuint fShader = prepareShader(GL_FRAGMENT_SHADER, fp);
	GLuint vtgfprogram = glCreateProgram();
	glAttachShader(vtgfprogram, vShader);
	glAttachShader(vtgfprogram, tcShader);
	glAttachShader(vtgfprogram, teShader);
	glAttachShader(vtgfprogram, gShader);
	glAttachShader(vtgfprogram, fShader);
	finalizeShaderProgram(vtgfprogram);
	return vtgfprogram;
}

GLuint Utils::loadCubeMap(const char* mapDir)
{
	GLuint textureRef;
	std::string xp = mapDir; xp = xp + "/xp.jpg";
	std::string xn = mapDir; xn = xn + "/xn.jpg";
	std::string yp = mapDir; yp = yp + "/yp.jpg";
	std::string yn = mapDir; yn = yn + "/yn.jpg";
	std::string zp = mapDir; zp = zp + "/zp.jpg";
	std::string zn = mapDir; zn = zn + "/zn.jpg";
	textureRef = SOIL_load_OGL_cubemap(xp.c_str(), xn.c_str(), yp.c_str(), yn.c_str(), zp.c_str(), zn.c_str(),
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (textureRef == 0) std::cout << "didnt find cube map image file" << std::endl;
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, textureRef);
	// reduce seams
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return textureRef;
}

GLuint Utils::loadTexture(const char* texImagePath)
{
	GLuint textureRef;
	textureRef = SOIL_load_OGL_texture(texImagePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (textureRef == 0) std::cout << "didnt find texture file " << texImagePath << std::endl;
	// ----- mipmap/anisotropic section
	glBindTexture(GL_TEXTURE_2D, textureRef);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	if (glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
		GLfloat anisoset = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);
	}
	// ----- end of mipmap/anisotropic section
	return textureRef;
}

// GOLD material - ambient, diffuse, specular, and shininess
float* Utils::goldAmbient() { static float a[4] = { 0.2473f, 0.1995f, 0.0745f, 1 }; return (float*)a; }
float* Utils::goldDiffuse() { static float a[4] = { 0.7516f, 0.6065f, 0.2265f, 1 }; return (float*)a; }
float* Utils::goldSpecular() { static float a[4] = { 0.6283f, 0.5559f, 0.3661f, 1 }; return (float*)a; }
float Utils::goldShininess() { return 51.2f; }

// SILVER material - ambient, diffuse, specular, and shininess
float* Utils::silverAmbient() { static float a[4] = { 0.1923f, 0.1923f, 0.1923f, 1 }; return (float*)a; }
float* Utils::silverDiffuse() { static float a[4] = { 0.5075f, 0.5075f, 0.5075f, 1 }; return (float*)a; }
float* Utils::silverSpecular() { static float a[4] = { 0.5083f, 0.5083f, 0.5083f, 1 }; return (float*)a; }
float Utils::silverShininess() { return 51.2f; }

// BRONZE material - ambient, diffuse, specular, and shininess
float* Utils::bronzeAmbient() { static float a[4] = { 0.2125f, 0.1275f, 0.0540f, 1 }; return (float*)a; }
float* Utils::bronzeDiffuse() { static float a[4] = { 0.7140f, 0.4284f, 0.1814f, 1 }; return (float*)a; }
float* Utils::bronzeSpecular() { static float a[4] = { 0.3936f, 0.2719f, 0.1667f, 1 }; return (float*)a; }
float Utils::bronzeShininess() { return 25.6f; }
