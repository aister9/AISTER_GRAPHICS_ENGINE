#pragma once
#include "glHeaders.h"
#include "Scene.h"
#include <vector>

std::vector<float> linspace(float start, float end, size_t points);


class Keypoint_utils {

public:
    AISTER_GRAPHICS_ENGINE::Scene *scene; 
    AISTER_GRAPHICS_ENGINE::PLYdata* plys;
    glm::vec2 resolution;

    Keypoint_utils(glm::vec2 resol) {
        scene = new AISTER_GRAPHICS_ENGINE::Scene(resol, "test");
        resolution = resol;
    }

    void Init(std::string plyPath, std::string texPath, int width = 640, int height = 480) {
        AISTER_GRAPHICS_ENGINE::PLYdata* data = new AISTER_GRAPHICS_ENGINE::PLYdata(plyPath);
        data->position = -1.f * data->getCeneter();
        plys = data;
        AISTER_GRAPHICS_ENGINE::Texture* tex = new AISTER_GRAPHICS_ENGINE::Texture(texPath);

        AISTER_GRAPHICS_ENGINE::Shader* sh = new AISTER_GRAPHICS_ENGINE::Shader();
        sh->initShaders("mesh_vertex.glsl", "mesh_frag.glsl");

        AISTER_GRAPHICS_ENGINE::plyRenderer *renderer = new AISTER_GRAPHICS_ENGINE::plyRenderer();
        renderer->SetShaderPLY(data, sh, tex);

        data->print();

        scene->pushRenderer(renderer);
    }

    void save_render_image(std::string folderPath) {
        std::vector<glm::vec3> camposes = CameraPositions(3, 3, plys->get_r_bbox() / 0.035 * 0.18);

        std::cout << "RenderQueue: " << scene->renderQueue.size() << std::endl;
        std::cout << glm::to_string(plys->getTRS()) << std::endl;

        for (int i = 0; i < camposes.size(); i++) {
            AISTER_GRAPHICS_ENGINE::Camera cam;
            cam.position = camposes[i];
            cam._far = 1000.f;

            cam.screenResolution = resolution;

            scene->setCamera(cam);
            scene->Render(false);
            scene->saveToPNG(folderPath + "/" + std::to_string(i) + "_RGB.png");
            scene->Render(true);
            scene->saveToPNG(folderPath + "/" + std::to_string(i) + "_depth.png");
        }
    }
    
    std::vector<glm::vec3> CameraPositions(int n1, int n2, float r) {

        std::vector<glm::vec3> positionList;

        float pis = glm::pi<float>();

        std::vector<float> theta_list = linspace(0.1, pis, n1 + 1);
        std::vector<float> phi_list = linspace(0, 2*pis, n2 + 1);
        theta_list.pop_back();
        phi_list.pop_back();

        std::vector<glm::vec2> pair_lst;

        for (auto xx : theta_list) {
            for (auto yy : phi_list) {
                pair_lst.push_back(glm::vec2(xx, yy));
            }
        }

        for (auto vars : pair_lst) {
            float x = r * glm::sin(vars.x) * glm::cos(vars.y);
            float y = r * glm::sin(vars.x) * glm::sin(vars.y);
            float z = r * glm::cos(vars.x);
            positionList.push_back(glm::vec3(x, y, z));
        }

        return positionList;
    }

    glm::mat4 getCameraPose(glm::vec3 cameraPosition) {
        glm::mat4 eyes(1.0f);

        glm::vec3 z_direct = glm::normalize(cameraPosition);
        glm::vec3 g_direct(0, 0, 1);
        glm::vec3 x_direct = -1.f * glm::cross(z_direct, g_direct);
        x_direct = glm::normalize(x_direct);
        glm::vec3 y_direct = glm::cross(z_direct, x_direct);
        y_direct = glm::normalize(y_direct);

        glm::mat3 pose(x_direct, y_direct, z_direct);
        pose = glm::inverse(pose);

        eyes = glm::translate(glm::mat4(1.0f), cameraPosition) * glm::mat4(pose);

        return eyes;
    }
};

std::vector<float> linspace(float start, float end, size_t points)
{
    std::vector<float> res(points);
    float step = (end - start) / (points - 1);
    size_t i = 0;
    for (auto& e : res)
    {
        e = start + step * i++;
    }
    return res;
}
