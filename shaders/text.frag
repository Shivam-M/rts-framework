#version 330 core
in vec2 textureCoords;
out vec4 colour;

uniform sampler2D characterTexture;
uniform vec4 characterColour;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(characterTexture, textureCoords).r);
    colour = vec4(characterColour.x / 255, characterColour.y / 255, characterColour.z / 255, characterColour.w / 255) * sampled;
}
