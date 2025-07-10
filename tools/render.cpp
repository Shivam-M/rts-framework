#include "../assets/text.h"
#include "../tools/text_renderer.h"
#include "render.h"

GLuint textureVAO, textureVBO, textureShader;
GLuint colourShader;

static GLint uniformColourQuad, uniformColourSecondaryQuad, uniformRadiusQuad, uniformPriorityQuad, uniformTime, uniformType, uniformSpeed, uniformSize, uniformDirection;
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

    textureShader = CompileShader("shaders/textured.vert", "shaders/textured.frag");
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

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --

    colourShader = CompileShader("shaders/coloured.vert", "shaders/coloured.frag");
    glUseProgram(colourShader);

    glUniformMatrix4fv(glGetUniformLocation(colourShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    uniformColourQuad = glGetUniformLocation(colourShader, "colour");
    uniformColourSecondaryQuad = glGetUniformLocation(colourShader, "colourSecondary");
    uniformRadiusQuad = glGetUniformLocation(colourShader, "radius");
    uniformPriorityQuad = glGetUniformLocation(colourShader, "z_value");
}

void Render::normaliseCoordinates(Vector2* location) {
    float x = location->x, y = location->y;
    x = location->x * scale + offsets.x, y = location->y * scale + offsets.y;
    location->x = x < 0.5 ? -1 + (x * 2) : x > 0.5 ? ((x - 0.5) * 2) : 0;
    location->y = y < 0.5 ? (-1 + (y * 2)) * -1 : y > 0.5 ? ((y - 0.5) * 2) * -1 : 0;
}

void Render::alignCoordinates(Vector2* location, Vector2* size) {
    size->x *= 2 * scale;
    size->y *= 2 * scale;
    location->y -= size->y;
}

void Render::drawText(Vector2 location, string& message, Font* font, Colour& colour, float font_scale, float priority) {
    TextRenderer::render_text(font, location.x * resolution.x, (location.y) * resolution.y, message, colour, font_scale, priority);
}

void Render::drawQuadBatch() {
    glBindVertexArray(textureVAO);
    glUseProgram(colourShader);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);

    GLfloat x, y, w, h, vertices[16];

    vertices[2] = 0.0f;   vertices[3] = 0.0f;
    vertices[6] = 0.0f;   vertices[7] = 1.0f;
    vertices[10] = 1.0f;  vertices[11] = 1.0f;
    vertices[14] = 1.0f;  vertices[15] = 0.0f;

    for (int i = 0; i < quad_count_; i++) {
        QuadData quad = batched_quads_[i];
        float scale = quad.fixed_position ? 1.0f : this->scale;
        Vector2 offsets = quad.fixed_position ? Vector2{ 0, 0 } : this->offsets;

        x = (quad.location.x * scale + offsets.x) * WINDOW_WIDTH;
        y = (1.0f - (quad.location.y * scale + offsets.y) - quad.size.y * scale) * WINDOW_HEIGHT;
        w = quad.size.x * scale * WINDOW_WIDTH;
        h = quad.size.y * scale * WINDOW_HEIGHT;

        vertices[0] = x;      vertices[1] = y + h;
        vertices[4] = x;      vertices[5] = y;
        vertices[8] = x + w;  vertices[9] = y;
        vertices[12] = x + w; vertices[13] = y + h;

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glUniform1f(uniformRadiusQuad, quad.radius);

        const Colour* colour = quad.colour;
        glUniform4f(uniformColourQuad, colour->x_ / 255.0f, colour->y_ / 255.0f, colour->z_ / 255.0f, colour->w_ / 255.0f);

        const Colour* colour_gradient = quad.gradient;
        glUniform4f(uniformColourSecondaryQuad, colour_gradient->x_ / 255.0f, colour_gradient->y_ / 255.0f, colour_gradient->z_ / 255.0f, colour_gradient->w_ / 255.0f);

        glUniform1f(uniformPriorityQuad, quad.priority);

        if (quad.size.x >= 1.00) {
            bool t = true;
        }

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    quad_count_ = 0;
}

void Render::drawCustom(vector<Vector2> points, Colour colour, Colour gradient) {
    return;
    /*glBegin(GL_QUADS);
    glColor4ub(colour.getX(), colour.getY(), colour.getZ(), colour.getW());
    for (int i = 0; i < 4; i++) {
        if (i == 2 && gradient != colour) glColor4ub(gradient.getX(), gradient.getY(), gradient.getZ(), gradient.getW());
        points[i].set(points[i].x * scale + offsets.x, points[i].y * scale + offsets.y);
        normaliseCoordinates(&points[i]);
        glVertex2f(points[i].x, points[i].y);
    }
    glEnd();*/
}

void Render::toggleFullscreen() {
    fullscreen_ = !fullscreen_;
    glfwSetWindowMonitor(window_, fullscreen_ ? glfwGetPrimaryMonitor() : NULL, 100, 100, resolution.x, resolution.y, GLFW_DONT_CARE);
    renderWindow();
}

void Render::drawTextureBatch() {
    glBindVertexArray(textureVAO);
    glUseProgram(textureShader);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);

    GLfloat x, y, w, h, vertices[16];

    vertices[2] = 0.0f;   vertices[3] = 0.0f;
    vertices[6] = 0.0f;   vertices[7] = 1.0f;
    vertices[10] = 1.0f;  vertices[11] = 1.0f;
    vertices[14] = 1.0f;  vertices[15] = 0.0f;

	time = glfwGetTime();

    for (int i = 0; i < texture_count_; i++) {
		TextureData tx = batched_textures_[i];
        if (tx.texture == nullptr || tx.colour == nullptr) continue; // temp
        float scale = tx.fixed_position ? 1.0f : this->scale;
        Vector2 offsets = tx.fixed_position ? Vector2{ 0, 0 } : this->offsets;

        x = (tx.location.x * scale + offsets.x) * WINDOW_WIDTH;
        y = (1.0f - (tx.location.y * scale + offsets.y) - tx.size.y * scale) * WINDOW_HEIGHT;
        w = tx.size.x * scale * WINDOW_WIDTH;
        h = tx.size.y * scale * WINDOW_HEIGHT;

        vertices[0] = x;      vertices[1] = y + h;
        vertices[4] = x;      vertices[5] = y;
        vertices[8] = x + w;  vertices[9] = y;
        vertices[12] = x + w; vertices[13] = y + h;

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        Colour* colour = tx.colour;
        glUniform4f(uniformColour, colour->x_ / 255.0f, colour->y_ / 255.0f, colour->z_ / 255.0f, colour->w_ / 255.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tx.texture->data);
        const Colour* secondary_colour = tx.secondary_colour;
        /*if (tx.secondary_texture) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, tx.secondary_texture->data);
            glUniform1i(uniformUseSecondTexture, 1);
        } else {
            glUniform1i(uniformUseSecondTexture, 0);
        }*/

        /*if (tx.location.x == 0 && tx.location.y == 0) {
            glUniform1f(uniformTime, glfwGetTime());
        }
        else {
			glUniform1f(uniformTime, 0);
        }

        glUniform1f(uniformTime, glfwGetTime());*/


        Blend* blend = tx.blend;

        glUniform1i(uniformType, blend->type);

        if (blend->type != 0) {
            glUniform1f(uniformTime, blend->fixed ? 1.0f : time);
            glUniform1f(uniformSpeed, blend->speed);
            glUniform1f(uniformSize, blend->size);
			glUniform2f(uniformDirection, blend->direction.x, blend->direction.y);
            //glUniform1f(uniformTime, 1.f);
            // dir glm::vec2
        }

        
        if (!tx.secondary_colour) {
            glUniform4f(uniformColourSecondary, colour->x_ / 255.0f, colour->y_ / 255.0f, colour->z_ / 255.0f, colour->w_ / 255.0f);
        } else {
            glUniform4f(uniformColourSecondary, secondary_colour->x_ / 255.0f, secondary_colour->y_ / 255.0f, secondary_colour->z_ / 255.0f, secondary_colour->w_ / 255.0f);
        }

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    texture_count_ = 0;
}

void Render::renderMoveable(Moveable* moveable) {
    if (moveable->hasFlag(CUSTOM)) {
        // drawCustom(moveable->getPoints(), moveable->getColourRef(), moveable->getGradientColourRef());
    } else if (moveable->hasFlag(CURVED)) {
        drawQuadB(moveable->getLocation(), moveable->getSize(), &moveable->getColourRef(), &moveable->getGradientColourRef(), moveable->getPriority(), 0.025, moveable->hasFlag(FIXED_POS));
    } else if (moveable->hasFlag(TEXTURED)) {
        drawTextureB(moveable->getLocation(), moveable->getSize(), moveable->getTexture(), &moveable->getColourRef(), &moveable->getBlend(), moveable->hasFlag(FIXED_POS), moveable->getSecondaryTexture(), &moveable->getGradientColourRef());
    } else {
        drawQuadB(moveable->getLocation(), moveable->getSize(), &moveable->getColourRef(), &moveable->getGradientColourRef(), moveable->getPriority(), 0.0f, moveable->hasFlag(FIXED_POS));
    }
}

void Render::renderWindow() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time_shapes = glfwGetTime();
    int skipped_moveables = 0;
    int culled_moveables = 0;

    /*float left = 0 + offsets.x * -1;
    float right = 1 + offsets.x * -1;
    float top = 0 + offsets.y * -1;
    float bottom = 1 + offsets.y * -1;*/

    /*log_t("left ", left);
    log_t("right ", right);
    log_t("top ", top);
    log_t("bottom ", bottom);
    log_t("--");*/

    for (Moveable* moveable: *objects_) {
        if (moveable->getFlags() & (DISABLED | PARTICLES | PANEL | NO_RENDER)) {
            skipped_moveables++;
            continue;
        }
        if ((moveable->location.x + moveable->size.x <= 0) || (moveable->location.y + moveable->size.y <= 0) || (moveable->location.x >= 1) || (moveable->location.y >= 1)) {
        // if ((moveable->location.x + moveable->size.x <= left) || (moveable->location.x >= right)) {
        // if (not (moveable->location.x >= left && moveable->location.x <= right)) {
            culled_moveables++;
        } else renderMoveable(moveable);
    }

    skipped_count = skipped_moveables;
    culled_count = culled_moveables;   
    
    // glDepthMask(GL_FALSE);
    
    //glDisable(GL_DEPTH_TEST);
    drawTextureBatch();
    
    //glEnable(GL_DEPTH_TEST);    
    drawQuadBatch();

    draw_times[0] = glfwGetTime() - time_shapes;
    
    float time_text = glfwGetTime();

    Vector2 text_dimensions, text_location;
    Colour text_background_colour = Colour(40, 40, 40, 255);
    float background_scale = 1.05f;
    for (Text* text : *text_objects_) if (!(text->getFlags() & DISABLED) && (text->getFlags() & TEXT_BACKGROUND)) {
        text_dimensions = text->getTextSize();
        text_dimensions = text_dimensions / resolution;

        text_location = text->getLocation();
        text_location.y -= text_dimensions.y;

        text_dimensions.x *= background_scale;
        text_dimensions.y *= background_scale * (16.0 / 9.0);
        text_location.x -= text_dimensions.x * (background_scale - 1) / (2 * background_scale);
        text_location.y -= text_dimensions.y * (background_scale * 1.5 - 1) / (2 * background_scale * 1.5);

        text_background_colour.setW(min(text->getColourRef().getW(), 200.f));
        drawQuadB(text_location, text_dimensions, &text_background_colour, &text_background_colour, 0.00, 0.0125);
    }

    drawQuadBatch();
    
    TextRenderer::init_shader();
    for (Text* text : *text_objects_) if (!(text->getFlags() & DISABLED)) {
        drawText(
            text->hasFlag(FIXED_POS) ? text->getLocation() : text->getLocation() * scale + offsets,
            text->getContent(),
            text->getFont(),
            text->getColourRef(),
            text->hasFlag(FIXED_POS) ? text->getScale() : text->getScale() * scale,
            text->getPriority());
    }
    TextRenderer::reset_shader();
    draw_times[1] = glfwGetTime() - time_text;

    // glEnable(GL_DEPTH_TEST);
    // drawQuadBatch();

    glfwSwapBuffers(window_);

    /*
    int w, h;
    glfwGetWindowSize(window_, &w, &h);
    glViewport(0, 0, w, h); // use vp instead of offset/scale
    resolution.x = w;
    resolution.y = h;
    */
}
