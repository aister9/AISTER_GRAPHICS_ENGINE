#version 460 core

uniform vec4 mtlColor;

uniform sampler2D textures;

out vec4 color;
in vec2 TexCoord;

void main() {
	color = texture(textures, TexCoord);
}
