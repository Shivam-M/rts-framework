#include "render.h"

#include "../assets/text.h"
#include "../tools/text_renderer.h"
#include "../tools/common.h"

using namespace std;

GLuint textureVAO, textureVBO, textureShader;
GLuint colourShader;

static GLint uniformColourQuad, uniformColourSecondaryQuad, uniformRadiusQuad, uniformTime, uniformType, uniformSpeed, uniformSize, uniformDirection;
static GLint uniformColour, uniformColourSecondary, uniformTexture, uniformTextureSecondary, uniformUseSecondTexture;

Render::Render(GLFWwindow* window, vector<Moveable*>* objects, vector<Text*>* text_objects) {
    window_ = window;
    objects_ = objects;
    text_objects_ = text_objects;
    TextRenderer::setup();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    textureShader = compile_shader("shaders/textured.vert", "shaders/textured.frag");
    glUseProgram(textureShader);

    uniformColour = glGetUniformLocation(textureShader, "colour");
    uniformColourSecondary = glGetUniformLocation(textureShader, "colourSecondary");
    uniformTexture = glGetUniformLocation(textureShader, "texturePrimary");
    uniformTextureSecondary = glGetUniformLocation(textureShader, "textureSecondary");
    uniformUseSecondTexture = glGetUniformLocation(textureShader, "useSecondTexture");
    uniformTime = glGetUniformLocation(textureShader, "time");
    uniformType = glGetUniformLocation(textureShader, "type");
	uniformSpeed = glGetUniformLocation(textureShader, "speed");
	uniformSize = glGetUniformLocation(textureShader, "size");
	uniformDirection = glGetUniformLocation(textureShader, "direction");

    glUniform1i(uniformTexture, 0);
    glUniform1i(uniformTextureSecondary, 1);
    glUniform1i(uniformType, 0);
    glUniform1f(uniformTime, 1.f);
    glUniform1f(uniformSpeed, 1.f);
    glUniform1f(uniformSize, 1.f);
    glUniform2f(uniformDirection, 0.5, 0.5);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT));
    glUniformMatrix4fv(glGetUniformLocation(textureShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glGenVertexArrays(1, &textureVAO);
    glGenBuffers(1, &textureVBO);

    glBindVertexArray(textureVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 1000 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --

    colourShader = compile_shader("shaders/coloured.vert", "shaders/coloured.frag");
    glUseProgram(colourShader);

    glUniformMatrix4fv(glGetUniformLocation(colourShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    uniformColourQuad = glGetUniformLocation(colourShader, "colour");
    uniformColourSecondaryQuad = glGetUniformLocation(colourShader, "colourSecondary");
    uniformRadiusQuad = glGetUniformLocation(colourShader, "radius");
}

void Render::draw_text(Vector2 location, const string& message, Font* font, const Colour& colour, const float& font_scale) {
    TextRenderer::render_text(font, location.x * resolution.x, (location.y) * resolution.y, message, colour, font_scale);
}

GLfloat quad_vertex_buffer[1000 * 16];
void Render::draw_batched_quads() {
    glBindVertexArray(textureVAO);
    glUseProgram(colourShader);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);

    int vertex_count = 0;

    for (int i = 0; i < quad_count_; i++) {
        const QuadData& quad_data = batched_quads_[i];
        GLfloat x, y, w, h;

        if (quad_data.fixed_position) {
            x = quad_data.location.x * WINDOW_WIDTH;
            y = (1.0f - (quad_data.location.y) - quad_data.size.y) * WINDOW_HEIGHT;
            w = quad_data.size.x * WINDOW_WIDTH;
            h = quad_data.size.y * WINDOW_HEIGHT;
        } else {
            x = (quad_data.location.x * this->scale + this->offsets.x) * WINDOW_WIDTH;
            y = (1.0f - (quad_data.location.y * this->scale + this->offsets.y) - quad_data.size.y * this->scale) * WINDOW_HEIGHT;
            w = quad_data.size.x * this->scale * WINDOW_WIDTH;
            h = quad_data.size.y * this->scale * WINDOW_HEIGHT;
        }

        quad_vertex_buffer[vertex_count++] = x;
        quad_vertex_buffer[vertex_count++] = y + h;
        quad_vertex_buffer[vertex_count++] = 0.0f;
        quad_vertex_buffer[vertex_count++] = 0.0f;

        quad_vertex_buffer[vertex_count++] = x;
        quad_vertex_buffer[vertex_count++] = y;
        quad_vertex_buffer[vertex_count++] = 0.0f;
        quad_vertex_buffer[vertex_count++] = 1.0f;

        quad_vertex_buffer[vertex_count++] = x + w;
        quad_vertex_buffer[vertex_count++] = y;
        quad_vertex_buffer[vertex_count++] = 1.0f;
        quad_vertex_buffer[vertex_count++] = 1.0f;

        quad_vertex_buffer[vertex_count++] = x + w;
        quad_vertex_buffer[vertex_count++] = y + h;
        quad_vertex_buffer[vertex_count++] = 1.0f;
        quad_vertex_buffer[vertex_count++] = 0.0f;
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * sizeof(GLfloat), quad_vertex_buffer);

    for (int i = 0; i < quad_count_; i++) {
        const QuadData& quad_data = batched_quads_[i];
        const Colour* colour = quad_data.colour;
        const Colour* colour_gradient = quad_data.gradient;

        // glUniform1f(uniformPriorityQuad, quad_data.priority);
        glUniform1f(uniformRadiusQuad, quad_data.radius);
        glUniform4f(uniformColourQuad, colour->x_ / 255.0f, colour->y_ / 255.0f, colour->z_ / 255.0f, colour->w_ / 255.0f);
        
        if (colour_gradient) {
            glUniform4f(uniformColourSecondaryQuad, colour_gradient->x_ / 255.0f, colour_gradient->y_ / 255.0f, colour_gradient->z_ / 255.0f, colour_gradient->w_ / 255.0f);
        } else {
            glUniform4f(uniformColourSecondaryQuad, colour->x_ / 255.0f, colour->y_ / 255.0f, colour->z_ / 255.0f, colour->w_ / 255.0f);
        }

        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }

    quad_count_ = 0;
}

void Render::draw_custom(vector<Vector2> points, Colour colour, Colour gradient) {
    return; // TODO: Reimplement custom shapes for sidescroller
}

void Render::toggle_fullscreen() {
    fullscreen_ = !fullscreen_;
    glfwSetWindowMonitor(window_, fullscreen_ ? glfwGetPrimaryMonitor() : NULL, 100, 100, resolution.x, resolution.y, GLFW_DONT_CARE);
    render_window();
}

GLfloat vertex_buffer[1000 * 16];
void Render::draw_batched_textures() {
    glBindVertexArray(textureVAO);
    glUseProgram(textureShader);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);

    int vertex_count = 0;

    for (int i = 0; i < texture_count_; i++) {
        const TextureData& texture_data = batched_textures_[i];
        GLfloat x, y, w, h;

        if (texture_data.fixed_position) {
            x = texture_data.location.x * WINDOW_WIDTH;
            y = (1.0f - (texture_data.location.y) - texture_data.size.y) * WINDOW_HEIGHT;
            w = texture_data.size.x * WINDOW_WIDTH;
            h = texture_data.size.y * WINDOW_HEIGHT;
        } else {
            x = (texture_data.location.x * this->scale + this->offsets.x) * WINDOW_WIDTH;
            y = (1.0f - (texture_data.location.y * this->scale + this->offsets.y) - texture_data.size.y * this->scale) * WINDOW_HEIGHT;
            w = texture_data.size.x * this->scale * WINDOW_WIDTH;
            h = texture_data.size.y * this->scale * WINDOW_HEIGHT;
        }

        vertex_buffer[vertex_count++] = x;
        vertex_buffer[vertex_count++] = y + h;
        vertex_buffer[vertex_count++] = 0.0f;
        vertex_buffer[vertex_count++] = 0.0f;

        vertex_buffer[vertex_count++] = x;
        vertex_buffer[vertex_count++] = y;
        vertex_buffer[vertex_count++] = 0.0f;
        vertex_buffer[vertex_count++] = 1.0f;

        vertex_buffer[vertex_count++] = x + w;
        vertex_buffer[vertex_count++] = y;
        vertex_buffer[vertex_count++] = 1.0f;
        vertex_buffer[vertex_count++] = 1.0f;

        vertex_buffer[vertex_count++] = x + w;
        vertex_buffer[vertex_count++] = y + h;
        vertex_buffer[vertex_count++] = 1.0f;
        vertex_buffer[vertex_count++] = 0.0f;
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * sizeof(GLfloat), vertex_buffer);

    const float time = glfwGetTime();

    for (int i = 0; i < texture_count_; i++) {
        const TextureData& texture_data = batched_textures_[i];
        const Colour* colour = texture_data.colour;
        const Colour* secondary_colour = texture_data.secondary_colour;
        const Blend* blend = texture_data.blend;

        glUniform4f(uniformColour, colour->x_ / 255.0f, colour->y_ / 255.0f, colour->z_ / 255.0f, colour->w_ / 255.0f);
        glUniform1i(uniformType, blend->type);

        if (blend->type != 0) {
            glUniform1f(uniformTime, blend->fixed ? 1.0f : time);
            glUniform1f(uniformSpeed, blend->speed);
            glUniform1f(uniformSize, blend->size);
            glUniform2f(uniformDirection, blend->direction.x, blend->direction.y);
        }

        if (secondary_colour) {
            glUniform4f(uniformColourSecondary, secondary_colour->x_ / 255.0f, secondary_colour->y_ / 255.0f, secondary_colour->z_ / 255.0f, secondary_colour->w_ / 255.0f);
        }

        glBindTexture(GL_TEXTURE_2D, texture_data.texture->data);
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }

    texture_count_ = 0;
}

void Render::render_moveable(Moveable* moveable) {
    if (moveable->has_flag(CUSTOM)) {
        // draw_custom(moveable->get_points(), moveable->get_colour_ref(), moveable->get_gradient_colour_ref());
    } else if (moveable->has_flag(CURVED)) {
        draw_quad(moveable->get_location(), moveable->get_size(), &moveable->evaluated_colour, &moveable->gradient_colour, 0.025f, moveable->has_flag(FIXED_POS));
    } else if (moveable->has_flag(TEXTURED)) {
        draw_texture(moveable->get_location(), moveable->get_size(), moveable->get_texture(), &moveable->evaluated_colour, &moveable->blend, moveable->has_flag(FIXED_POS), moveable->get_secondary_texture(), &moveable->gradient_colour);
    } else {
        draw_quad(moveable->get_location(), moveable->get_size(), &moveable->evaluated_colour, &moveable->gradient_colour, 0.0f, moveable->has_flag(FIXED_POS));
    }
}

void Render::render_window() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time_shapes = glfwGetTime();
    int skipped_moveables = 0;
    int culled_moveables = 0;

    for (Moveable* moveable: *objects_) {
        if (moveable->get_flags() & (DISABLED | PARTICLES | PANEL | NO_RENDER))
            skipped_moveables++;
        else if ((moveable->location.x + moveable->size.x <= 0) || (moveable->location.y + moveable->size.y <= 0) || (moveable->location.x >= 1) || (moveable->location.y >= 1))
            culled_moveables++;
        else render_moveable(moveable);
    }

    skipped_count = skipped_moveables;
    culled_count = culled_moveables;   
    
    // glDepthMask(GL_FALSE);
    
    // glDisable(GL_DEPTH_TEST);
    draw_batched_textures();
    
    // glEnable(GL_DEPTH_TEST);    
    draw_batched_quads();

    draw_times[0] = glfwGetTime() - time_shapes;
    
    float time_text = glfwGetTime();

    Vector2 text_dimensions, text_location;
    Colour text_background_colour = Colour(40, 40, 40, 255);
    const float background_scale = 1.05f;
    for (Text* text : *text_objects_) if (!(text->get_flags() & DISABLED) && (text->get_flags() & TEXT_BACKGROUND)) {
        text_dimensions = text->dimensions;
        text_dimensions /= resolution;

        text_location = text->get_location();
        text_location.y -= text_dimensions.y;

        text_dimensions.x *= background_scale;
        text_dimensions.y *= background_scale * (16.0 / 9.0);
        text_location.x -= text_dimensions.x * (background_scale - 1) / (2 * background_scale);
        text_location.y -= text_dimensions.y * (background_scale * 1.5 - 1) / (2 * background_scale * 1.5);

        text_background_colour.set_alpha(min(text->colour.get_w(), 200.f));
        draw_quad(text_location, text_dimensions, &text_background_colour, &text_background_colour, 0.0f, false);
    }

    draw_batched_quads();
    
    TextRenderer::init_shader();
    for (Text* text : *text_objects_) if (!(text->get_flags() & DISABLED)) {
        draw_text(
            text->has_flag(FIXED_POS) ? text->get_location() : text->get_location() * scale + offsets,
            text->get_content(),
            text->get_font(),
            text->evaluated_colour,
            text->has_flag(FIXED_POS) ? text->get_scale() : text->get_scale() * scale);
    }
    TextRenderer::reset_shader();
    draw_times[1] = glfwGetTime() - time_text;

    // glEnable(GL_DEPTH_TEST);
    // draw_batched_quads();

    glfwSwapBuffers(window_);

    /*
    int w, h;
    glfwGetWindowSize(window_, &w, &h);
    glViewport(0, 0, w, h); // use vp instead of offset/scale
    resolution.x = w;
    resolution.y = h;
    */
}
