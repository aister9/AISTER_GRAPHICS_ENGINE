#pragma once
#include <iostream>
#include "glHeaders.h"


namespace AISTER_GRAPHICS_ENGINE {
    class Scene {
    public:
        GLFWwindow* window;

        bool glfwewInit(GLFWwindow** window, int width, int height) {
            if (!glfwInit()) return false; // glfw 초기화

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

            *(window) = glfwCreateWindow(width, height, "Test window", nullptr, nullptr);

            if (!window) {
                glfwTerminate();
                std::cout << "failed" << std::endl;
                return false;
            }

            glfwMakeContextCurrent(*window); // 윈도우 컨텍스트 생성

            glfwSwapInterval(0); // 스왑 간격 : 0 설정하면 fps 제한X, 1 설정하면 fps 제한 60

            if (glewInit() != GLEW_OK) { // GLEW 초기호 실패하면 종료
                glfwTerminate();
                return false;
            }

            std::cout << glGetString(GL_VERSION) << std::endl; // OpenGL 버전

            return true;
        }

        void glfwDraw() {
            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();

                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                /*
                    GL-get FrameBuffer to Texture
                */

                glfwSwapBuffers(window);
            }
        }
    };
}