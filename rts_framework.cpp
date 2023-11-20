#include <string>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

#include "defs/colour.h"
#include "defs/province.h"
#include "defs/unit.h"
#include "defs/nation.h"
#include "defs/object.h"

#include "include/GLFW/glfw3.h"

#define MAX_RESTRICTED_GAME_SPEED 2000

static int ProvinceCount = 0;
static int NationCount = 0;

Province* createProvince(string name) { return new Province(ProvinceCount++, name); }
Nation* createNation(string name) { return new Nation(NationCount++, name); }

vector<Nation*> nations;
GLFWwindow* window;


void draw_quad(double x, double y, double width, double height, Vector3 colour) {
    glColor3d(colour.getX(), colour.getY(), colour.getZ());
    glBegin(GL_QUADS);

    x /= 1280, y /= 720;

    // Convert coordinates
    x = x < 0.5 ? -1 + (x * 2) : x > 0.5 ? ((x - 0.5) * 2) : 0;
    y = y < 0.5 ? (-1 + (y * 2)) * -1 : y > 0.5 ? ((y - 0.5) * 2) * -1 : 0;

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

int main() {
    Province* prov_paris = createProvince("Paris");
    prov_paris->setLocation(100, 100);
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

    if (!(window = glfwCreateWindow(1280, 720, "RTS", NULL, NULL))) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glClearColor(0.155, 0.155, 0.155, 0);

    render_window();

    while (true) {
        glfwPollEvents();
        render_window();
        cout << "* Day #" << elapsed_days << endl;
        for (Nation* nation : nations) {
            nation->evaluate();
            cout << nation->getName() << "\t" << nation->getCapital()->getName() << " (" << nation->getNumberProvinces() << ")\tMoney: " << nation->getMoney() << " [" << nation->getIncome() << "]\t";
            for (Province* province : nation->getOwnedProvinces()) {
                cout << province->getName() << ", ";
            } cout << endl;
            for (Unit* unit : nation->getOwnedUnits()) {
                string target_name = unit->getTarget() ? unit->getTarget()->getName() : "None";
                cout << "Unit #" << unit->getID() << "\tSize: " << unit->getSize() << "\t Location : " << unit->getX() << ", " << unit->getY() << "\tProvince : " << unit->getProvince()->getName() << "\t\tTarget: " << target_name << endl;
            } cout << endl;
        } cout << endl;
        elapsed_days++;
        if (elapsed_days == 7) {
            nation_germany->removeProvince(prov_berlin);
        }
        if (game_speed > 0) this_thread::sleep_for(chrono::milliseconds((int)(MAX_RESTRICTED_GAME_SPEED / game_speed)));
    }

    delete prov_paris;
    delete prov_berlin;
    delete prov_hamburg;
    delete nation_france;
    delete nation_germany;
    // + Units
    // TODO: Use unique ptrs
}
