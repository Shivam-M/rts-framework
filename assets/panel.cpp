#include "panel.h"

#include "../assets/text.h"

using namespace std;

Panel::Panel() {
    add_flag(PANEL);
}

void Panel::show(bool with_fade) {
    if (!has_flag(DISABLED)) return;
    remove_flag(DISABLED);
    for (Moveable* moveable : bundle_)
        moveable->remove_flag(DISABLED); // Move with fade() to prevent having to cycle twice
    if (with_fade) fade();
}

void Panel::hide(bool with_fade) {
    if (has_flag(DISABLED)) return;
    add_flag(DISABLED);
    if (with_fade) fade_out();
    else {
        for (Moveable* moveable : bundle_) {
            moveable->add_flag(DISABLED);
        }
    }
}

void Panel::add(Moveable* moveable) {
    moveable->add_flag(UNDER_PANEL);
    moveable->add_flag(UI);
    moveable->size *= size;
    moveable->location = location + (moveable->location * size);
    moveable->parent = this;
    bundle_.push_back(moveable);
}

void Panel::remove(Moveable* moveable) {
    bundle_.erase(std::remove(bundle_.begin(), bundle_.end(), moveable), bundle_.end());
    moveable->parent = nullptr;
}

void Panel::clear() {
    bundle_.clear();
}

vector<Moveable*>* Panel::get() {
    return &bundle_;
}

Moveable* Panel::get_moveable_by_name(const string& name) {
    for (Moveable* moveable : bundle_) {
        if (moveable->name == name) {
            return moveable;
        }
    }
    return nullptr;
}

Text* Panel::get_text_by_name(const string& name) {
    return static_cast<Text*>(get_moveable_by_name(name));
}

void Panel::fade() {
    for (Moveable* mv : bundle_) {
        ColourShift entrance = ColourShift(mv->colour.with_alpha(0), mv->colour);
        entrance.speed = 0.0075f;
        mv->colour.a = 0;
        entrance.loop = false;
        // entrance.reswap();
        entrance.direction = ColourShift::Direction::Up;
        mv->set_colour_shift(entrance);
    }
}

void Panel::fade_out() {
    for (Moveable* mv : bundle_) {
        // mv->set_colour(mv->get_default_colour());
        Colour original_colour = mv->colour;
        Colour transparent_colour = original_colour;
        transparent_colour.a = 0;
        ColourShift entrance = ColourShift(mv->colour, mv->colour.with_alpha(0));
        entrance.direction = ColourShift::Direction::Up;
        entrance.speed = 0.0075f;
        entrance.loop = false;
        entrance.fade_to_death = true;
        mv->set_colour_shift(entrance);
    }
}

void Panel::set_location(float x, float y) {
    for (Moveable* moveable : bundle_) {
        Vector2 new_location = moveable->location;
        new_location.x += -location.x + x;
        new_location.y += -location.y + y;
        moveable->set_location(new_location.x, new_location.y);
    }
    Moveable::set_location(x, y);
}

void Panel::set_size(float x, float y) {
    Vector2 updated_size = { x, y };
    Vector2 current_size = size;
    if (updated_size.x == 0 || updated_size.y == 0) return;
    for (Moveable* moveable : bundle_) {
        Vector2 original_size = moveable->size / current_size;
        Vector2 original_location = (moveable->location - location) / current_size;
        moveable->size = original_size * updated_size;
        moveable->location = location + (original_location * updated_size);
    }
    Moveable::set_size(x, y);
}
