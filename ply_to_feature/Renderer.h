#pragma once
#include "plyData.h"
#include "Shader.h"
#include "Camera.h"

namespace AISTER_GRAPHICS_ENGINE {
	class plyRenderer {
		unsigned int VBO, VAO, EBO;
		Shader* shader;
		PLYdata* data;

	public:
		void SetShaderPLY(PLYdata *_data, Shader* _shader) {
			data = _data;
			shader = _shader;

			initShader();
		}

		void initShader() {
			glGenVertexArrays(1, &(this->VAO));
			glGenBuffers(1, &(this->VBO));
			glGenBuffers(1, &(this->EBO));
			glBindVertexArray(this->VAO);

			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data->vertices.size(), &data->vertices[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * data->faces.size(), &data->faces[0], GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
		}

		void Draw(Camera cam, glm::vec4 color) {
			shader->call();

			glm::mat4 trs = data->getTRS();
			glm::mat4 MVPmat = trs * cam.getProjectionMatrix() * cam.getViewMatrix(glm::vec3(0,0,0), glm::vec3(0,0,1));

			GLuint location_MVP = glGetUniformLocation(shader->shaderProgram, "MVP");
			glUniformMatrix4fv(location_MVP, 1, GL_FALSE, &MVPmat[0][0]);
			GLuint location_color = glGetUniformLocation(shader->shaderProgram, "mtlColor");
			glUniform4fv(location_color, 1, &(color)[0]);

			glBindVertexArray(this->VAO);
			glDrawElements(GL_TRIANGLES, data->faces.size(), GL_UNSIGNED_INT, 0);

		}

		void Draw(glm::mat4 MVP, glm::vec4 color) {
			shader->call();

			GLuint location_MVP = glGetUniformLocation(shader->shaderProgram, "MVP");
			glUniformMatrix4fv(location_MVP, 1, GL_FALSE, &MVP[0][0]);
			GLuint location_color = glGetUniformLocation(shader->shaderProgram, "mtlColor");
			glUniform4fv(location_color, 1, &(color)[0]);

			glBindVertexArray(this->VAO);
			glDrawElements(GL_TRIANGLES, data->faces.size(), GL_UNSIGNED_INT, 0);
		}
	};
}