#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec4 colour_secondary;

out vec2 TextureCoords;
out vec4 Colour;
out vec4 ColourSecondary;

uniform mat4 projection;

const vec2 TEXTURE_COORDS[4] = vec2[4](
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0)
);

void main() {
    gl_Position = projection * vec4(position, 0.0, 1.0);
    TextureCoords = TEXTURE_COORDS[gl_VertexID & 3];
    Colour = colour;
    ColourSecondary = colour_secondary;
}
