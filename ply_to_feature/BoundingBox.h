#pragma once
#include "glHeaders.h"
#include "Object.h"
#include <vector>


namespace AISTER_GRAPHICS_ENGINE {
	class bbox : public Object_t {
	public:
		glm::vec3 center;
		glm::vec3 size;

		std::vector<glm::vec3> getCorner() {
			std::vector<glm::vec3> res = {
				glm::vec3(center.x - size.x / 2, center.y - size.y / 2, center.z - size.z / 2),
				glm::vec3(center.x + size.x / 2, center.y - size.y / 2, center.z - size.z / 2),
				glm::vec3(center.x - size.x / 2, center.y + size.y / 2, center.z - size.z / 2),
				glm::vec3(center.x + size.x / 2, center.y + size.y / 2, center.z - size.z / 2),
				glm::vec3(center.x - size.x / 2, center.y - size.y / 2, center.z + size.z / 2),
				glm::vec3(center.x + size.x / 2, center.y - size.y / 2, center.z + size.z / 2),
				glm::vec3(center.x - size.x / 2, center.y + size.y / 2, center.z + size.z / 2),
				glm::vec3(center.x + size.x / 2, center.y + size.y / 2, center.z + size.z / 2) };
			return res;
		};
	};
}