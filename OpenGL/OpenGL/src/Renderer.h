#pragma once
#include<iostream>
#include"GL/glew.h"
#include"VertexArray.h"
#include"IndexBuffer.h"
#include"Shader.h"
class VertexArray;
class IndexBuffer;
class Shader;
//使用class 解决了 缺少类型说明符 - 假定为 int。注意: C++ 不支持默认 int，使用多个类头文件问题
/* 反斜杠后面不能有空格 */
#define ASSERT(x) if (!(x)) __debugbreak()

#define GLCall(x) do { \
    GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));\
 } while (0)

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
    void Clear() const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};