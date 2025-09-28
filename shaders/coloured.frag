#version 330 core

out vec4 FragColour;

in vec2 TextureCoords;
in vec4 Colour;
in vec4 ColourSecondary;

uniform float radius;

void main() {
    if (radius > 0.0) {
        vec2 p = abs(TextureCoords - 0.5) - (0.5 - radius);
        if (length(max(p, 0.0)) > radius) discard;
    }
    
    if (Colour == ColourSecondary) {
        FragColour = Colour;
    } else {
        FragColour = mix(Colour, ColourSecondary, TextureCoords.x);
    }
} 