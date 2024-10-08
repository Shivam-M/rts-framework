#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 textureCoords;

uniform mat4 projection;
uniform float z_value;

void main() {
    gl_Position = projection * vec4(vertex.xy, z_value, 1.0);
    textureCoords = vertex.zw;
}
