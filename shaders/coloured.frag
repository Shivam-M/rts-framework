#version 330 core

out vec4 FragColour;

uniform vec4 colour;
uniform vec4 colourSecondary;
uniform float radius;

in vec2 TextureCoords;

void main() {
	if (radius > 0.0f) {
		vec2 p = TextureCoords;

		float left = (p.x < radius) ? radius - p.x : 0.0;
		float right = (p.x > 1.0 - radius) ? p.x - (1.0 - radius) : 0.0;
		float top = (p.y > 1.0 - radius) ? p.y - (1.0 - radius) : 0.0;
		float bottom = (p.y < radius) ? radius - p.y : 0.0;

		float distanceToCorner = length(vec2(left, bottom)) +
								 length(vec2(left, top)) +
								 length(vec2(right, bottom)) +
								 length(vec2(right, top));

		if (distanceToCorner > radius) discard;
	}
	FragColour = mix(colour, colourSecondary, TextureCoords.x);
}