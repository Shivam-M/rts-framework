#include "text_renderer.h"

#include "../assets/moveable.h"
#include "../tools/common.h"

#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

GLuint VAO, VBO, TextShader;
int AtlasWidth = 4096, AtlasHeight = 4096;
GLuint uniformColour, uniformPriority;

void TextRenderer::setup() {
    TextShader = CompileShader("shaders/text.vert", "shaders/text.frag");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f);

    glUseProgram(TextShader);
    glUniformMatrix4fv(glGetUniformLocation(TextShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    uniformColour = glGetUniformLocation(TextShader, "characterColour");
    uniformPriority = glGetUniformLocation(TextShader, "z_value");
    glUseProgram(0);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4 * 128, NULL, GL_DYNAMIC_DRAW);
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

    FT_Set_Char_Size(face, height << 6, height << 6, 96, 96);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLuint atlasTexture;
    glGenTextures(1, &atlasTexture);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, AtlasWidth, AtlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int x = 0, y = 0, maxHeight = 0;
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            log_t(CON_RED "ERROR! Failed to load font glyph");
            continue;
        }

        if (x + face->glyph->bitmap.width > AtlasWidth) {
            x = 0;
            y += maxHeight;
            maxHeight = 0;
        }

        maxHeight = max(maxHeight, (int)face->glyph->bitmap.rows);

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        float u0 = x / (float)AtlasWidth;
        float v0 = y / (float)AtlasHeight;
        float u1 = (x + face->glyph->bitmap.width) / (float)AtlasWidth;
        float v1 = (y + face->glyph->bitmap.rows) / (float)AtlasHeight;

        ft_font->characters[c] = Character {
            atlasTexture,
            static_cast<unsigned int>(face->glyph->advance.x),
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            glm::vec4(u0, v0, u1, v1)
        };

        x += face->glyph->bitmap.width * 1.5;
    }

    ft_font->texture = atlasTexture;
    ft_font->h = height;

    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return ft_font;
}

GLuint lastBoundTexture = -1;
Colour lastBoundColour = Colour(0, 0, 0, 0);
float vertices[1024 * 6 * 4 * 2];
float xpos, ypos, w, h;
size_t vertexIndex = 0;
void TextRenderer::render_text(Font* ft_font, const float& x, const float& y, string const& text, const Colour& colour, const float& scale, const float& priority) {
    glUniform1f(uniformPriority, priority);

    if (lastBoundTexture != ft_font->texture) {
        glBindTexture(GL_TEXTURE_2D, ft_font->texture);
        lastBoundTexture = ft_font->texture;
    }

    if (lastBoundColour != colour) {
        glUniform4f(uniformColour, colour.x_ / 255.0f, colour.y_ / 255.0f, colour.z_ / 255.0f, colour.w_ / 255.0f);
        lastBoundColour = colour;
    }

    float baseline_y = floor(y + 0.5f);
    float current_x = x;

    for (const char& c : text) {
        Character& ch = ft_font->characters[c];

        xpos = current_x + ch.bearing.x * scale;
        ypos = baseline_y - ch.bearing.y * scale;

        w = ch.size.x * scale;
        h = ch.size.y * scale;

        vertices[vertexIndex++] = xpos;
        vertices[vertexIndex++] = ypos + h;
        vertices[vertexIndex++] = ch.uv.x;
        vertices[vertexIndex++] = ch.uv.w;

        vertices[vertexIndex++] = xpos;
        vertices[vertexIndex++] = ypos;
        vertices[vertexIndex++] = ch.uv.x;
        vertices[vertexIndex++] = ch.uv.y;

        vertices[vertexIndex++] = xpos + w;
        vertices[vertexIndex++] = ypos;
        vertices[vertexIndex++] = ch.uv.z;
        vertices[vertexIndex++] = ch.uv.y;

        vertices[vertexIndex++] = xpos;
        vertices[vertexIndex++] = ypos + h;
        vertices[vertexIndex++] = ch.uv.x;
        vertices[vertexIndex++] = ch.uv.w;

        vertices[vertexIndex++] = xpos + w;
        vertices[vertexIndex++] = ypos;
        vertices[vertexIndex++] = ch.uv.z;
        vertices[vertexIndex++] = ch.uv.y;

        vertices[vertexIndex++] = xpos + w;
        vertices[vertexIndex++] = ypos + h;
        vertices[vertexIndex++] = ch.uv.z;
        vertices[vertexIndex++] = ch.uv.w;

        current_x += (ch.advance >> 6) * scale;
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexIndex * sizeof(float), vertices);
    glDrawArrays(GL_TRIANGLES, 0, vertexIndex / 4);
    vertexIndex = 0;
}

Vector2 TextRenderer::calculate_text_dimensions(Font* ft_font, const string& text, const float& scale) {
    float width = 0.0f, height = 0.0f;

    Vector2 dimensions;

    for (const char& c : text) {
        const Character& ch = ft_font->characters[c];
        dimensions.x += (ch.advance >> 6) * scale;
        dimensions.y = max(dimensions.y, ch.size.y * scale);
    }

    return dimensions;
}

void TextRenderer::init_shader() {
    glUseProgram(TextShader);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void TextRenderer::reset_shader() {
    glBindVertexArray(0);
    /*
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    */
}
