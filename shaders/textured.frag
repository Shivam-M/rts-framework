#version 330 core

in vec2 TextureCoords;
in vec2 ScreenPos;
out vec4 FragColour;

uniform sampler2D texturePrimary;
uniform sampler2D textureSecondary;
uniform vec4 colour;
uniform vec4 colourSecondary;
uniform bool useSecondTexture;

// Blend:
uniform int type;
uniform float time;
uniform float speed;
uniform float size;
uniform vec2 direction;
// todo: convert into array ^^
// todo: use speed/size/direction


float get_blend() {
    switch (type) {
        case 1:  // Wave
            float wave = sin(dot(TextureCoords, direction) * 6.2831 * size - time * speed) * 0.5 + 0.5;
            return clamp(TextureCoords.x + wave, 0.0, 1.0);
        case 2:  // Pulse
            float stripePosition = mod(dot(ScreenPos * (1.0f / 1280), normalize(direction)) * (100.0 / size) + time * speed, 2);
            return step(1.0, stripePosition);
        case 3:  // Stripe
            float stripe = mod(TextureCoords.y * 10.0 + time, 2.0);
            return step(1.0, stripe);
        case 4:  // Ripple
            float centre = distance(TextureCoords, vec2(0.5, 0.5));
            return sin(centre * 20.0 - time * 4.0) * 0.5 + 0.5;
        default:
            return 1.0f;
    }
}

void main() {
    vec4 textured_base = texture(texturePrimary, TextureCoords);
    vec4 textured_colour = textured_base * colour;

    if (type == 0) {
        FragColour = textured_colour;
    } else {
        FragColour = textured_base * mix(colour, colourSecondary, get_blend());
    }
}
