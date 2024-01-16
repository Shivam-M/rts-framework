#include "../assets/text.h"
#include "../shapes/circle.h"

#include "render.h"

#define PI 3.14159265358979
#define GLW_SMALL_ROUNDED_CORNER_SLICES 5 

static Vector2 roundedCorners[GLW_SMALL_ROUNDED_CORNER_SLICES] = { {0} };

static void createRoundedCorners(Vector2* corner_points, int num) {
    float a = 0, slice = PI / 2 / num;
    for (int i = 0; i < num; a += slice, i++) corner_points[i].set(cosf(a), sinf(a));
}

static void alignCoordinates(Vector2* location, Vector2* size) {
    size->x *= 2;
    size->y *= 2;
    location->y -= size->y;
}

Render::Render(GLFWwindow* window, vector<Moveable*>* objects, vector<Text*>* text_objects) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    createRoundedCorners(roundedCorners, GLW_SMALL_ROUNDED_CORNER_SLICES);
    window_ = window;
    objects_ = objects;
    text_objects_ = text_objects;
}

void Render::normaliseCoordinates(Vector2* location) {
    float x = location->x + offsets.x, y = location->y + offsets.y;
    location->x = x < 0.5 ? -1 + (x * 2) : x > 0.5 ? ((x - 0.5) * 2) : 0;
    location->y = y < 0.5 ? (-1 + (y * 2)) * -1 : y > 0.5 ? ((y - 0.5) * 2) * -1 : 0;
}

void Render::drawCircle(Circle* circle) {
    drawCircle(circle->getLocation(), circle->getColour(), circle->getGradientColour(), circle->getRadius(), circle->getGenerality());
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

void Render::drawText(Vector2 location, string message, font_data font, Colour colour) {
    glColor4ub(colour.getX(), colour.getY(), colour.getZ(), colour.getW());
    location.x *= resolution.x;
    location.y = resolution.y - location.y * resolution.y;
    print(font, location.x, location.y, message);
}

void Render::drawQuad(Vector2 location, Vector2 size, Colour colour, Colour gradient) {
    normaliseCoordinates(&location);
    alignCoordinates(&location, &size);

    // glMatrixMode(GL_MODELVIEW);
    // glPushMatrix();
    glBegin(GL_QUADS);
    glColor4ub(colour.getX(), colour.getY(), colour.getZ(), colour.getW());
    glVertex2f(location.x, location.y);
    glVertex2f(location.x, location.y + size.y);
    if (colour != gradient) glColor4ub(gradient.getX(), gradient.getY(), gradient.getZ(), gradient.getW());
    glVertex2f(location.x + size.x, location.y + size.y);
    glVertex2f(location.x + size.x, location.y);
    glEnd();
    // glPopMatrix();
}

void Render::drawCurvedQuad(Vector2 location, Vector2 size, Colour colour, Colour gradient, float radius) {
    normaliseCoordinates(&location);
    alignCoordinates(&location, &size);

    float left = location.x, top = location.y, bottom = location.y + size.y, right = location.x + size.x;
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i < GLW_SMALL_ROUNDED_CORNER_SLICES; i++) {
        glColor4ub(colour.getX(), colour.getY(), colour.getZ(), colour.getW());
        glVertex2f(left + radius - radius * roundedCorners[i].x, bottom - radius + radius * roundedCorners[i].y);
        if (colour != gradient) glColor4ub(gradient.getX(), gradient.getY(), gradient.getZ(), gradient.getW());
        glVertex2f(left + radius - radius * roundedCorners[i].x, top + radius - radius * roundedCorners[i].y);
    }

    for (int i = GLW_SMALL_ROUNDED_CORNER_SLICES - 1; i >= 0; i--) {
        glColor4ub(colour.getX(), colour.getY(), colour.getZ(), colour.getW());
        glVertex2f(right - radius + radius * roundedCorners[i].x, bottom - radius + radius * roundedCorners[i].y);
        if (colour != gradient) glColor4ub(gradient.getX(), gradient.getY(), gradient.getZ(), gradient.getW());
        glVertex2f(right - radius + radius * roundedCorners[i].x, top + radius - radius * roundedCorners[i].y);
    }
    glEnd();
}

void Render::drawTexture(Vector2 location, Vector2 size, Texture* texture, Colour colour, bool flip) {
    normaliseCoordinates(&location);
    alignCoordinates(&location, &size);

    // glMatrixMode(GL_MODELVIEW);
    // glPushMatrix();
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
    // glBindTexture(GL_TEXTURE_2D, 0);
    // glPopMatrix();
}

void Render::drawCustom(vector<Vector2> points, Colour colour, Colour gradient) {
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

void Render::renderWindow() {
    glClear(GL_COLOR_BUFFER_BIT);

    float time_shapes = glfwGetTime();
    for (Moveable* moveable: *objects_) {
        if (moveable->getFlags() & DISABLED) continue;
        if ((moveable->location.x + moveable->size.x < 0) || (moveable->location.y + moveable->size.y < 0) || (moveable->location.x > 1) || (moveable->location.y > 1)) continue;
        
        if (moveable->getFlags() & CUSTOM) {
            drawCustom(moveable->getPoints(), moveable->getColour(), moveable->getGradientColour());
        } else if (moveable->getFlags() & CURVED) {
            drawCurvedQuad(moveable->getLocation(), moveable->getSize(), moveable->getColour(), moveable->getGradientColour(), 0.025);
        } else if (moveable->getFlags() & TEXTURED) {
            drawTexture(moveable->getLocation(), moveable->getSize(), moveable->getTexture(), moveable->getColour());
        } else if (moveable->getFlags() & CIRCLE) {
            drawCircle(reinterpret_cast<Circle*>(moveable));
        } else {
            drawQuad(moveable->getLocation(), moveable->getSize(), moveable->getColour(), moveable->getGradientColour());
        }
    }
    draw_times[0] = glfwGetTime() - time_shapes;

    float time_text = glfwGetTime();
    for (Text* text: *text_objects_) if (!(text->getFlags() & DISABLED)) drawText(text->getLocation(), text->getContent(), text->getFont(), text->getColour());
    draw_times[1] = glfwGetTime() - time_text;

    glfwSwapBuffers(window_);
}
