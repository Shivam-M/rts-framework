#version 330 core
out vec4 colour;

uniform vec4 objectColour;

void main() {
    colour = vec4(objectColour.x / 255, objectColour.y / 255, objectColour.z / 255, objectColour.w / 255);
}
