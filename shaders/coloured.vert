#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

out vec2 TextureCoords;

uniform mat4 projection;
uniform float z_value;

void main() {
    gl_Position = projection * vec4(position, z_value, 1.0);
    TextureCoords = texCoord;
}
