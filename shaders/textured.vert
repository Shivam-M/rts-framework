#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

out vec2 TextureCoords;
out vec2 ScreenPos;
uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(position, 0.0, 1.0);
    TextureCoords = texCoord;
    ScreenPos = ((gl_Position.xy / gl_Position.w) * 0.5 + 0.5) * vec2(1280, 720);  // todo: make this dynamic
}
