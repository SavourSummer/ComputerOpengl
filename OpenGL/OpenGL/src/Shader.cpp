#include "Shader.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

#include"Renderer.h"



Shader::Shader(const std::string& filepath)
    :m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);

}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind()
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::UnBind()
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}



ShaderProgramSource Shader:: ParseShader(const std::string& filePath)
{
    std::ifstream stream(filePath); /* ����û�ж��ļ��Ƿ��������� is_open */
    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) { /* �ҵ�#shader��� */
            if (line.find("vertex") != std::string::npos) { /* ������ɫ����� */
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) { /* Ƭ����ɫ����� */
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(),ss[1].str() };
}

 unsigned int Shader:: CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id;
    /* ���������� */
    GLCall(id = glCreateShader(type)); /* ������Ӧ���͵���ɫ�� */
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr)); /* ������ɫ��Դ�� */
    GLCall(glCompileShader(id)); /* ������ɫ�� */

    /* ��������� */
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result)); // ��ȡ��ǰ��ɫ������״̬
    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length)); // ��ȡ��־����
        char* msg = (char*)_malloca(length * sizeof(char)); /* Cherno������õ�alloca, ����IDE��ʾ, ������ĳ���_malloca���� */
        GLCall(glGetShaderInfoLog(id, length, &length, msg)); // ��ȡ��־��Ϣ
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << msg << std::endl;
        GLCall(glDeleteShader(id)); // ɾ����ɫ��
        return 0;
    }

    return id;
}

 unsigned int Shader:: CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program;
    GLCall(program = glCreateProgram()); /* �������� */
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    /* ����ɫ�����ӵ������� */
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program)); /* ���ӳ��� */
    GLCall(glValidateProgram(program)); /* ��֤ */

    /* ɾ����ɫ�� */
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}
 int Shader::GetUniformLocation(const std::string& name)//���ڽ���ͳһ����u_Colorλ��
 {
     if (m_UniformLocationCache.find(name)!=m_UniformLocationCache.end())
     {
         return m_UniformLocationCache[name];

     }
     int location;
     GLCall(location = glGetUniformLocation(m_RendererID, name.c_str()));
     if (location == -1)
     {
         std::cout << "Warning: uniform' " << name << "'doesn't exist" << std::endl;

     }
     m_UniformLocationCache[name] = location;
     return location;
 }