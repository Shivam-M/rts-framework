#include "image.h"

#include "../tools/common.h"

#ifndef _WIN32
#define sprintf_s snprintf
#endif

#define __STDC_LIB_EXT1__

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <fstream>

using namespace std;

map<string, Texture*> Image::images = {};

struct Rectangle { int left, top, right, bottom; };

void cropRectangle(const unsigned char* image, int width, int height, unsigned char target_colour[3], Rectangle& rect) {
    rect.left = width;
    rect.top = height;
    rect.right = 0;
    rect.bottom = 0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int i = (y * width + x) * 3;
            if (image[i] == target_colour[0] && image[i + 1] == target_colour[1] && image[i + 2] == target_colour[2]) {
                rect.left = min(rect.left, x);
                rect.top = min(rect.top, y);
                rect.right = max(rect.right, x);
                rect.bottom = max(rect.bottom, y);
            }
        }
    }
}

void Image::load_map(const string& path, const string& data) { // add cache
    int width, height, channels;
    unsigned char* image = stbi_load(path.c_str(), &width, &height, &channels, 3);

    if (image == nullptr) {
        log_t("Error loading map image file.");
        return;
    }

    FILE* file;
    FOPEN(file, data.c_str(), "r");

    if (file == nullptr) {
        log_t("Error loading map data file.");
        return;
    }

    ofstream output_file("data/generated/province_dimensions.txt", ios::trunc);
    if (!output_file.is_open()) {
        log_t("Error loading province data output file.");
        return;
    }

    int id, r, g, b;
    while (FSCANF(file, "%d,%d,%d,%d", &id, &r, &g, &b) == 4) {
        log_t("Extracting map image for province ID... #", id);
        unsigned char target_colour[3] = {
            static_cast<unsigned char>(r),
            static_cast<unsigned char>(g),
            static_cast<unsigned char>(b)
        };
        Rectangle rect;
        cropRectangle(image, width, height, target_colour, rect);

        int output_width = rect.right - rect.left + 1;
        int output_height = rect.bottom - rect.top + 1;
        unsigned char* output_image = new unsigned char[output_width * output_height * 4];

        float relx = rect.left / (float)width;
        float rely = rect.top / (float)height;
        float relw = (rect.right - rect.left) / (float)width;
        float relh = (rect.bottom - rect.top) / (float)height;

        relx *= 2.0f;
        rely *= 2.0f;
        relw *= 2.0f;
        relh *= 2.0f;
        
        relx -= 0.5f;
        rely -= 0.90f;

        output_file << to_string(id) << "," << to_string(relx) << "," << to_string(rely) << "," << to_string(relw) << "," << to_string(relh) << endl;

        for (int y = rect.top, yy = 0; y <= rect.bottom; ++y, ++yy) {
            for (int x = rect.left, xx = 0; x <= rect.right; ++x, ++xx) {
                int index = (y * width + x) * 3;
                int index_output = (yy * output_width + xx) * 4;
                if (index_output >= 0 &&(index_output < (output_width * output_height * 4 ) - 1)) {
                    if (image[index] == target_colour[0] && image[index + 1] == target_colour[1] && image[index + 2] == target_colour[2]) {
                        output_image[index_output] = 255;
                        output_image[index_output + 1] = 255;
                        output_image[index_output + 2] = 255;
                        output_image[index_output + 3] = 255;
                    } else {
                        output_image[index_output] = 0;
                        output_image[index_output + 1] = 0;
                        output_image[index_output + 2] = 0;
                        output_image[index_output + 3] = 0;
                    }
                }
            }
        }
        string filename = "data/generated/" + to_string(id) + "_province.png";
        stbi_write_png(filename.c_str(), output_width, output_height, 4, output_image, output_width * 4);
        delete[] output_image;
    }
    fclose(file);
    stbi_image_free(image);
    output_file.close();
}

void Image::load_image(const string& path) {
    if (images.count(path)) return;
    log_t("Loading image... " CON_RED + path + CON_NORMAL);
    Texture* texture = new Texture();

    glGenTextures(1, &texture->data);
    texture->image = stbi_load(path.c_str(), &texture->width, &texture->height, &texture->composition, STBI_rgb_alpha);
    texture->path = path;

    glBindTexture(GL_TEXTURE_2D, texture->data);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->image);
    glBindTexture(GL_TEXTURE_2D, 0);
    images[path] = texture;
}

Texture* Image::get_image(const string& path) {
	load_image(path);
    return images[path];
}

void Image::cleanup() {
    for (auto& [path, texture] : Image::images) {
        delete texture;
    }
}