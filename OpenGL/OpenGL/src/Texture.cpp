#include "Texture.h"
#include "vendor/stb_image/stb_image.h"

Texture::Texture(const std::string& path)
	:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{

	/*
	glGenTextures(1, &m_RendererID)：
这行代码生成一个纹理对象，并将其ID存储在变量 m_RendererID 中。
纹理对象用于存储图像数据，以便在渲染过程中将其映射到几何图形上。
glBindTexture(GL_TEXTURE_2D, m_RendererID)：
这行代码将纹理对象绑定到当前的OpenGL上下文。
GL_TEXTURE_2D 表示我们正在操作2D纹理。
纹理参数设置：
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)：设置纹理的缩小过滤器为线性过滤。
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)：设置纹理的放大过滤器为线性过滤。
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)：设置纹理的水平包裹模式为边缘截断。
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)：设置纹理的垂直包裹模式为边缘截断。*/
	stbi_set_flip_vertically_on_load(1);// 将设置stb_image在加载图像时垂直翻转图像数据，以便与OpenGL的纹理坐标系统一致。这样，你就不需要手动翻转图像或纹理坐标了。
		m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);
	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	/*glTexImage2D 是 OpenGL 中的一个函数，用于定义二维纹理图像。让我们逐个解释这个函数的参数：

GL_TEXTURE_2D：这是一个纹理目标，表示我们正在操作的是一个二维纹理。
0：这是纹理的细节级别编号，通常用于多级别纹理（mipmap）。在这里，我们使用基本级别，因此为0。
GL_RGBA8：这是纹理的内部格式。在这里，我们使用RGBA颜色通道，每个通道8位。
m_Width 和 m_Height：这是纹理的宽度和高度，以像素为单位。
0：这是边框的宽度。通常情况下，我们不使用边框，因此为0。
GL_RGBA：这是纹理数据的格式，即纹理中每个像素的颜色通道顺序。在这里，我们使用RGBA。
GL_UNSIGNED_BYTE：这是纹理数据的数据类型，表示每个颜色通道的值是无符号字节。
m_LocalBuffer：这是指向纹理数据的指针，通常是一个数组。*/
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	if (m_LocalBuffer)
	{
		stbi_image_free(m_LocalBuffer);
	}
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));//激活纹理插槽
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind()
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}