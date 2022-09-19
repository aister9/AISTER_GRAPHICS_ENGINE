#pragma once
#include "Object.h"
#include "glHeaders.h"

namespace AISTER_GRAPHICS_ENGINE {
	class Camera : public Object_t {
	public:
		float fovy = 45.0f;

		glm::vec2 screenResolution;

		float _near = 0.1f;
		float _far = 4.5f;

		glm::mat4 getProjectionMatrix() {

			return glm::perspective(glm::radians(fovy), (float)screenResolution.x / (float)screenResolution.y, _near, _far);
		}

		glm::mat4 getViewMatrix(glm::vec3 direction, glm::vec3 up) {

			return glm::lookAt(position, direction, up);
		}
	};
}