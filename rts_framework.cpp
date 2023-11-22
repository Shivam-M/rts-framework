#include <string>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;

#include "defs/colour.h"
#include "defs/province.h"
#include "defs/unit.h"
#include "defs/nation.h"
#include "defs/object.h"

#include "include/GLFW/glfw3.h"

#define MAX_RESTRICTED_GAME_SPEED 2000
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define PRINT_DEBUG_

static int ProvinceCount = 0;
static int NationCount = 0;

Province* createProvince(string name) { return new Province(ProvinceCount++, name); }
Nation* createNation(string name) { return new Nation(NationCount++, name); }

vector<Nation*> nations;
GLFWwindow* window;

Unit* selected_unit; Province* selected_province;


void draw_quad(double x, double y, double width, double height, Vector3 colour) {
    glColor3d(colour.getX(), colour.getY(), colour.getZ());
    glBegin(GL_QUADS);

    // Convert coordinates from [0-1] where (0,0) is the top-left into OpenGL -1 to 1 coords
    x /= 1280, y /= 720;
    width *= 2, height *= 2;

    x = (2.0f * x - 1.0f);
    y = (1.0f - 2.0f * y) - height;

    glVertex2d(x, y);
    glVertex2d(x, y + height);
    glVertex2d(x + width, y + height);
    glVertex2d(x + width, y);

    glEnd();
}

void render_window() {
    glClear(GL_COLOR_BUFFER_BIT);
    int location[2];
    for (Nation* nation : nations) {
        for (Province* province : nation->getOwnedProvinces()) {
            province->getLocation(&location[0], &location[1]);
            draw_quad(location[0], location[1], 0.05, 0.05, province->getColour().getRGBf());
        }
        for (Unit* unit : nation->getOwnedUnits()) {
            draw_quad(unit->getX(), unit->getY(), 0.05, 0.05, nation->getColour().getRGBf());
        }
    }
    glfwSwapBuffers(window);
}

bool is_within(double x, double y, double width, double height, double px, double py) {
    return px >= x && px <= x + width && py >= y && py <= y + height;
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    double x, y, relx, rely;
    glfwGetCursorPos(window, &x, &y);
    relx = x / WINDOW_WIDTH, rely = y / WINDOW_HEIGHT;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        for (Nation* nation : nations) {
            for (Unit* unit : nation->getOwnedUnits()) {
                if (is_within(unit->getX() / WINDOW_WIDTH, unit->getY() / WINDOW_HEIGHT, 0.05, 0.05, relx, rely)) {
                    cout << "Selected UNIT ID " << unit->getID() << endl;
                }
            }
            for (Province* province : nation->getOwnedProvinces()) {
                int location[2];
                province->getLocation(&location[0], &location[1]);

                if (is_within(location[0] / 1280.0, location[1] / 720.0, 0.05, 0.05, relx, rely)) {
                    cout << "Selected Province ID " << province->getID() << endl;
                    selected_province = province;
                }
            }
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (selected_province) {
            selected_province->setLocation(x, y);
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {}

int main() {
    Province* prov_paris = createProvince("Paris");
    prov_paris->setLocation(700, 100);
    prov_paris->setValue(0.42);

    Nation* nation_france = createNation("France");
    nation_france->setColour(Colour(0, 0, 255));
    nation_france->setMoney(10000);
    nation_france->addProvince(prov_paris);
    nation_france->setCapital(prov_paris);
    nation_france->hireUnit(1000);
    
    Province* prov_berlin = createProvince("Berlin");
    Province* prov_hamburg = createProvince("Hamburg");
    prov_berlin->setLocation(400, 100);
    prov_berlin->setValue(0.35);
    prov_hamburg->setLocation(400, 200);
    prov_hamburg->setValue(0.20);

    Nation* nation_germany = createNation("Germany");
    nation_germany->setColour(Colour(255, 0, 255));
    nation_germany->setMoney(7800);
    nation_germany->addProvince(prov_hamburg);
    nation_germany->addProvince(prov_berlin);
    nation_germany->setCapital(prov_berlin);
    nation_germany->hireUnit(2250);
    
    nation_germany->getOwnedUnits()[0]->setProvince(prov_hamburg);
    nation_germany->getOwnedUnits()[0]->setTarget(prov_paris);
    nation_germany->getOwnedUnits()[0]->initiateBattle(nation_france->getOwnedUnits()[0]);

    nations.push_back(nation_france);
    nations.push_back(nation_germany);

    double game_speed = 5;
    int elapsed_days = 0;

    if (!glfwInit()) return -1;
    if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RTS", NULL, NULL))) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glClearColor(0.155, 0.155, 0.155, 0);

    int update_rate = game_speed;
    int frames = 0, updates = 0, fps_limit = 0;
    double limit = 1.0 / game_speed;
    double last_time = glfwGetTime(), timer = last_time, last_frame_time = glfwGetTime();;
    double delta_time = 0, current_time = 0;

    while (!glfwWindowShouldClose(window)) {
        current_time = glfwGetTime();
        limit = 1.0 / update_rate;
        delta_time += (current_time - last_time) / limit;
        last_time = current_time;

        while (delta_time >= 1.0) {
#ifdef PRINT_DEBUG
            cout << "* Day #" << elapsed_days << endl;
            for (Nation* nation : nations) {
                nation->evaluate();
                cout << nation->getName() << "\t" << nation->getCapital()->getName() << " (" << nation->getNumberProvinces() << ")\tMoney: " << nation->getMoney() << " [" << nation->getIncome() << "]\t";
                for (Province* province : nation->getOwnedProvinces()) {
                    cout << province->getName() << ", ";
                } cout << endl;
                for (Unit* unit : nation->getOwnedUnits()) {
                    string target_name = unit->getTarget() ? unit->getTarget()->getName() : "None";
                    cout << "Unit #" << unit->getID() << "\tSize: " << unit->getSize() << "\t Location: " << unit->getX() << ", " << unit->getY() << "\tProvince: " << unit->getProvince()->getName() << "\t\tTarget: " << target_name << endl;
                } cout << endl;
            } cout << endl;
#else
            for (Nation* nation : nations) nation->evaluate();
#endif
            elapsed_days++;
            if (elapsed_days == 7) {
                nation_germany->removeProvince(prov_berlin);
            }

            glfwPollEvents();
            updates++;
            delta_time--;
        }

        if (fps_limit == 0 || glfwGetTime() - last_frame_time >= (1.0 / fps_limit)) {
            last_frame_time = glfwGetTime();
            render_window();
            frames++;
        }

        if (glfwGetTime() - timer > 1.0) {
            timer++;
            updates = 0, frames = 0;
        }
    }

    delete prov_paris;
    delete prov_berlin;
    delete prov_hamburg;
    delete nation_france;
    delete nation_germany;
    // + Units
    // TODO: Use unique ptrs
}
