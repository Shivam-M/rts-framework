#version 330 core

layout (location = 0) in vec2 position;

out vec2 TextureCoords;
out vec2 ScreenPosition;

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
    ScreenPosition = ((gl_Position.xy / gl_Position.w) * 0.5 + 0.5) * vec2(1280, 720); // TODO: make dynamic
}
