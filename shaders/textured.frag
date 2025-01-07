#version 330 core

in vec2 TextureCoords;
out vec4 FragColour;

uniform sampler2D texturePrimary;
uniform sampler2D textureSecondary;
uniform vec4 colour;
uniform vec4 colourSecondary;
uniform bool useSecondTexture;

void main() {
    vec4 textured1 = texture(texturePrimary, TextureCoords);
    vec4 texturedColour1 = textured1 * colour;

    if (useSecondTexture) {
        vec4 textured2 = texture(textureSecondary, TextureCoords);
        vec4 texturedColour2 = textured2 * colourSecondary;

        if (texturedColour1.a > 0.0) {
            FragColour = mix(texturedColour1, texturedColour2, texturedColour2.a);
        }
    } else {
        // vec4 gradientColour = mix(colour, colourSecondary, TextureCoords.x);
        // FragColour = textured1 * gradientColour;
        FragColour = texturedColour1;
    }
}
