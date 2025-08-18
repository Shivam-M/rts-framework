#include <iostream>
#include <vector>

using namespace std;

struct Colour {
    float x, y, z, w;

    Colour(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    Colour& operator+=(const Colour& other) {
        x += other.x; y += other.y; z += other.z; w += other.w;
        return *this;
    }

    Colour& operator*=(const Colour& other) {
        x *= other.x; y *= other.y; z *= other.z; w *= other.w;
        return *this;
    }
};

struct ColourFilter {
    enum class Mode { Addition, Multiplication };
    Mode mode;
    Colour colour;

    ColourFilter(Colour c, Mode m) : colour(c), mode(m) {}
};

int main() {
    Colour colour(100, 100, 100, 100);
    vector<ColourFilter*> filters;

    filters.emplace_back(new ColourFilter(Colour(10, 20, 30, 40), ColourFilter::Mode::Addition));
    filters.emplace_back(new ColourFilter(Colour(-40, -30, -20, -10), ColourFilter::Mode::Addition));
    filters.emplace_back(new ColourFilter(Colour(2, 2, 2, 2), ColourFilter::Mode::Multiplication));
    // filters.emplace_back(new ColourFilter(Colour(0.5, 0.5, 0.5, 0.5), ColourFilter::Mode::Multiplication));

    for (ColourFilter* filter : filters) {
        switch (filter->mode) {
            case ColourFilter::Mode::Addition:
                colour += filter->colour;
                break;
            case ColourFilter::Mode::Multiplication:
                colour *= filter->colour;
                break;
        }
    }
    
    // printf("%f, %f, %f, %f\n", colour.x_, colour.y_, colour.z_, colour.w_);
    
    cout << colour.x << ", " << colour.y << ", " << colour.z << ", " << colour.w << endl;
    
    return 0;
}
