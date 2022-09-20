#pragma once
#include <iostream>
#include "glHeaders.h"
#include "plyData.h"
#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"
#include <opencv2/opencv.hpp>


namespace AISTER_GRAPHICS_ENGINE {
    class Scene {
    public:
        unsigned int fbo;
        GLuint RenderTexture;
        
        Camera cam;
        std::vector<plyRenderer*> renderQueue;

        GLFWwindow* windows;

        glm::vec2 renderResolution;
        uchar* renderImage;

        Scene(glm::vec2 _RenderResolution) {
            renderResolution = _RenderResolution;

            renderImage = new uchar[renderResolution.x * renderResolution.y * 4];

            (windows) = glfwCreateWindow(renderResolution.x, renderResolution.y, "Hidden window", nullptr, nullptr);
            glfwHideWindow(windows);

            genFrameBuffers();
        }

        void setCamera(Camera _cam) {
            cam = _cam;
        }

        void pushRenderer(plyRenderer* renderer) {
            plyRenderer* pp = renderer;
            renderQueue.push_back(pp);
        }

        void genFrameBuffers() {
            glGenFramebuffers(1, &fbo);
            glGenTextures(1, &RenderTexture);
            glBindTexture(GL_TEXTURE_2D, RenderTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D,0);
        }

        void Render(bool depth) {
            int a = 0; 
            glEnable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD);
            glDepthFunc(GL_LESS);

            while (!glfwWindowShouldClose(windows) && a++ <=2) {
                glBindFramebuffer(GL_FRAMEBUFFER, fbo);

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                /////////////////////////////////////////////////////////////////////////////////////////////////////
                /*Draw Objects*/

                for (auto renderer : renderQueue) {
                    renderer->Draw(cam, depth);
                }

                glReadPixels(0, 0, renderResolution.x, renderResolution.y, GL_BGRA, GL_UNSIGNED_BYTE, renderImage);

                /////////////////////////////////////////////////////////////////////////////////////////////////////


                glBindTexture(GL_TEXTURE_2D, RenderTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderResolution.x, renderResolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderTexture, 0);
                glBindTexture(GL_TEXTURE_2D, 0);

                unsigned int rbo;
                glGenRenderbuffers(1, &rbo);
                glBindRenderbuffer(GL_RENDERBUFFER, rbo);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, renderResolution.x, renderResolution.y);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                glDeleteRenderbuffers(1, &rbo);

                glfwSwapBuffers(windows);
            }
        }

        void saveToPNG(std::string filepath) {
            cv::Mat imgs(renderResolution.y, renderResolution.x, CV_8UC4);
            imgs.data = renderImage;
            cv::flip(imgs, imgs, 0);
            cv::imwrite(filepath, imgs);
        }
    };
}