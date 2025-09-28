#version 330 core
in vec2 TextureCoords;
in vec4 FragColour;

out vec4 colour;

uniform sampler2D characterTexture;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(characterTexture, TextureCoords).r);
    colour = FragColour * sampled;
}
