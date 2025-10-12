#version 330 core

out vec4 FragColour;

in vec2 TextureCoords;
in vec4 Colour;
in vec4 ColourSecondary;

void main() {
    if (Colour == ColourSecondary) {
        FragColour = Colour;
    } else {
        FragColour = mix(Colour, ColourSecondary, TextureCoords.x);
    }
} 