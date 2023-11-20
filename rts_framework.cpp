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

#define MAX_RESTRICTED_GAME_SPEED 2000

static int ProvinceCount = 0;
static int NationCount = 0;

Province* createProvince(string name) { return new Province(ProvinceCount++, name); }
Nation* createNation(string name) { return new Nation(NationCount++, name); }


int main() {
    vector<Nation*> nations;
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
    
    nation_germany->getOwnedUnits()[0]->setTarget(prov_paris);
    nation_germany->getOwnedUnits()[0]->initiateBattle(nation_france->getOwnedUnits()[0]);

    nations.push_back(nation_france);
    nations.push_back(nation_germany);

    double game_speed = 2.5;
    int elapsed_days = 0;

    while (true) {
        cout << "* Day #" << elapsed_days << endl;
        for (Nation* nation : nations) {
            nation->evaluate();
            cout << nation->getName() << "    " << nation->getCapital()->getName() << " (" << nation->getNumberProvinces() << ")    Money: " << nation->getMoney() << " [" << nation->getIncome() << "]    ";
            for (Province* province : nation->getOwnedProvinces()) {
                cout << province->getName() << ", ";
            } cout << endl;
            for (Unit* unit : nation->getOwnedUnits()) {
                string target_name = unit->getTarget() ? unit->getTarget()->getName() : "None";
                cout << "Unit #" << unit->getID() << "    Size: " << unit->getSize() << "     Location : " << unit->getX() << ", " << unit->getY() << "    Province : " << unit->getProvince()->getName() << "        Target: " << target_name << endl;
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
