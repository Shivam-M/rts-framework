#define SKIP
#ifndef SKIP
#include "render.h"
#include "../assets/player.h"
#include "../assets/enemy.h"
#define PI 3.14159265358979


// 06/08/2021 - new text renderer from https://github.com/benhj/glfreetype

double an = 0;

Render::Render() {}

Render::Render(GLFWwindow* w, vector<Moveable*>* o, vector<Text*>* t, vector<Text*>* s) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    window_ = w;
    objects_ = o;
    text_objects_ = t;
    secondary_objects_ = s;
}

void Render::drawText(Text* t) {
    if (t->getFlags() & DISABLED) return;
    drawText(t->getContent(), t->getLocation(), t->getFont(), t->getGLColour());
    if (debug_) {
        // drawQuad(t->getLocation(), t->getSize(), new double[4]{ 1.0, 0.6, 0.8, 0.3 }, new double[4]{ 1.0, 0.6, 0.8, 0.3 });
        if (Moveable* m = t->getCollisionBox()) if (t->getFlags() & SELECTED) drawQuad(m);
    }
}

void Render::drawText(string text, double* location, font_data font, double* colour) {
    drawText(text, location[0], location[1], font, colour);
}

void Render::drawText(string text, double x, double y, font_data font, double* colour) {
    int* resolution = retrieveDimensions();

    // x += offsets[0];
    // y += offsets[1];

    glColor4dv(colour);
    x *= resolution[0] * internal_scale_w;
    y *= resolution[1] * internal_scale_h;
    y  = resolution[1] - y;
    y *= internal_scale_h;
    print(font, x + viewport[0] + offsets[0], y + offsets[1], text);
    delete resolution;
}

void Render::drawCircle(Circle* c) {
    if (c->getFlags() & RING)
        if (c->getFlags() & BACKGROUND) drawRing(c->getLocation()[0] - ((*objects_)[0]->getLocation()[0] * 0.03), c->getLocation()[1], c->getRadius(), c->getGLColour(), c->getPercentage());
        else drawRing(c->getLocation()[0], c->getLocation()[1], c->getRadius(), c->getGLColour(), c->getPercentage());
    else {
        if (c->getFlags() & BACKGROUND) drawCircle(c->getLocation()[0] - ((*objects_)[0]->getLocation()[0] * 0.03), c->getLocation()[1], c->getRadius(), c->getGLColour());
        else drawCircle(c->getLocation(), c->getRadius(), c->getGLColour(), c->getGLGradient(), c->getGenerality());
    } if (debug_) drawQuad(c->getLocation(), c->getSize(), new double[4]{ 1.0, 0.6, 0.8, 0.3 }, new double[4]{ 1.0, 0.6, 0.8, 0.3 });
}

void Render::drawCircle(double* location, double radius, double* colour, double* gradient_colour, double generality) {
    drawCircle(location[0] * scale + offsets[0], location[1] * scale + offsets[1], radius * scale, colour, gradient_colour, generality);
}

void Render::drawCircle(double x, double y, double radius, double* colour, double* gradient_colour, double generality) {
    if (x <= -1 || x >= 2 || y <= -1 || y >= 2) return; // lenient, can change to meet individual needs
    int* dimensions = retrieveDimensions();
    
    x += 0.03;
    y += 0.0575;

    double original_x = x;
    double original_y = y;
    
    glBegin(GL_TRIANGLE_FAN);
    normaliseCoordinates(&x, &y);

    colour == nullptr ? glColor4d(1.0, 1.0, 1.0, 1.0) : glColor4dv(colour);
    gradient_colour = gradient_colour == nullptr ? colour : gradient_colour;

    glVertex2f((GLfloat)x, (GLfloat)y);
    for (float i = 0.0f; i <= 360; i += generality) {
        glColor4dv(gradient_colour);
        double cir_x = (radius * cos(PI * i / 180.0) + original_x);
        double cir_y = (radius * (float(dimensions[0]) / dimensions[1]) * sin(PI * i / 180.0) + original_y);
        normaliseCoordinates(&cir_x, &cir_y);
        glVertex2f(cir_x, cir_y);
    }
    glEnd();

    free(dimensions);
}

void Render::drawRing(double x, double y, double radius, double* colour, double percentage) {
    x *= scale, y *= scale, radius *= scale * 0.5;
    float fSize = 1000, alpha = 1 / fSize * 6.28318530718;
    const float cosA = cos(alpha), sinA = sin(alpha);
    float curX = radius, curY = 0;
    for (size_t i = 0; i < fSize; ++i) {
        if (i >= fSize * percentage) break;
        float ncurX = cosA * curX - sinA * curY;
        curY = sinA * curX + cosA * curY;
        curX = ncurX;
        drawQuad(curX + (x + radius) + offsets[0], (curY + y + (radius / 2)) * (16.0 / 9) + offsets[1], 0.005 * scale, 0.005 * (16.0 / 9) * scale, colour);
    }
}

void Render::drawQuad(Moveable* q) {
    double* location = q->getLocation();
    double* size = q->getSize();
    glLoadIdentity();
    if (q->getFlags() & QUAD) {
        drawCustom(q->getPoints(), q->getGLColour(), q->getGLGradient());
        if (debug_) {
            if (debug_) drawQuad(location, size, new double[4]{ 0.6, 1.0, 0.8, 0.3 }, new double[4]{ 0.6, 1.0, 0.8, 0.3 });
        }
    } // bd
    else if (q->getFlags() & BACKGROUND) drawQuad(((location[0] - ((*objects_)[0]->getLocation()[0] * 0.03)) * scale) + offsets[0], (location[1] * scale) + offsets[1], size[0] * scale, size[1] * scale, q->getGLColour(), q->getGLGradient());
    else if (q->getFlags() & CURVED) {
        if (!(q->getFlags() & DISABLED)) {
            drawCurvedQuad(location[0] * scale, location[1] * scale, size[0] * scale, size[1] * scale, 0.025f, q->getGLColour(), q->getGLGradient());
        }
    }
    else drawQuad(location, size, q->getGLColour(), q->getGLGradient());
}

void Render::drawQuad(double* location, double* size, double* colour, double* gradient_colour) {
    drawQuad((location[0] * scale) + offsets[0], (location[1] * scale) + offsets[1], size[0] * scale, size[1] * scale, colour, gradient_colour);
    // drawCurvedQuad(location[0] * scale, location[1] * scale, size[0] * scale, size[1] * scale, 0.05f, colour, gradient_colour);
}

void Render::drawQuad(double x, double y, double width, double height, double* colour, double* gradient_colour) {
    if ((x + width < 0) || (y + height < 0) || (y > 1) || (x > 1)) return;
    normaliseCoordinates(&x, &y);
    width *= 2;
    height *= 2;
    y -= height;

    // glRotatef(30, 1, 0, 0);

    if (render_method == 0) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glBegin(GL_QUADS);
        glTranslatef(0, 0, 0);
        glRotatef(50, 0, 0, 1);
        colour == nullptr ? glColor4d(1.0, 1.0, 1.0, 1.0) : glColor4dv(colour);
        glVertex2d(x, y);
        glVertex2d(x, y + height);
        if (gradient_colour) glColor4dv(gradient_colour);
        glVertex2d(x + width, y + height);
        glVertex2d(x + width, y);
        glEnd();
        glPopMatrix();
    }

    else {
        colour = colour == nullptr ? new double[] {1.0, 1.0, 1.0, 1.0} : colour;
        GLfloat vertices[8] = { x, y,    x, y + height,   x + width, y + height,    x + width, y };
        GLfloat colours[16];

        for (int a = 0; a < 16; a += 4) for (int b = 0; b < 4; b++) colours[a + b] = colour[b];
        if (gradient_colour != nullptr) for (int x = 8; x < 14; x += 4) for (int y = 0; y < 4; y++) colours[x + y] = gradient_colour[y];

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, vertices);
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, colours);
        glDrawArrays(GL_QUADS, 0, 4);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

#define GLW_SMALL_ROUNDED_CORNER_SLICES 20 

typedef struct glwVec2 {
    float x;
    float y;
} glwVec2;

static glwVec2 glwRoundedCorners[GLW_SMALL_ROUNDED_CORNER_SLICES] = { {0} };

static void createRoundedCorners(glwVec2* arr, int num) {
    float slice = PI / 2 / num;
    int i;
    float a = 0;
    for (i = 0; i < num; a += slice, ++i) {
        arr[i].x = cosf(a);
        arr[i].y = sinf(a);
    }
}

void Render::drawCurvedQuad(double x, double y, float width, float height,
    float radius, double* colour, double* gradient_colour) {
    createRoundedCorners(glwRoundedCorners, GLW_SMALL_ROUNDED_CORNER_SLICES);

    if ((x + width < 0) || (y + height < 0) || (y > 1) || (x > 1)) return;
    normaliseCoordinates(&x, &y);
    width *= 2;
    height *= 2;
    y -= height;

    colour = colour ? colour : new double[] {1.0, 1.0, 1.0, 1.0};
    gradient_colour = gradient_colour ? gradient_colour : colour;

    float left = x, top = y, bottom = y + height, right = x + width;
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUAD_STRIP);
    // Draw left rounded side.
    for (int i = 0; i < GLW_SMALL_ROUNDED_CORNER_SLICES; ++i) {
        glColor4d(colour[0], colour[1], colour[2], colour[3]);
        glVertex2f(left + radius - radius * glwRoundedCorners[i].x,
            bottom - radius + radius * glwRoundedCorners[i].y);
        glColor4d(gradient_colour[0], gradient_colour[1], gradient_colour[2], gradient_colour[3]);
        glVertex2f(left + radius - radius * glwRoundedCorners[i].x,
            top + radius - radius * glwRoundedCorners[i].y);
    }
    // Draw right rounded side.
    for (int i = GLW_SMALL_ROUNDED_CORNER_SLICES - 1; i >= 0; --i) {
        glColor4d(colour[0], colour[1], colour[2], colour[3]);
        glVertex2f(right - radius + radius * glwRoundedCorners[i].x,
            bottom - radius + radius * glwRoundedCorners[i].y);
        glColor4d(gradient_colour[0], gradient_colour[1], gradient_colour[2], gradient_colour[3]);
        glVertex2f(right - radius + radius * glwRoundedCorners[i].x,
            top + radius - radius * glwRoundedCorners[i].y);
    }
    glEnd();
}

void Render::drawTexture(Moveable* m) {
    if (m->getFlags() & PLAYER) {
        Player* p = (Player*) m;
        drawTexture(m->getLocation(), m->getSize(), m->getTexture(), m->getGLColour(), p->facingRight());
        ang = p->rotation;
    } else if (m->getFlags() & ENEMY) {
        Enemy* e = (Enemy*)m;
        drawTexture(m->getLocation(), m->getSize(), m->getTexture(), m->getGLColour(), e->facingRight());
    } else {
        drawTexture(m->getLocation(), m->getSize(), m->getTexture(), m->getGLColour());
    }
}

void Render::drawTexture(double* location, double* size, Texture* texture, double* colour, bool right) {
    drawTexture((location[0] * scale) + offsets[0], (location[1] * scale) + offsets[1], size[0] * scale, size[1] * scale, texture, colour, right);
}

void Render::drawTexture(double x, double y, double width, double height, Texture* texture, double* colour, bool right) {
    if ((x + width < 0) || (y + height < 0) || (y > 1) || (x > 1)) return;
    normaliseCoordinates(&x, &y);
    width *= 2;
    height *= 2;
    y -= height;

    double x1 = x + ((width) / 2);
    double y1 = y + ((height) / 2);

    double e_x = 0.2;
    double e_y = 0.6;
    

    normaliseCoordinates(&e_x, &e_y);

    /*
    if (0 && width / 2 <= 0.12) { // bad, remove enemy
        glLoadIdentity();
        glTranslatef(x1, y1, 0.0);
        glScalef(1, 16 / 9.0, 1);
        glRotatef(ang, 0, 0, 1);
        glScalef(1, 9.0 / 16, 1);
        glTranslatef(-x1, -y1, 0.0);
        // ang++;
        if (ang == 359) {
            ang == 0;
        }
    }*/

    /* REMOVE FLIP
    if (1 && width / 2 <= 0.12) { // bad, remove enemy
        glLoadIdentity();
        glTranslatef(x1, y1, 0.0);
        glScalef(1, 16 / 9.0, 1);
        glRotatef(-(ang - 270), 0, 0, 1);
        glScalef(1, 9.0 / 16, 1);
        glTranslatef(-x1, -y1, 0.0);
        // ang++;
        /*
        if (ang == 359) {
            ang == 0;
        }*/
    //}

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture->data);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        // colour == nullptr ? glColor4d(1.0, 1.0, 1.0, 1.0) : glColor4dv(colour);
        // glTranslatef(x, y, 0);
        // glRotatef(0.5, 0, 0, 1);
        glColor4d(1.0, 1.0, 1.0, 0.61);
        if (right) {
            glTexCoord2i(0, 1); glVertex2d(x, y);
            glTexCoord2i(0, 0); glVertex2d(x, y + height);
            glTexCoord2i(1, 0); glVertex2d(x + width, y + height);
            glTexCoord2i(1, 1); glVertex2d(x + width, y);
        }
        else {
            glTexCoord2i(1, 1); glVertex2d(x, y);
            glTexCoord2i(1, 0); glVertex2d(x, y + height);
            glTexCoord2i(0, 0); glVertex2d(x + width, y + height);
            glTexCoord2i(0, 1); glVertex2d(x + width, y);
        }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
}

void Render::drawCustom(vector<map<double, double>> points, double* colour, double* gradient_colour) {
    // Don't render offscreen.
    glBegin(GL_QUADS);
        colour == nullptr ? glColor4d(1.0, 1.0, 1.0, 1.0) : glColor4dv(colour); // Stuck with order.
    
        for (int x = 0; x < 4; x++) {
            points[x][0] = (points[x][0] * scale) + offsets[0], points[x][1] = (points[x][1] * scale) + offsets[1];
            normaliseCoordinates(&points[x][0], &points[x][1]);
            glVertex2d(points[x][0], points[x][1]);
            if (x == 1 && gradient_colour) glColor4dv(gradient_colour);
        }
    glEnd();
}

void Render::normaliseCoordinates(double* x, double* y) {
    *x = *x < 0.5 ? -1 + (*x * 2) : *x > 0.5 ? ((*x - 0.5) * 2) : 0;
    *y = *y < 0.5 ? (-1 + (*y * 2)) * -1 : *y > 0.5 ? ((*y - 0.5) * 2) * -1 : 0;
}

void Render::toggleFullscreen() {
    int* dimensions = retrieveDimensions();
    fullscreen_ = !fullscreen_;
    glfwSetWindowMonitor(window_, fullscreen_ ? glfwGetPrimaryMonitor() : NULL, 100, 100, dimensions[0], dimensions[1], GLFW_DONT_CARE);
    renderWindow();
    delete dimensions;
}

int* Render::retrieveDimensions() {
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    glViewport(0, 0, width, height);
    return new int[] { int(width), int(height) };
}

void Render::renderWindow() {
    glClear(GL_COLOR_BUFFER_BIT);
    int* resolution = retrieveDimensions();

    double c_quad = glfwGetTime();
    for (Moveable* m : *objects_) {
        Moveable* mv = m;
        if (m->getFlags() & CIRCLE) {
            drawCircle((Circle*)m);
        }
        else if (m->getFlags() & TEXTURED) drawTexture(m);
        else drawQuad(m);
    }
    draw_times[0] = glfwGetTime() - c_quad;

    double c_text = glfwGetTime();
    for (Text* t : *text_objects_) drawText(t);
    draw_times[1] = glfwGetTime() - c_text;

    glfwSwapBuffers(window_);
    delete resolution;
}
#endif