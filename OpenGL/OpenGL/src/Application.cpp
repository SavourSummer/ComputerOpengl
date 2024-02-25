#include<iostream>
#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<string>
#include<fstream>
#include<sstream>

#include"Renderer.h"
#include"IndexBuffer.h"
#include"VertexBuffer.h"
#include"VertexArray.h"
#include"Shader.h"
#include "Texture.h"
//注意生成的exe同一目录须有res/shaders/Basic.shader




int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    //设置Opengl版本3.3，原4.6,并且设置核心版本，查看兼容性
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 6);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    /**
     * 交换间隔，交换缓冲区之前等待的帧数，通常称为v-sync
     * 默认情况下，交换间隔为0
     * 这里设置为1，即每帧更新一次
     **/
    glfwSwapInterval(1);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

    unsigned char* glVersion;
    GLCall(glVersion = (unsigned char*)glGetString(GL_VERSION));
    std::cout << "Status: Using GL " << glVersion << std::endl;

    /* 顶点位置浮点型数组 */
    //float positions[] = {
    //    -0.5f, -0.5f, // 0
    //    0.5f, -0.5f,  // 1
    //    0.5f, 0.5f,   // 2
    //    -0.5f, 0.5f,  // 3
    //};
    //注意只有矩形坐标与纹理坐标对应才有正确的图像
    float positions[] = {
            0.0f, 0.0f, 0.0f, 0.0f, // 0
            1.0f, 0.0f, 1.0f, 0.0f,  // 1
            1.0f, 1.0f, 1.0f, 1.0f,    // 2
            0.0f, 1.0f, 0.0f, 1.0f   // 3
    };
    /* 索引缓冲区所需索引数组 */
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    VertexArray va;
    VertexBuffer vb(positions,4*4*sizeof(float));//把4*2改为4*4因为现在有4个浮点数了

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);
    
    //unsigned int vao;//设置Vertex Array Object,用于统一大量绘制的中介
    //GLCall(glGenVertexArrays(1, &vao));
    //GLCall(glBindVertexArray(vao));
    //VertexBuffer vb(positions, 4 * 2 * sizeof(float));
    //unsigned int buffer;
    //GLCall(glGenBuffers(1, &buffer)); /* 生成缓冲区 */
    //GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); /* 绑定缓冲区 */
    //GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW)); /* 设置缓冲区数据 */

    //GLCall(glEnableVertexAttribArray(0)); /* 激活顶点属性-索引0-位置 */
    //GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0)); /* 设置顶点属性-索引0 */

    /*unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));*/
    IndexBuffer ib(indices, 6);

    /* 从文件中解析着色器源码 */
    //ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    //unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    //GLCall(glUseProgram(shader)); /* 使用着色器程序 */


    //int location;
    //GLCall(location = glGetUniformLocation(shader, "u_Color")); /* 获取指定名称统一变量的位置 */
    //ASSERT(location != -1);
    //GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f)); /* 设置对应的统一变量 */
    Shader shader("res/shaders/Basic.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f,1.0f);

    Texture texture("res/textures/ChernoLogo.png");
    texture.Bind();
    shader.SetUniform1i("u_Texture", 0);//把纹理传给0号插槽
    va.Bind();
    ib.Bind();
    Renderer renderer;
    //解绑
    /*GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));*/
    //GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
    //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        //GLCall(glClear(GL_COLOR_BUFFER_BIT));
        renderer.Clear();
       // GLCall(glUseProgram(shader));
        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
       // GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
       // GLCall(glBindVertexArray(vao));
        /* 绘制 */
       /* GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));*/
        vb.Bind();
       // GLCall(glEnableVertexAttribArray(0));
       // GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

        ib.Bind();

        //GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        renderer.Draw(va, ib, shader);
        if (r > 1.0f) {
            increment = -0.05f;
        }
        else if (r < 0.0f) {
            increment = 0.05f;
        }
        r += increment;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    //GLCall(glDeleteProgram(shader)); /* 删除着色器程序 */

    glfwTerminate();
    return 0;
}