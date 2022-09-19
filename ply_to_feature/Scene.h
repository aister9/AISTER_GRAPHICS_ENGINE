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

        glm::vec2 renderResolution;
        uchar* renderImage;

        Scene(glm::vec2 _RenderResolution) {
            renderResolution = _RenderResolution;

            renderImage = new uchar[renderResolution.x * renderResolution.y * 4];

            genFrameBuffers();
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

        void Render() {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /////////////////////////////////////////////////////////////////////////////////////////////////////
            /*Draw Objects*/


            /////////////////////////////////////////////////////////////////////////////////////////////////////


            glBindTexture(GL_TEXTURE_2D, RenderTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderResolution.x, renderResolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderTexture, 0);
            glReadPixels(0, 0, renderResolution.x, renderResolution.y, GL_BGRA, GL_UNSIGNED_BYTE, renderImage);
            glBindTexture(GL_TEXTURE_2D, 0);


            unsigned int rbo;
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, renderResolution.x, renderResolution.y);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            glDeleteRenderbuffers(1, &rbo);
        }

        void saveToPNG(std::string filepath) {
            cv::Mat imgs(renderResolution.y, renderResolution.x, CV_8UC4);
            imgs.data = renderImage;
            cv::flip(imgs, imgs, 0);
            cv::imwrite(filepath, imgs);
        }
    };
}