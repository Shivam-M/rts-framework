#pragma once

#include "../assets/moveable.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

struct Character {
    unsigned int texture_id;
    glm::ivec2   size;
    glm::ivec2   bearing;
    unsigned int advance;
};

struct Font {
    float h;
    vector<GLuint> textures;
    map<char, Character> characters;
};

class TextRenderer {
    public:
        static void setup();
        static void init_shader();
        static void reset_shader();
        static void render_text(Font* ft_font, float x, float y, string const& text, Colour colour, float scale = 1.0f);
        static Font* load_font(string font_name, int height);    
};
