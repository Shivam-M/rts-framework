#include "render.h"

#include "../assets/text.h"
#include "../tools/text_renderer.h"
#include "../tools/common.h"

using namespace std;

const int BUFFER_SIZE = 512 * 4 * 10;

GLuint shared_VAO, shared_VBO;
GLuint shader_textured, shader_coloured;
GLfloat vertex_buffer[BUFFER_SIZE];

TextureUniforms uniforms_textured;
QuadUniforms uniforms_coloured;

Render::Render(GLFWwindow* window, vector<Moveable*>* objects, vector<Text*>* text_objects) : window_(window), objects_(objects), text_objects_(text_objects) {
    TextRenderer::setup();

    shader_textured = compile_shader("shaders/textured.vert", "shaders/textured.frag");
    glUseProgram(shader_textured);

    uniforms_textured.time = glGetUniformLocation(shader_textured, "time");
    uniforms_textured.type = glGetUniformLocation(shader_textured, "type");
    uniforms_textured.speed = glGetUniformLocation(shader_textured, "speed");
    uniforms_textured.size = glGetUniformLocation(shader_textured, "size");
    uniforms_textured.direction = glGetUniformLocation(shader_textured, "direction");
    uniforms_textured.projection = glGetUniformLocation(shader_textured, "projection");

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT));
    glUniformMatrix4fv(uniforms_textured.projection, 1, GL_FALSE, glm::value_ptr(projection));

    glGenVertexArrays(1, &shared_VAO);
    glBindVertexArray(shared_VAO);

    glGenBuffers(1, &shared_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, shared_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * BUFFER_SIZE, NULL, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));

    // --

    shader_coloured = compile_shader("shaders/coloured.vert", "shaders/coloured.frag");
    glUseProgram(shader_coloured);

    uniforms_coloured.radius = glGetUniformLocation(shader_coloured, "radius");
    uniforms_coloured.projection = glGetUniformLocation(shader_coloured, "projection");

    glUniformMatrix4fv(uniforms_coloured.projection, 1, GL_FALSE, glm::value_ptr(projection));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Render::fill_vertex_buffer(int& count, const Vector2& location, const Vector2& size, bool fixed_position, Colour* colour, Colour* secondary) const { // ideally move logic to moveable class
    float sf = fixed_position ? 1.0f : scale;
    float ox = fixed_position ? 0.0f : offsets.x;
    float oy = fixed_position ? 0.0f : offsets.y;

    float x = (location.x * sf + ox) * WINDOW_WIDTH;
    float y = (1.0f - (location.y * sf + oy) - size.y * sf) * WINDOW_HEIGHT;
    float w = size.x * sf * WINDOW_WIDTH;
    float h = size.y * sf * WINDOW_HEIGHT;

    Vector4 normalised_colour = *colour / 255.0f;
    Vector4 normalised_colour_secondary = *secondary / 255.0f;
    Vector2 vertex_corner_positions[4] = {
        { x,     y + h },
        { x,     y     },
        { x + w, y     },
        { x + w, y + h }
    };

    for (const auto& position : vertex_corner_positions) {
        vertex_buffer[count++] = position.x;
        vertex_buffer[count++] = position.y;

        vertex_buffer[count++] = normalised_colour.x;
        vertex_buffer[count++] = normalised_colour.y;
        vertex_buffer[count++] = normalised_colour.z;
        vertex_buffer[count++] = normalised_colour.w;

        vertex_buffer[count++] = normalised_colour_secondary.x;
        vertex_buffer[count++] = normalised_colour_secondary.y;
        vertex_buffer[count++] = normalised_colour_secondary.z;
        vertex_buffer[count++] = normalised_colour_secondary.w;
    }
}

void Render::draw_batched_quads() {
    glUseProgram(shader_coloured);
    glBindVertexArray(shared_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, shared_VBO);

    int vertex_count = 0;
    for (int i = 0; i < quad_count_; i++) {
        const QuadData& quad_data = batched_quads_[i];
        fill_vertex_buffer(vertex_count, quad_data.location, quad_data.size, quad_data.fixed_position, quad_data.colour, quad_data.colour_secondary);
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * sizeof(GLfloat), vertex_buffer);

    for (int i = 0; i < quad_count_; i++) {
        const QuadData& quad_data = batched_quads_[i];

        glUniform1f(uniforms_coloured.radius, quad_data.radius); // todo: probably remove
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }

    quad_count_ = 0;
}

void Render::draw_batched_textures() {
    glUseProgram(shader_textured);
    glBindVertexArray(shared_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, shared_VBO);

    int vertex_count = 0;
    for (int i = 0; i < texture_count_; i++) {
        const TextureData& texture_data = batched_textures_[i];
        fill_vertex_buffer(vertex_count, texture_data.location, texture_data.size, texture_data.fixed_position, texture_data.colour, texture_data.colour_secondary);
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * sizeof(GLfloat), vertex_buffer);

    const float time = glfwGetTime();

    for (int i = 0; i < texture_count_; i++) {
        const TextureData& texture_data = batched_textures_[i];
        const Blend* blend = texture_data.blend;

        glUniform1i(uniforms_textured.type, blend->type);

        if (blend->type != 0) {
            glUniform1f(uniforms_textured.time, blend->fixed ? 1.0f : time);
            glUniform1f(uniforms_textured.speed, blend->speed);
            glUniform1f(uniforms_textured.size, blend->size);
            glUniform2f(uniforms_textured.direction, blend->direction.x, blend->direction.y);
        }

        glBindTexture(GL_TEXTURE_2D, texture_data.texture->data);
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }

    texture_count_ = 0;
}

void Render::draw_text(const Vector2& location, const string& message, Font* font, const Colour& colour, float scale, bool fixed) const {
    const Vector2& position = fixed ? location : location * this->scale + this->offsets;
    TextRenderer::render_text(font, position * resolution, message, colour, fixed ? scale : scale * this->scale);
}

void Render::draw_custom(vector<Vector2>* points, Colour* colour, Colour* gradient) {
    return; // TODO: Reimplement custom shapes for sidescroller
}

void Render::toggle_fullscreen() {
    fullscreen_ = !fullscreen_;
    glfwSetWindowMonitor(window_, fullscreen_ ? glfwGetPrimaryMonitor() : NULL, 100, 100, resolution.x, resolution.y, GLFW_DONT_CARE);
    render_window();
}

void Render::render_moveable(Moveable* moveable) {
    if (moveable->has_flag(CUSTOM)) {
        // draw_custom(moveable->get_points(), moveable->get_colour_ref(), moveable->get_gradient_colour_ref());
    } else if (moveable->has_flag(CURVED)) {
        draw_quad(moveable->get_location(), moveable->get_size(), &moveable->evaluated_colour, &moveable->gradient_colour, 0.025f, moveable->has_flag(FIXED_POS));
    } else if (moveable->has_flag(TEXTURED)) {
        draw_texture(moveable->get_location(), moveable->get_size(), moveable->get_texture(), &moveable->evaluated_colour, &moveable->blend, moveable->has_flag(FIXED_POS), &moveable->gradient_colour);
    } else {
        draw_quad(moveable->get_location(), moveable->get_size(), &moveable->evaluated_colour, &moveable->gradient_colour, 0.0f, moveable->has_flag(FIXED_POS));
    }
}

void Render::render_window() {
    glClear(GL_COLOR_BUFFER_BIT);

    float time_shapes = glfwGetTime();
    int skipped_moveables = 0;
    int culled_moveables = 0;

    for (Moveable* moveable: *objects_) {
        if (moveable->get_flags() & (DISABLED | PARTICLES | PANEL | NO_RENDER))
            skipped_moveables++;
        else if ((moveable->location.x + moveable->size.x <= 0) || (moveable->location.y + moveable->size.y <= 0) || (moveable->location.x >= 1) || (moveable->location.y >= 1))
            culled_moveables++;
        else
            render_moveable(moveable);
    }

    skipped_count = skipped_moveables;
    culled_count = culled_moveables;   

    draw_batched_textures();  
    draw_batched_quads();

    draw_times[0] = glfwGetTime() - time_shapes;
    
    float time_text = glfwGetTime();
    
    TextRenderer::init_shader();
    for (Text* text : *text_objects_) {
        if (text->has_flag(DISABLED)) continue;
        draw_text(text->get_location(), text->get_content(), text->get_font(), text->evaluated_colour, text->get_scale(), text->has_flag(FIXED_POS));
    }
    TextRenderer::reset_shader();

    draw_times[1] = glfwGetTime() - time_text;

    glfwSwapBuffers(window_);
}
