#pragma once
#include "glHeaders.h"
#include <vector>

std::vector<float> linspace(float start, float end, size_t points);


class Keypoint_utils {

public:


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
