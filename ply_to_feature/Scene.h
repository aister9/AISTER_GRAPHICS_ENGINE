#pragma once
#include <iostream>
#include "glHeaders.h"


namespace AISTER_GRAPHICS_ENGINE {
    class Scene {
    public:
        GLFWwindow* window;

        bool glfwewInit(GLFWwindow** window, int width, int height) {
            if (!glfwInit()) return false; // glfw �ʱ�ȭ

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

            glfwMakeContextCurrent(*window); // ������ ���ؽ�Ʈ ����

            glfwSwapInterval(0); // ���� ���� : 0 �����ϸ� fps ����X, 1 �����ϸ� fps ���� 60

            if (glewInit() != GLEW_OK) { // GLEW �ʱ�ȣ �����ϸ� ����
                glfwTerminate();
                return false;
            }

            std::cout << glGetString(GL_VERSION) << std::endl; // OpenGL ����

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