#pragma once

#include "../assets/moveable.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

struct Character {
    unsigned int texture_id;
    unsigned int advance;
    glm::ivec2 size;
    glm::ivec2 bearing;
    glm::vec4 uv;
};

struct Font {
    float h;
    GLuint texture;
    Character characters[128];
};

class TextRenderer {
    public:
        static void setup();
        static void init_shader();
        static void reset_shader();
        static void render_text(Font* ft_font, float x, const float& y, string const& text, Colour& colour, const float& scale = 1.0f, const float& priority = 0.0f);
        static Font* load_font(string font_name, int height, float scale = 1.0f);    
        static Vector2 calculate_text_dimensions(Font* ft_font, const string& text, const float& scale);
};
