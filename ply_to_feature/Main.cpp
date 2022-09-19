#include <iostream>
#include "glHeaders.h"
#include "plyData.h"
#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"

using namespace std;

glm::mat4 projection;
glm::vec3 camPos(30, 0, 0);
glm::mat4 view;

bool glfwewInit(GLFWwindow** window, int width, int height) {
    if (!glfwInit()) return false; // glfw 초기화

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    *(window) = glfwCreateWindow(width, height, "Test window", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        cout << "failed" << endl;
        return false;
    }

    glfwMakeContextCurrent(*window); // 윈도우 컨텍스트 생성

    glfwSwapInterval(0); // 스왑 간격 : 0 설정하면 fps 제한X, 1 설정하면 fps 제한 60

    if (glewInit() != GLEW_OK) { // GLEW 초기호 실패하면 종료
        glfwTerminate();
        return false;
    }

    cout << glGetString(GL_VERSION) << endl; // OpenGL 버전

    return true;
}


int main(int argc, char* args[]) {

    GLFWwindow* window;
    int width = 1280, height = 960;

    if (!glfwewInit(&window, width, height)) return -1;

    AISTER_GRAPHICS_ENGINE::PLYdata plys("example/BBQSauce.ply");
    plys.print();
    AISTER_GRAPHICS_ENGINE::Texture tex("example/BBQSauce.png");

    AISTER_GRAPHICS_ENGINE::Shader sh;
    sh.initShaders("mesh_vertex.glsl", "mesh_frag.glsl");

    AISTER_GRAPHICS_ENGINE::plyRenderer renderer;
    renderer.SetShaderPLY(&plys, &sh, &tex);
    cout << "check" << endl;

    AISTER_GRAPHICS_ENGINE::Camera cam;
    cam.position = camPos;
    cam._far = 1000.f;
    
    cam.screenResolution = glm::vec2(width, height);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD);
    glDepthFunc(GL_LESS);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Camera Matrix Example//
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 4.5f);
    view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned char* frameImage = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 4);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDepthFunc(GL_LESS);
        renderer.Draw(cam, glm::vec4(1, 0, 0, 1));

        /*
            GL-get FrameBuffer to Texture
        */
        glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, frameImage);


        glfwSwapBuffers(window);
    }

    cv::Mat imgs(height, width, CV_8UC4);
    imgs.data = frameImage;
    cv::flip(imgs, imgs, 0);
    cv::imwrite("test.png", imgs);

    glfwDestroyWindow(window);
    glfwTerminate();

}