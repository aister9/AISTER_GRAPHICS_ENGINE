#include <iostream>
#include "glHeaders.h"
#include "plyData.h"
#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"
#include "Scene.h"
#include "keypoint_utils.h"

using namespace std;

glm::vec3 camPos(30, 0, 0);

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
    int width = 640, height = 480;

    if (!glfwewInit(&window, width, height)) return -1;

    AISTER_GRAPHICS_ENGINE::PLYdata plys("example/Cherries.ply");
    plys.print();
    plys.position = -1.f * plys.getCenter();
    cout << "r : " << plys.get_r_bbox() << endl;
    AISTER_GRAPHICS_ENGINE::Texture tex("example/Cherries.png");

    AISTER_GRAPHICS_ENGINE::Shader sh;
    sh.initShaders("mesh_vertex.glsl", "mesh_frag.glsl");

    AISTER_GRAPHICS_ENGINE::plyRenderer renderer;
    renderer.SetShaderPLY(&plys, &sh, &tex);

    AISTER_GRAPHICS_ENGINE::Camera cam;
    cam.position = camPos;
    cam._far = 1000.f;
    
    cam.screenResolution = glm::vec2(width, height);

    cout << "camera matrix : " << glm::to_string(cam.getProjectionMatrix()) << endl;
    
    AISTER_GRAPHICS_ENGINE::Scene test(glm::vec2(width, height));
    test.pushRenderer(&renderer);
    test.setCamera(cam);
    test.Render(false);
    test.saveToPNG("test1.png");
    test.Render(true);
    test.saveToPNG("test2.png");
    
    Keypoint_utils utils(glm::vec2(640, 480));
    utils.Init("example/Cherries.ply", "example/Cherries.png");
    utils.save_render_image("render");
    utils.FindKeypoint();
    
    vector<glm::vec3> camposes = utils.CameraPositions(3, 3, plys.get_r_bbox() / 0.035 * 0.18);
        
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // m2c values
    glm::mat4 m2c(glm::vec4(0.04992853, 0.001840242, -0.001938306, 0), glm::vec4(-4.371726E-05, -0.03569358, -0.03501383, 0), glm::vec4(0.002672364, -0.03496545, 0.03564093, 0), glm::vec4(1.21034, 0.8187663, -2.933632,1));
    
    // fx, fy, cx, cy, w, h, znear, zfar
    // Unity -> GL change to (fx, fy) -> (-fx, -fy)
    glm::mat4 persp = AISTER_GRAPHICS_ENGINE::getPerspectiveUsingK(572.4124, 573.5692, 320, 240, 640, 480, 0.01f, 4.5f);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD);
    glDepthFunc(GL_LESS);

    unsigned char* frameImage = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 4);
    unsigned char* frameImage2 = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 4);

    bool drawdepth = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthFunc(GL_LESS);
        renderer.Draw(cam, glm::vec4(1, 0, 0, 1), true);

        glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, frameImage2);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDepthFunc(GL_LESS);
        //renderer.Draw(cam, glm::vec4(1, 0, 0, 1), false);
        renderer.Draw(persp * m2c, glm::vec4(1, 0, 0, 1));

        glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, frameImage);


        glfwSwapBuffers(window);
    }

    cv::Mat imgs(height, width, CV_8UC4);
    imgs.data = frameImage;
    cv::flip(imgs, imgs, 0);
    cv::imwrite("test.png", imgs);

    cv::Mat imgs2(height, width, CV_8UC4);
    imgs2.data = frameImage2;
    cv::flip(imgs2, imgs2, 0);
    cv::imwrite("test3.png", imgs2);

    glfwDestroyWindow(window);
    glfwTerminate();

}