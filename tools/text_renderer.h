#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

struct Colour;
struct Vector2;

struct Character {
    unsigned int texture_id;
    unsigned int advance;
    float advance_pixels;
    glm::ivec2 size;
    glm::ivec2 bearing;
    glm::vec4 uv;
};

struct Font {
    float height;
    unsigned int texture;
    Character characters[128];
};

class TextRenderer {
    public:
        static void flush_batch();
        static void setup();
        static void init_shader();
        static void reset_shader();
        static void render_text(Font* ft_font, const Vector2& position, const string& text, const Colour& colour, float scale = 1.0f);
        static void calculate_text_dimensions(Font* ft_font, const string& text, float scale, Vector2& dimensions);
        static Font* load_font(const string& font_name, int height, float scale = 1.0f);    
};
