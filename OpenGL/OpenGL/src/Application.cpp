//#include<iostream>
#include<GL/glew.h>
#include <GLFW/glfw3.h>
//#include<string>
//#include<fstream>
//#include<sstream>

#include"Renderer.h"
//#include"IndexBuffer.h"
//#include"VertexBuffer.h"
//#include"VertexArray.h"
//#include"Shader.h"
//#include "Texture.h"
//注意生成的exe同一目录须有res/shaders/Basic.shader

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Test/TestClearColor.h"
#include "Test/TestTexture2D.h"
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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
    // 下面是顶点个纹理
    ////  -100.0f, -100.0f, 0.0f, 0.0f, // 0
    //    100.0f, -100.0f, 1.0f, 0.0f,  // 1
    //    100.0f, 100.0f, 1.0f, 1.0f,    // 2
    //    -100.0f, 100.0f, 0.0f, 1.0f   // 3
    // 100.0f, 100.0f, 0.0f, 0.0f, // 0
    //200.0f, 100.0f, 1.0f, 0.0f,  // 1
    //    200.0f, 200.0f, 1.0f, 1.0f,    // 2
    //    100.0f, 200.0f, 0.0f, 1.0f   // 3
    ////};
    //注意只有矩形坐标与纹理坐标对应才有正确的图像
    //float positions[] = {
    //        -50.0f, -50.0f, 0.0f, 0.0f, // 0
    //         50.0f, -50.0f, 1.0f, 0.0f, // 1
    //         50.0f, 50.0f, 1.0f, 1.0f,  // 2
    //        -50.0f, 50.0f, 0.0f, 1.0f   // 3
    //        
    //       
    //};//将C放置于x=0,y=0点，即左下角位置
    /* 索引缓冲区所需索引数组 */
   /* unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };*/
    /**
         * 混合:
         * 将输出颜色(判断着色器输出的颜色)和目标缓冲区已有的颜色结合
         * glEnable/glDisable(启用&关闭) => glBlendFunc(指定颜色因子) => glBlendEquation(指定混合模式)
         * glBlendEquation(mode) mode: src和dest的混合方式(默认GL_FUNC_ADD, 叠加)
         *
         **/
         /* 启用混合(默认不会启用) */
    GLCall(glEnable(GL_BLEND));
    /**
         * glBlendFunc(src, dest) 指定颜色因子
         * src 指定输出颜色(RGBA)因子的计算方式, 默认为GL_ONE
         * dest 指定目标颜色因子的计算方式, 默认为GL_ZERO
         * GL_SRC_ALPHA 因为src的alpha为0, GL_ONE_MINUS_SRC_ALPHA 1-src.alpha
         * RGBA = Srgba * GL_SRC_ALPHA + Drgba * GL_ONE_MINUS_SRC_ALPHA
         **/
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    //VertexArray va;
    //VertexBuffer vb(positions,4*4*sizeof(float));//把4*2改为4*4因为现在有4个浮点数了

    //VertexBufferLayout layout;
    //layout.Push<float>(2);
    //layout.Push<float>(2);
    //va.AddBuffer(vb, layout);
    //
    ////unsigned int vao;//设置Vertex Array Object,用于统一大量绘制的中介
    ////GLCall(glGenVertexArrays(1, &vao));
    ////GLCall(glBindVertexArray(vao));
    ////VertexBuffer vb(positions, 4 * 2 * sizeof(float));
    ////unsigned int buffer;
    ////GLCall(glGenBuffers(1, &buffer)); /* 生成缓冲区 */
    ////GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); /* 绑定缓冲区 */
    ////GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW)); /* 设置缓冲区数据 */

    ////GLCall(glEnableVertexAttribArray(0)); /* 激活顶点属性-索引0-位置 */
    ////GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0)); /* 设置顶点属性-索引0 */

    ///*unsigned int ibo;
    //GLCall(glGenBuffers(1, &ibo));
    //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    //GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));*/
    //IndexBuffer ib(indices, 6);
    ////glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    ///* 这里应该是 960x720 而不是 960x540 的分辨率 */
    //glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 720.0f, -1.0f, 1.0f);
    ////glm::vec4 vp(100.0f, 100.0f, 0.0f, 1.0f);
    ///* 相机位置 视图矩阵 x&y&z */
    ////glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));
    //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    ///* 模型矩阵 对象位置 */
    ////glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));
    ////glm::mat4 mvp = proj * view * model; /* 模型视图投影矩阵 */

    ///* 从文件中解析着色器源码 */
    ////ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    ////unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    ////GLCall(glUseProgram(shader)); /* 使用着色器程序 */


    //int location;
    //GLCall(location = glGetUniformLocation(shader, "u_Color")); /* 获取指定名称统一变量的位置 */
    //ASSERT(location != -1);
    //GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f)); /* 设置对应的统一变量 */
    //Shader shader("res/shaders/Basic.shader");
    //shader.Bind();
    //shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f,1.0f);
    ////shader.SetUniformMat4f("u_MVP", proj
    ////shader.SetUniformMat4f("u_MVP", mvp);//不绑定，以生成两个C
    //Texture texture("res/textures/ChernoLogo.png");
    //texture.Bind();
    //shader.SetUniform1i("u_Texture", 0);//把纹理传给0号插槽
    ///* 解绑 */
    //va.UnBind();
    //shader.UnBind();
    //vb.UnBind();
    //ib.UnBind();
    Renderer renderer;


    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::StyleColorsDark();

    // 需要指定glsl版本, 也就是shader中的version
    const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

    //解绑
    /*GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));*/
    //GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
    //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    /*float r = 0.0f;
    float increment = 0.05f;*/
   // test::TestClearColor test;
    //这行代码创建了一个新的 test::TestMenu 对象，并将 currentTest 作为参数传递给 test::TestMenu 的构造函数。然后，它将新创建的 test::TestMenu 对象的地址赋值给 testMenu 指针。

    //    这意味着 testMenu 现在指向一个 test::TestMenu 对象，这个对象持有一个指向 currentTest 所指所指向的 Test 对象的引用。这样，你就可以通过 testMenu 来访问和操作 currentTest。
    test::Test* currentTest = nullptr;
    test::TestMenu* testMenu = new test::TestMenu(currentTest);
    currentTest = testMenu;

    testMenu->RegisterTest<test::TestClearColor>("ClearColor");
    testMenu->RegisterTest<test::TestTexture2D>("2D Texture");
    /*glm::vec3 tranlation(200, 100, 0);*///设置C的初始位置
    //glm::vec3 tranlationA(200, 100, 0);//生成两个C坐标
    //glm::vec3 tranlationB(400, 100, 0);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        /* Render here */
        //GLCall(glClear(GL_COLOR_BUFFER_BIT));
        renderer.Clear();
        //test.OnUpdate(0.0f);
       // test.OnRender();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

       // {
       //     glm::mat4 model = glm::translate(glm::mat4(1.0f), tranlationA);
       //     //glm::mat4 mvp = proj * view * model;

       // glm::mat4 mvp = proj * view * model; /* 模型视图投影矩阵 */
       //// GLCall(glUseProgram(shader));
       // shader.Bind();
       //// shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
       // shader.SetUniformMat4f("u_MVP", mvp); /*  */
       //// GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
       //// GLCall(glBindVertexArray(vao));
       // /* 绘制 */
       ///* GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
       // GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));*/
       // //va.Bind();
       // //vb.Bind();
       //// GLCall(glEnableVertexAttribArray(0));
       //// GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

       // //ib.Bind();

       // //GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
       // renderer.Draw(va, ib, shader);
       // }
       // {
       //     glm::mat4 model = glm::translate(glm::mat4(1.0f), tranlationB);
       //     glm::mat4 mvp = proj * view * model;

       //     shader.Bind();
       //     shader.SetUniformMat4f("u_MVP", mvp);

       //     renderer.Draw(va, ib, shader);
       // }
        /*if (r > 1.0f) {
            increment = -0.05f;
        }
        else if (r < 0.0f) {
            increment = 0.05f;
        }
        r += increment;*/
        //{
        //    ImGui::Begin("ImGui");
        //    ImGui::SliderFloat3("TranlationA", &tranlationA.x, 0.0f, 960.0f);
        //    ImGui::SliderFloat3("TranlationB", &tranlationB.x, 0.0f, 960.0f);//设置可移动距离，包括x和y
        //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //    ImGui::End();
        //}
        //test.OnImGuiRender();
        if (currentTest)
        {
            currentTest->OnUpdate(0.0f);
            currentTest->OnRender();
            ImGui::Begin("ImGui-Test");
            if (currentTest != testMenu && ImGui::Button("<-"))
            {
                delete currentTest;
                currentTest = testMenu;
            }
            currentTest->OnImGuiRender();
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    delete currentTest;
    if (currentTest != testMenu)
    {
        delete testMenu;
    }
    //GLCall(glDeleteProgram(shader)); /* 删除着色器程序 */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
