#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>

#include "text_renderer.h"

#include "error.h"
#include "file.h"
#include "shader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

GLuint VAO, VBO;
GLuint shader;

void TextRenderer::setup() {
    shader = CompileShader("shaders/text.vert", "shaders/text.frag");
    glUseProgram(shader);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1280), 0.0f, static_cast<float>(720));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUseProgram(0);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Font* TextRenderer::load_font(string font_path, int height, float scale) {
    Font* ft_font = new Font();
    FT_Library library;
    FT_Face face;

    if (FT_Init_FreeType(&library)) {
        log_t(CON_RED "ERROR! FreeType initialisation failed");
        return nullptr;
    }

    if (FT_New_Face(library, font_path.c_str(), 0, &face)) {
        log_t(CON_RED "ERROR! Failed to load font file");
        return nullptr;
    }

    // FT_Set_Pixel_Sizes(face, 0, height);
    FT_Set_Char_Size(face, height << 6, height << 6, 96, 96);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            log_t(CON_RED "ERROR! Failed to load font glyph");
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);

        ft_font->characters.emplace(c, Character{ texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                                               static_cast<unsigned int>(face->glyph->advance.x) });
    
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return ft_font;
}

void TextRenderer::render_text(Font* ft_font, float x, float y, string const& text, Colour colour, float scale) {
    glUniform4f(glGetUniformLocation(shader, "characterColour"), colour.getX(), colour.getY(), colour.getZ(), colour.getW());

    for (string::const_iterator c = text.begin(); c != text.end(); c++) {
        Character ch = ft_font->characters.at(*c);

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.texture_id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.advance >> 6) * scale;
    }
}

Vector2 TextRenderer::calculate_text_dimensions(Font* ft_font, const string& text, float scale) {
    float width = 0.0f;
    float height = 0.0f;

    for (const char& c : text) {
        Character ch = ft_font->characters.at(c);
        width += (ch.advance >> 6) * scale;
        height = max(height, ch.size.y * scale);
    }

    return Vector2(width, height);
}

void TextRenderer::init_shader() {
    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void TextRenderer::reset_shader() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
