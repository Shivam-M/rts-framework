#include "../assets/text.h"
#include "../shapes/circle.h"
#include "../assets/panel.h"
#include "../assets/particle_group.h"

#include "../tools/text_renderer.h"

#include "render.h"

#define PI 3.14159265358979
#define GLW_SMALL_ROUNDED_CORNER_SLICES 10

static Vector2 roundedCorners[GLW_SMALL_ROUNDED_CORNER_SLICES] = { {0} };

constexpr void createRoundedCorners(Vector2* corner_points, int num) {
    float a = 0, slice = PI / 2 / num;
    for (int i = 0; i < num; a += slice, i++) corner_points[i].set(cosf(a), sinf(a));
}

Render::Render(GLFWwindow* window, vector<Moveable*>* objects, vector<Text*>* text_objects) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    createRoundedCorners(roundedCorners, GLW_SMALL_ROUNDED_CORNER_SLICES);
    window_ = window;
    objects_ = objects;
    text_objects_ = text_objects;
    TextRenderer::setup();
}

void Render::normaliseCoordinates(Vector2* location) {
    float x = location->x * scale +offsets.x, y = location->y * scale + offsets.y;
    location->x = x < 0.5 ? -1 + (x * 2) : x > 0.5 ? ((x - 0.5) * 2) : 0;
    location->y = y < 0.5 ? (-1 + (y * 2)) * -1 : y > 0.5 ? ((y - 0.5) * 2) * -1 : 0;
}

void Render::alignCoordinates(Vector2* location, Vector2* size) {
    size->x *= 2 * scale;
    size->y *= 2 * scale;
    location->y -= size->y;
}

void Render::drawCircle(Circle* circle) {
    drawCircle(circle->getLocation(), circle->getColourRef(), circle->getGradientColourRef(), circle->getRadius(), circle->getGenerality());
}

void Render::drawCircle(Vector2 location, Colour colour, Colour gradient, float radius, float generality) {
    Vector2 original_location = location; // Check change
    normaliseCoordinates(&location);

    if (colour != gradient) glColor4ub(gradient.getX(), gradient.getY(), gradient.getZ(), gradient.getW());

    float resolution_ratio = resolution.x / resolution.y;
    Vector2 circle_coords;
    glVertex2f(location.x, location.y);
    for (float i = 0; i <= 360; i += generality) {
        glColor4ub(colour.getX(), colour.getY(), colour.getZ(), colour.getW());
        circle_coords.x = (radius * cos(PI * i / 180.0) + original_location.x);
        circle_coords.y = (radius * resolution_ratio * sin(PI * i / 180.0) + original_location.y);
        normaliseCoordinates(&circle_coords);
        glVertex2f(circle_coords.x, circle_coords.y);
    }
    glEnd();
}

void Render::drawText(Vector2 location, string message, Font* font, Colour& colour, float font_scale) {
    TextRenderer::render_text(font, location.x * resolution.x, (1 - location.y) * resolution.y, message, colour, font_scale);
}

void Render::drawQuad(Vector2 location, Vector2 size, Colour& colour, Colour& gradient) {
    normaliseCoordinates(&location);
    alignCoordinates(&location, &size);

    glBegin(GL_QUADS);
    glColor4ub(colour.getX(), colour.getY(), colour.getZ(), colour.getW());
    glVertex2f(location.x, location.y);
    glVertex2f(location.x, location.y + size.y);
    if (colour != gradient) glColor4ub(gradient.getX(), gradient.getY(), gradient.getZ(), gradient.getW());
    glVertex2f(location.x + size.x, location.y + size.y);
    glVertex2f(location.x + size.x, location.y);
    glEnd();
}

void Render::drawCurvedQuad(Vector2 location, Vector2 size, Colour& colour, Colour& gradient, float radius) {
    normaliseCoordinates(&location);
    alignCoordinates(&location, &size);

    float left = location.x, top = location.y, bottom = location.y + size.y, right = location.x + size.x;
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i < GLW_SMALL_ROUNDED_CORNER_SLICES; i++) {
        glColor4ub(colour.getX(), colour.getY(), colour.getZ(), colour.getW());
        glVertex2f(left + radius - radius * roundedCorners[i].x, bottom - radius + radius * roundedCorners[i].y);
        // if (colour != gradient) glColor4ub(gradient.getX(), gradient.getY(), gradient.getZ(), gradient.getW());
        glVertex2f(left + radius - radius * roundedCorners[i].x, top + radius - radius * roundedCorners[i].y);
    }

    for (int i = GLW_SMALL_ROUNDED_CORNER_SLICES - 1; i >= 0; i--) {
        if (colour != gradient) glColor4ub(gradient.getX(), gradient.getY(), gradient.getZ(), gradient.getW());
        // glColor4ub(colour.getX(), colour.getY(), colour.getZ(), colour.getW());
        glVertex2f(right - radius + radius * roundedCorners[i].x, bottom - radius + radius * roundedCorners[i].y);
        // if (colour != gradient) glColor4ub(gradient.getX(), gradient.getY(), gradient.getZ(), gradient.getW());
        glVertex2f(right - radius + radius * roundedCorners[i].x, top + radius - radius * roundedCorners[i].y);
    }
    glEnd();
}

void Render::drawTexture(Vector2 location, Vector2 size, Texture* texture, Colour& colour, bool flip) {
    normaliseCoordinates(&location);
    alignCoordinates(&location, &size);

    glBindTexture(GL_TEXTURE_2D, texture->data);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glColor4ub(colour.getX(), colour.getY(), colour.getZ(), colour.getW());
    glTexCoord2i(flip, 1);     glVertex2f(location.x, location.y);
    glTexCoord2i(flip, 0);     glVertex2f(location.x, location.y + size.y);
    glTexCoord2i(1 - flip, 0); glVertex2f(location.x + size.x, location.y + size.y);
    glTexCoord2i(1 - flip, 1); glVertex2f(location.x + size.x, location.y);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Render::drawCustom(vector<Vector2> points, Colour colour, Colour gradient) {
    return;
    glBegin(GL_QUADS);
    glColor4ub(colour.getX(), colour.getY(), colour.getZ(), colour.getW());
    for (int i = 0; i < 4; i++) {
        if (i == 2 && gradient != colour) glColor4ub(gradient.getX(), gradient.getY(), gradient.getZ(), gradient.getW());
        points[i].set(points[i].x * scale + offsets.x, points[i].y * scale + offsets.y);
        normaliseCoordinates(&points[i]);
        glVertex2f(points[i].x, points[i].y);
    }
    glEnd();
}

void Render::toggleFullscreen() {
    fullscreen_ = !fullscreen_;
    glfwSetWindowMonitor(window_, fullscreen_ ? glfwGetPrimaryMonitor() : NULL, 100, 100, resolution.x, resolution.y, GLFW_DONT_CARE);
    renderWindow();
}

void Render::drawQuadBatch() {
    glBegin(GL_QUADS);
    for (QuadData& quad : quadBatch) {
        normaliseCoordinates(&quad.location);
        alignCoordinates(&quad.location, &quad.size);
        glColor4ub(quad.colour.getX(), quad.colour.getY(), quad.colour.getZ(), quad.colour.getW());
        glVertex2f(quad.location.x, quad.location.y);
        glVertex2f(quad.location.x, quad.location.y + quad.size.y);
        glColor4ub(quad.gradient.getX(), quad.gradient.getY(), quad.gradient.getZ(), quad.gradient.getW());
        glVertex2f(quad.location.x + quad.size.x, quad.location.y + quad.size.y);
        glVertex2f(quad.location.x + quad.size.x, quad.location.y);
    }
    glEnd();
    quadBatch.clear();
}

void Render::drawTextureBatch() {
    glEnable(GL_TEXTURE_2D);
    for (TextureData& tx: textureBatch) {
        normaliseCoordinates(&tx.location);
        alignCoordinates(&tx.location, &tx.size);
        glBindTexture(GL_TEXTURE_2D, tx.texture->data);
        glBegin(GL_QUADS);
        glColor4ub(tx.colour.getX(), tx.colour.getY(), tx.colour.getZ(), tx.colour.getW());
        glTexCoord2i(tx.flip, 1);     glVertex2f(tx.location.x, tx.location.y);
        glTexCoord2i(tx.flip, 0);     glVertex2f(tx.location.x, tx.location.y + tx.size.y);
        glTexCoord2i(1 - tx.flip, 0); glVertex2f(tx.location.x + tx.size.x, tx.location.y + tx.size.y);
        glTexCoord2i(1 - tx.flip, 1); glVertex2f(tx.location.x + tx.size.x, tx.location.y);
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
    textureBatch.clear();
}

void Render::renderMoveable(Moveable* moveable) {
    if (moveable->getFlags() & CUSTOM) {
        drawCustom(moveable->getPoints(), moveable->getColourRef(), moveable->getGradientColourRef());
    } else if (moveable->getFlags() & CURVED) {
        drawCurvedQuad(moveable->getLocation(), moveable->getSize(), moveable->getColourRef(), moveable->getGradientColourRef(), 0.025);
    } else if (moveable->getFlags() & TEXTURED) {
        drawTextureB(moveable->getLocation(), moveable->getSize(), moveable->getTexture(), moveable->getColourRef());
    } else if (moveable->getFlags() & CIRCLE) {
        drawCircle(reinterpret_cast<Circle*>(moveable));
    } else {
        drawQuad(moveable->getLocation(), moveable->getSize(), moveable->getColourRef(), moveable->getGradientColourRef());
    }
}

void Render::renderWindow() { // todo: don't render off screen when zoomed
    glClear(GL_COLOR_BUFFER_BIT);

    float time_shapes = glfwGetTime();
    for (Moveable* moveable: *objects_) {
        if (moveable->getFlags() & (DISABLED | PARTICLES | PANEL)) continue;
        if ((moveable->location.x + moveable->size.x < 0) || (moveable->location.y + moveable->size.y < 0) || (moveable->location.x > 1) || (moveable->location.y > 1)) continue;
        renderMoveable(moveable);
    }

    drawTextureBatch(); // Note: won't take into account object order.
    draw_times[0] = glfwGetTime() - time_shapes;
    
    float time_text = glfwGetTime(); // TEMP
    
    for (Text* text : *text_objects_) if (!(text->getFlags() & DISABLED) && (text->getFlags() & TEXT_BACKGROUND)) {
        Vector2 dims = TextRenderer::calculate_text_dimensions(text->getFont(), text->getContent(), text->getScale());
        dims.x /= resolution.x;
        dims.y /= resolution.y;
        Vector2 loc = text->getLocation();
        loc.y -= dims.y;

        float bg_scale = 1.05f;
        dims.x *= bg_scale;
        dims.y *= bg_scale * 1.5;

        loc.x -= dims.x * (bg_scale - 1) / (2 * bg_scale);
        loc.y -= dims.y * (bg_scale * 1.5 - 1) / (2 * bg_scale * 1.5);

        Colour c1 = Colour(40, 40, 40, min(text->getColourRef().getW(), 200.f));

        drawCurvedQuad(loc, dims, c1, c1, 0.0125);
    }
    
    TextRenderer::init_shader();
    for (Text* text : *text_objects_) if (!(text->getFlags() & DISABLED)) {
        drawText(!text->hasFlag(FIXED_POS) ? text->getLocation() * scale + offsets : text->getLocation(), text->getContent(), text->getFont(), text->getColourRef(), !text->hasFlag(FIXED_POS) ? text->getScale() * scale : text->getScale());
    }
    TextRenderer::reset_shader();
    draw_times[1] = glfwGetTime() - time_text;

    glfwSwapBuffers(window_);

    /*
    int w, h;
    glfwGetWindowSize(window_, &w, &h);
    glViewport(0, 0, w, h); // use vp instead of offset/scale
    resolution.x = w;
    resolution.y = h;
    */

}
