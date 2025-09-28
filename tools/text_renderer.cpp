#include "text_renderer.h"

#include "../assets/moveable.h"
#include "../tools/common.h"

#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

const int ATLAS_WIDTH = 1024, ATLAS_HEIGHT = 1024;
GLuint VAO, VBO, shader_text;
GLuint texture_last_bound = -1;
float vertices[4096 * 6 * 8];
int vertex_index_batch = 0;

void TextRenderer::setup() { // todo: use EBOs to reduce buffer size and/or switch from GL_TRIANGLES
    shader_text = compile_shader("shaders/text.vert", "shaders/text.frag");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f);

    glUseProgram(shader_text);
    glUniformMatrix4fv(glGetUniformLocation(shader_text, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 8 * 4096, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

Font* TextRenderer::load_font(const string& font_path, int height, float scale) {
    Font* ft_font = new Font();
    FT_Library library;
    FT_Face face;
    GLuint texture_atlas;

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

    glGenTextures(1, &texture_atlas);
    glBindTexture(GL_TEXTURE_2D, texture_atlas);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ATLAS_WIDTH, ATLAS_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int x = 0, y = 0, max_height = 0;
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            log_t(CON_RED "ERROR! Failed to load font glyph");
            continue;
        }

        if (x + face->glyph->bitmap.width > ATLAS_WIDTH) {
            x = 0;
            y += max_height;
            max_height = 0;
        }

        max_height = max(max_height, (int)face->glyph->bitmap.rows);

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        float u0 = x / (float)ATLAS_WIDTH;
        float v0 = y / (float)ATLAS_HEIGHT;
        float u1 = (x + face->glyph->bitmap.width) / (float)ATLAS_WIDTH;
        float v1 = (y + face->glyph->bitmap.rows) / (float)ATLAS_HEIGHT;

        ft_font->characters[c] = Character{
            texture_atlas,
            static_cast<unsigned int>(face->glyph->advance.x),
            static_cast<float>(face->glyph->advance.x)/ 64.0f,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            glm::vec4(u0, v0, u1, v1)
        };

        x += face->glyph->bitmap.width * 1.5;
    }

    ft_font->texture = texture_atlas;
    ft_font->height = height;

    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return ft_font;
}

void TextRenderer::flush_batch() {
    if (vertex_index_batch > 0) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_index_batch * sizeof(float), vertices);
        glDrawArrays(GL_TRIANGLES, 0, vertex_index_batch / 8);
        vertex_index_batch = 0;
    }
}

void TextRenderer::render_text(Font* ft_font, float x, float y, string const& text, const Colour& colour, float scale) {
    if (texture_last_bound != ft_font->texture) {
        if (vertex_index_batch > 0) flush_batch();
        glBindTexture(GL_TEXTURE_2D, ft_font->texture);
        texture_last_bound = ft_font->texture;
    }

    static constexpr float NORMALISED = 1.0f / 255.0f;
    float r = colour.r * NORMALISED;
    float g = colour.g * NORMALISED;
    float b = colour.b * NORMALISED;
    float a = colour.a * NORMALISED;

    float baseline_y = floor(y + 0.5f);
    float current_x = x;

    int vertex_index = vertex_index_batch;

    for (const char& c : text) {
        const auto& ch = ft_font->characters[c];

        float xpos = current_x + ch.bearing.x * scale;
        float ypos = baseline_y - ch.bearing.y * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        float* v = &vertices[vertex_index];

        v[0] = xpos;      v[1] = ypos + h;  v[2] = ch.uv.x;  v[3] = ch.uv.w;  v[4] = r;  v[5] = g;  v[6] = b;  v[7] = a;
        v[8] = xpos;      v[9] = ypos;      v[10] = ch.uv.x; v[11] = ch.uv.y; v[12] = r; v[13] = g; v[14] = b; v[15] = a;
        v[16] = xpos + w; v[17] = ypos;     v[18] = ch.uv.z; v[19] = ch.uv.y; v[20] = r; v[21] = g; v[22] = b; v[23] = a;

        v[24] = xpos;     v[25] = ypos + h; v[26] = ch.uv.x; v[27] = ch.uv.w; v[28] = r; v[29] = g; v[30] = b; v[31] = a;
        v[32] = xpos + w; v[33] = ypos;     v[34] = ch.uv.z; v[35] = ch.uv.y; v[36] = r; v[37] = g; v[38] = b; v[39] = a;
        v[40] = xpos + w; v[41] = ypos + h; v[42] = ch.uv.z; v[43] = ch.uv.w; v[44] = r; v[45] = g; v[46] = b; v[47] = a;

        vertex_index += 48;
        current_x += ch.advance_pixels * scale;
    }

    vertex_index_batch = vertex_index;
}

void TextRenderer::calculate_text_dimensions(Font* ft_font, const string& text, float scale, Vector2& dimensions) {
    dimensions.x = 0;
    for (const char& c : text) {
        const auto& ch = ft_font->characters[c];
        dimensions.x += (ch.advance >> 6) * scale;
        dimensions.y = max(dimensions.y, ch.size.y * scale);
    }
}

void TextRenderer::init_shader() {
    glUseProgram(shader_text);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    vertex_index_batch = 0;
    texture_last_bound = -1;
}

void TextRenderer::reset_shader() {
    flush_batch();
    glBindVertexArray(0);
    glUseProgram(0);
}
