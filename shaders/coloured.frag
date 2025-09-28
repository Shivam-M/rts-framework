#version 330 core

out vec4 FragColour;

uniform vec4 colour;
uniform vec4 colourSecondary;
uniform float radius;

in vec2 TextureCoords;

void main() {
	if (radius > 0.0) {
        vec2 p = abs(TextureCoords - 0.5) - (0.5 - radius);
        if (length(max(p, 0.0)) > radius) discard;
    }
    
    if (colour == colourSecondary) {
        FragColour = colour;
    } else {
        FragColour = mix(colour, colourSecondary, TextureCoords.x);
    }
}