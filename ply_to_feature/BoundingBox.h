#pragma once
#include "glHeaders.h"
#include "Object.h"


namespace AISTER_GRAPHICS_ENGINE {
	class bbox : public Object_t {
	public:
		glm::vec3 ceneter;
		glm::vec3 size;
	};
}