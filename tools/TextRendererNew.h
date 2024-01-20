#pragma once

#include "../assets/moveable.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

struct Character {
    unsigned int TextureID;
    glm::ivec2   Size;
    glm::ivec2   Bearing;
    unsigned int Advance;
};

struct font_data2 {
    float h;
    vector<GLuint> textures;
    map<char, Character> Characters;
};

class TextRendererNew {
    public:
        static void setup();
        static void init_shader();
        static void reset_shader();
        static void render_text(font_data2 const& ft_font, float x, float y, string const& text, Colour colour, float scale = 1.0f);
        static font_data2 load_font(string font_name, int height);
        
};
