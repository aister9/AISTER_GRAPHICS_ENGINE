#pragma once
#include "glHeaders.h"
#include "Object.h"
#include <vector>
#include <string>
#include <fstream>

namespace AISTER_GRAPHICS_ENGINE {
	class PLYdata : public Object_t {
	public:
		std::vector<glm::vec3> vertices;
		std::vector<int> faces;
		std::vector<glm::vec2> uvs;

		PLYdata() {}

		//get ply data
		PLYdata(std::string ply_path) {
			std::ifstream m_file(ply_path);
			float vertexsize = 0;
			float facesize = 0;

			while (!m_file.eof()) {
				std::string tmp;
				m_file >> tmp;

				if (tmp._Equal("element")) {
					m_file >> tmp;
					if (tmp._Equal("vertex")) {
						m_file >> tmp;
						vertexsize = atof(tmp.c_str());
					}
					else if (tmp._Equal("face")) {
						m_file >> tmp;
						facesize = atof(tmp.c_str());
					}
				}

				if (tmp._Equal("end_header")) {
					for (int i = 0; i < vertexsize; i++) {
						float x, y, z, nx, ny, nz, uu, vv;
						m_file >> tmp;
						x = atof(tmp.c_str());
						m_file >> tmp;
						y = atof(tmp.c_str());
						m_file >> tmp;
						z = atof(tmp.c_str());
						m_file >> tmp;
						nx = atof(tmp.c_str());
						m_file >> tmp;
						ny = atof(tmp.c_str());
						m_file >> tmp;
						nz = atof(tmp.c_str());
						m_file >> tmp;
						uu = atof(tmp.c_str());
						m_file >> tmp;
						vv = atof(tmp.c_str());

						glm::vec3 pos(x, y, z);
						glm::vec2 _uv(uu, vv);

						vertices.push_back(pos);
						uvs.push_back(_uv);
					}

					for (int i = 0; i < facesize; i++) {
						int count = 0;
						m_file >> tmp;
						count = atoi(tmp.c_str());

						for (int j = 0; j < count; j++) {
							int idx = 0;
							m_file >> tmp;
							idx = atoi(tmp.c_str());
							faces.push_back(idx);
						}
					}
				}
			}
		}

		void print() {
			std::cout << "Vertex size : " << vertices.size() << " face : " << faces.size() / 3 << std::endl;
			std::cout << "Postion : " << position.x << ", " << position.y << ", " << position.z << std::endl;
			std::cout << "Rotation: " << rotation.w << ", " << rotation.x << ", " << rotation.y << ", " << rotation.z << std::endl;
			std::cout << "Scale: " << scale.x << ", " << scale.y << ", " << scale.z << std::endl;
		}
	};
}