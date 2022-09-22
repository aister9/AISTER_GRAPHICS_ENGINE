#pragma once
#include "glHeaders.h"
#include "Scene.h"
#include <opencv2/opencv.hpp>
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
        data->scale = glm::vec3(0.05, 0.05, 0.05);
        data->position = -1.f * data->getCenter();
        plys = data;
        AISTER_GRAPHICS_ENGINE::Texture* tex = new AISTER_GRAPHICS_ENGINE::Texture(texPath);

        AISTER_GRAPHICS_ENGINE::Shader* sh = new AISTER_GRAPHICS_ENGINE::Shader();
        sh->initShaders("mesh_vertex.glsl", "mesh_frag.glsl");

        AISTER_GRAPHICS_ENGINE::plyRenderer *renderer = new AISTER_GRAPHICS_ENGINE::plyRenderer();
        renderer->SetShaderPLY(data, sh, tex);

        data->print();

        scene->pushRenderer(renderer);
    }

    void FindKeypoint() {
        std::vector<glm::vec3> camposes = CameraPositions(3, 3, plys->get_r_bbox() / 0.035 * 0.18);
        std::vector<std::pair<uchar*, float*>> images = render_rgb_depth(camposes);
        std::vector<glm::vec3> three_dpts;
        for (int i = 0; i < camposes.size(); i++) {
            std::vector<glm::vec3> temp = getKeypoints(images[i].first, images[i].second, camposes[i]);
            three_dpts.insert(three_dpts.end(), temp.begin(), temp.end());
        }
        std::vector<glm::vec3> fartest_point_8 = fartest_point_sampling(three_dpts, 8);
        std::cout << "fartest_point_8" << std::endl;
        for (auto vv : fartest_point_8) {
            std::cout << glm::to_string(vv) << std::endl;
        }
        std::cout << "corner" << std::endl;
        for (auto vv : plys->get_3d_bbox().getCorner()) {
            std::cout << glm::to_string(vv) << std::endl;
        }
    }

    std::vector<glm::vec3> fartest_point_sampling(std::vector<glm::vec3> pts, int sn) {
        std::vector<glm::vec3> res;

        std::vector<bool> mask(pts.size(), false);
        std::vector<float> min_dist(pts.size(), 987654321);

        std::vector<int> retained_idx(sn);

        srand(time(0));
        int cur_idx = rand() % pts.size();
        for (int i = 0; i < sn; i++) {
            mask[cur_idx] = true;
            res.push_back(pts[cur_idx]);
            if (i < sn - 1) {
                for (int j = 0; j < pts.size(); j++) {
                    if (mask[j]) continue;
                    float dist = glm::distance(pts[j], pts[cur_idx]);
                    if (dist < min_dist[j]) min_dist[j] = dist;
                }

                int max_idx = 0;
                float max_d = 0.f;

                for (int j = 0; j < pts.size(); j++) {
                    if (mask[j]) continue;
                    if (min_dist[j] > max_d) {
                        max_idx = j;
                        max_d = min_dist[j];
                    }
                }

                cur_idx = max_idx;
            }
        }

        return res;
    }

    std::vector<std::pair<uchar*, float*>> render_rgb_depth(std::vector<glm::vec3> camposes) {
        std::cout << "RenderQueue: " << scene->renderQueue.size() << std::endl;
        std::cout << glm::to_string(plys->getTRS()) << std::endl;

        std::vector<std::pair<uchar*, float*>> res;

        for (int i = 0; i < camposes.size(); i++) {
            AISTER_GRAPHICS_ENGINE::Camera cam;
            cam.setFOV_K(572.4124, 573.5692, 640, 480, 0.005f, 4.5f);
            cam.position = camposes[i];
            cam.direction = glm::mat3(getCameraPose(camposes[i])) * cam.direction;

            cam.screenResolution = resolution;

            scene->setCamera(cam);
            scene->Render(false);
            uchar* rgbs = new uchar[resolution.x * resolution.y * 4];
            memcpy(rgbs, scene->renderImage, sizeof(uchar) * resolution.x * resolution.y * 4);
            scene->Render(true);
            float* depths = new float[resolution.x * resolution.y];
            memcpy(depths, scene->depthTexture, sizeof(float) * resolution.x * resolution.y);

            res.push_back(std::make_pair(rgbs, depths));
        }

        return  res;
    }

    void save_render_image(std::string folderPath) {
        std::vector<glm::vec3> camposes = CameraPositions(3, 3, plys->get_r_bbox() / 0.035 * 0.18);

        std::cout << "RenderQueue: " << scene->renderQueue.size() << std::endl;
        std::cout << glm::to_string(plys->getTRS()) << std::endl;

        for (int i = 0; i < camposes.size(); i++) {
            AISTER_GRAPHICS_ENGINE::Camera cam;
            cam.setFOV_K(572.4124, 573.5692, 640, 480, 0.005f, 4.5f);
            cam.position = camposes[i];
            cam.direction = glm::mat3(getCameraPose(camposes[i])) * cam.direction;

            cam.screenResolution = resolution;

            scene->setCamera(cam);
            scene->Render(false);
            scene->saveToPNG(folderPath + "/" + std::to_string(i) + "_RGB.png");
            scene->Render(true);
            scene->saveToEXR(folderPath + "/" + std::to_string(i) + "_depth.exr");
        }
    }

    std::vector<glm::vec3> getKeypoints(uchar* imageRaw, float* depthRaw, glm::vec3 composes) {
        cv::Ptr<cv::ORB> orb = cv::ORB::create();
        cv::Mat img(resolution.y, resolution.x, CV_8UC4, imageRaw);
        cv::Mat gray;
        cv::flip(img, gray, 0); // flip y axis (gl -> cv surface)
        cv::cvtColor(gray, gray, cv::COLOR_BGRA2GRAY);
        std::vector<cv::KeyPoint> kpts;
        cv::Mat des;
        orb->detectAndCompute(gray, cv::noArray(), kpts, des);

        cv::Mat outputImage;
        cv::flip(img, outputImage, 0);

        std::vector<glm::vec2> kpts_xys;
        for (auto kpt : kpts) {
            kpts_xys.push_back(glm::vec2(kpt.pt.x, kpt.pt.y));
            cv::circle(outputImage, cv::Point(kpt.pt.x, kpt.pt.y), 5, cv::Scalar(255, 0, 0));
        }

        glm::mat4 K(glm::vec4(572.4124, 0, 0, 0), glm::vec4(0, 573.5692, 0, 0), glm::vec4(320, 240, 1, 0), glm::vec4(0, 0, 0, 1));
        glm::mat4 K_inv = glm::inverse(K);

        std::vector<glm::vec3> pcd_dpt;
        cv::Mat depthMap(resolution.y, resolution.x, CV_32FC1, depthRaw);
        cv::Mat flipedDepth;
        cv::flip(depthMap, flipedDepth, 0);

        for(auto kpt : kpts_xys){
            glm::vec4 pts(kpt, flipedDepth.at<float>(kpt.y, kpt.x), 1.0f);
            glm::vec3 pcd_pts = flipedDepth.at<float>(kpt.y, kpt.x) * K_inv * pts;

            if(!glm::isinf(kpt.x) && pcd_pts.z >1e-8)
                pcd_dpt.push_back(pcd_pts);
        }

        AISTER_GRAPHICS_ENGINE::Camera cam;
        cam.setFOV_K(572.4124, 573.5692, 640, 480, 0.005f, 4.5f);
        cam.position = composes;

        cam.screenResolution = resolution;

        glm::mat4 w2c = get_c2w(getCameraPose(composes));
        glm::mat4 o2w = (glm::translate(glm::mat4(1.0f), plys->position) * glm::toMat4(plys->rotation));

        for (auto& kpt : pcd_dpt) {
            kpt = w2c*o2w * glm::vec4(kpt,1.0);
        }

        cv::imwrite("kptsTest.png", outputImage);

        return pcd_dpt;
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
        //pose = glm::transpose(pose);

        eyes = glm::translate(glm::mat4(1.0f), cameraPosition) * glm::mat4(pose);

        return eyes;
    }

    glm::mat4 get_c2w(glm::mat4 cam_pose) {
        glm::mat3 R_bcam2cv = glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
        
        glm::vec3 location = cam_pose[3];
        glm::mat3 rotation = glm::mat3(cam_pose[0], cam_pose[1], cam_pose[2]);

        glm::mat3 R_world2bcam = glm::transpose(rotation);

        glm::vec3 T_world2bcam = -1.f * R_world2bcam * location;

        glm::mat3 R_world2cv = R_bcam2cv * R_world2bcam;
        glm::vec3 T_world2cv = R_bcam2cv * T_world2bcam;

        return glm::mat4(
            glm::vec4(R_world2cv[0], 0),
            glm::vec4(R_world2cv[1], 0),
            glm::vec4(R_world2cv[2], 0),
            glm::vec4(T_world2cv,1));
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
