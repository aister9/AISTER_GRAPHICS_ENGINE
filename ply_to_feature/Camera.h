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

	glm::mat4 getPerspectiveUsingK(float fx, float fy, float cx, float cy, int w, int h, float znear, float zfar) {
		float fovy = 2 * glm::atan(h / (2 * fy));

		return glm::perspective(fovy, (float)w / (float)h, znear, zfar);
	}
}