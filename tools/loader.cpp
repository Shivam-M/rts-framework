#include "loader.h"
#include "../defs/colour.h"
#include "../defs/province_mv.h"
#include "../defs/unit.h"
#include "../defs/nation.h"

json Loader::ldata;

const json DEFAULTS = {
	{"colour",			"FFFFFF"},
	{"alpha",			1},
	{"alt_colour",		"FFFFFF"},
	{"alt_alpha",		1.0},
	{"level_name",      "LEVEL"},
	{"width",			1},
	{"height",			1},
	{"x",				0},
	{"y",				0},
	{"name",			"Generic Object"},
	{"velocity",		0},
	{"acceleration",	0},
	{"script",			""},
	{"font",			"arial.ttf"},
	{"font_custom",		0},
	{"size",			10},
	{"content",			"Sample Text"},
	{"variables",		nullptr},
	{"image",			nullptr},
	{"value",			0.10},
	{"id",				-1},
	{"provinces",		json::array()},
	{"units",			json::array()},
	{"neighbours",		json::array()}
};


static map<int, ProvinceMV*> province_map;
static map<int, Nation*> nation_map;
static map<int, Unit*> unit_map;


void Loader::parseCommon(json properties, Moveable* moveable) {
	short* colour = rgb(getString(properties, "colour"));
	short* alt_colour = rgb(getString(properties, "alt_colour"));
	string script = getString(properties, "script");

	moveable->setSize(getDouble(properties, "width"), getDouble(properties, "height"));
	moveable->setLocation(getDouble(properties, "x"), getDouble(properties, "y"));
	moveable->setColour(colour[0], colour[1], colour[2], (short)(getDouble(properties, "alpha") * 255));
	moveable->setGradientColour(alt_colour[0], alt_colour[1], alt_colour[2], (short)(getDouble(properties, "alt_alpha") * 255));

	if (script != "") moveable->loadScript(script);

	delete colour, alt_colour;
}

Fire* Loader::parseFire(json properties) {
	Fire* fire = new Fire();
	parseCommon(properties, fire);
	return fire;
}

Stars* Loader::parseStars(json properties) {
	Stars* stars = new Stars();
	parseCommon(properties, stars);
	return stars;
}

Collidable* Loader::parseCollidable(json properties) {
	Collidable* collidable = new Collidable();
	parseCommon(properties, collidable);
	collidable->setName(getString(properties, "name", "Default Collidable"));
	return collidable;
}

Moveable* Loader::parseMoveable(json properties) {
	Moveable* moveable = new Moveable();
	parseCommon(properties, moveable);
	return moveable;
}

Collidable* Loader::parseQuad(json properties) {
	Collidable* quad = new Collidable();
	vector<map<double, double>> points = { {}, {}, {}, {} };
	int c, d = 0;
	for (auto& points_data : properties["points"].items()) {
		c = 0;
		map<double, double> point = {};
		for (double point : points_data.value()) {
			points[c][d] = point;
			c++;
		} d++;
	}
	quad->addFlag(QUAD);
	// quad->addFlag(GHOST);
	quad->setPoints(points[0], points[1], points[2], points[3]);
	quad->setSize(abs(points[0][0] - points[1][0]), abs(points[0][1] - points[1][1]));
	// quad->setTexture()
	if (getString(properties, "texture", "NULL") != "NULL") {
		quad->setTexture(Image::getImage(getString(properties, "texture", "NULL")));
	}
	parseCommon(properties, quad);
	return quad;
}

Square* Loader::parseSquare(json properties) {
	// Square* square = new Square();
	return nullptr;
}

Circle* Loader::parseCircle(json properties) {
	Circle* circle = new Circle();
	parseCommon(properties, circle);
	return circle;
}

string Loader::getVariable(string var) {
	auto target_value = ldata.find("variables");
	if (target_value == ldata.end()) return "";
	json vars = ldata["variables"];
	return vars[var];
}

// replace with macro
Text* Loader::parseText(json properties) {
	Text* text = new Text();
	short* colour = rgb(getString(properties, "colour"));
	text->setColour(colour[0], colour[1], colour[2], getDouble(properties, "alpha") * 255);
	text->setContent(getString(properties, "content"));
	if (getInt(properties, "font_custom")) {
		text->setFont(Fonts::getFont(getString(properties, "font"), getInt(properties, "size"), true));
	} else {
		text->setFont(Fonts::getFont(getString(properties, "font"), getInt(properties, "size")));
	}
	text->setLocation(getDouble(properties, "x"), getDouble(properties, "y"));
	text->fixed = false;
	return text;
}

Collidable* Loader::parseSlider(json properties) {
	Collidable* slider = new Collidable();
	parseCommon(properties, slider);
	slider->addFlag(MOVING);
	return slider;
}

map<int, ProvinceMV*> Loader::getProvinceMap() {
	return province_map;
}

map<int, Nation*> Loader::getNationMap() {
	return nation_map;
}

map<int, Unit*> Loader::getUnitMap() {
	return unit_map;
}

ProvinceMV* Loader::parseProvince(json properties) {
	ProvinceMV* province = new ProvinceMV(getInt(properties, "id"), getString(properties, "name"));
	parseCommon(properties, province);
	province->addFlag(TEXTURED); // move to common
	
	font_data font = Fonts::getFont(CONSOLAS_BOLD, 8);
	province->setValue(getDouble(properties, "value"));
	province->setText(new Text(province->Moveable::getLocation()[0], province->Moveable::getLocation()[1], font, 189, 195, 199, 150, province->getName()));
	province->setBounds(province->getSize()[0] / 4, province->getSize()[1] / 4);
	province_map[province->getID()] = province;

	province->setTexture(Image::getImage("data/generated/" + to_string( province->getID()) + "_province.png"));

	if (province->getTexture()->image == nullptr) {
		if (getString(properties, "texture", "NULL") != "NULL") province->setTexture(Image::getImage(getString(properties, "texture", "NULL")));
	}

	return province;
}

Unit* Loader::parseUnit(json properties) {
	Unit* unit = new Unit(getInt(properties, "id"), nullptr, getInt(properties, "size"), getDouble(properties, "skill"), province_map[getInt(properties, "province")]);
	parseCommon(properties, unit);
	// offset x location by -1
	double* loc = unit->Moveable::getLocation();
	unit->Moveable::setLocation(loc[0] - 1, loc[1]);
	unit->Moveable::setName(getString(properties, "name"));

	font_data font = Fonts::getFont(CONSOLAS_BOLD, 8);
	unit->setText(new Text(unit->Moveable::getLocation()[0], unit->Moveable::getLocation()[1], font, 189, 195, 199, 250, unit->getName()));
	unit->setBounds(0, -0.0025);

	unit->addFlag(UNIT);


	unit_map[getInt(properties, "id")] = unit;
	return unit;
}

Nation* Loader::parseNation(json properties) {
	Nation* nation = new Nation(getInt(properties, "id"), getString(properties, "name"), province_map[getInt(properties, "capital")]);

	for (const auto& element : getArray(properties, "provinces")) {
		ProvinceMV* province = province_map[(int)element];
		if (province == nullptr) continue;

		short* colour = rgb(getString(properties, "colour"));
		nation->Moveable::setColour(colour[0], colour[1], colour[2], (short)(getDouble(properties, "alpha") * 255));

		nation->addProvince(province);
		province->Moveable::setColour(nation->Moveable::getColour());

		stringstream ss;
		info(ss << "Assigned province '"  << province->getName() << "' (" << province->getID() << ") to nation '" << nation->getName() << "' (" << nation->getID() << ")");
	}

	for (const auto& element : getArray(properties, "units")) {
		Unit* unit =  unit_map[(int)element];
		if (unit == nullptr) continue;
		nation->addUnit(unit);
		unit->Moveable::setColour(nation->Moveable::getColour());
		stringstream ss;
		info(ss << "Assigned unit '" << unit->getName() << "' (" << unit->getID() << ") to nation '" << nation->getName() << "' (" << nation->getID() << ")");
	}

	nation_map[getInt(properties, "id")] = nation;

	return nation;
}

string Loader::getString(json properties, string key, string def) {
	if (properties.find(key) != properties.end()) {
		string val = properties[key];
		if (val[0] == 38) { // '&'
			val.erase(0, 1);
			return getVariable(val);
		} return properties[key];
	} else {
		if (def.empty()) {
			return DEFAULTS[key];
		} return def;
	}
}

int Loader::getInt(json properties, string key) {
	return properties.find(key) != properties.end() ? properties[key] : DEFAULTS[key];
}

double Loader::getDouble(json properties, string key) {
	return properties.find(key) != properties.end() ? properties[key] : DEFAULTS[key];
}

json::array_t Loader::getArray(json properties, string key) {
	auto target_value = properties.find(key);
	return target_value != properties.end() && target_value.value().is_array() ? properties[key] : DEFAULTS[key];
}


Level* Loader::load(string f, vector <Moveable*>* q, vector<Text*>* t, int identifier) {
	Level* level = new Level();
	ifstream level_file(f);
	json level_data = json::parse(level_file);
	stringstream ss;

	info(ss << "Loading level... " << f << " [" << getString(level_data, "level_name") << "]");

	Moveable* mv_background = new Moveable();
	short* bg_colour = rgb(level_data["background"]);

	mv_background->setColour(bg_colour[0], bg_colour[1], bg_colour[2], (short)((double)level_data["background_alpha"] * 255));
	mv_background->setSize(1.0, 1.0);
	mv_background->setLocation(0.0, 0.0);
	mv_background->addFlag(UNEDITABLE);
	mv_background->setName("Background " + to_string(identifier));

	auto target_value = level_data.find("image");
	if (target_value != level_data.end()) {
		mv_background->addFlag(TEXTURED);
		mv_background->setTexture(Image::getImage(level_data["image"]));
	}

	if (!(level_data["background"] == level_data["alt_background"] || level_data["alt_background"] == "")) {
		short* gr_colour = rgb(level_data["alt_background"]);
		mv_background->setGradientColour(gr_colour[0], gr_colour[1], gr_colour[2], (short)((double)level_data["alt_background_alpha"] * 255));
		free(gr_colour);
	}

	level->objects.push_back(mv_background);
	ldata = level_data;

	for (auto& el : level_data["objects"].items()) {
		json object_data = el.value();
		string type = object_data["type"];

		if (type == "COLLIDABLE") {
			level->objects.push_back(parseCollidable(object_data));
		} else if (type == "QUAD") {
			level->objects.push_back(parseQuad(object_data));
		} else if (type == "MOVEABLE") {
			level->objects.push_back(parseMoveable(object_data));
		} else if (type == "CIRCLE") {
			level->objects.push_back(parseCircle(object_data));
		} else if (type == "SQUARE") {
			level->objects.push_back(parseSquare(object_data));
		} else if (type == "TEXT") {
			level->text_objects.push_back(parseText(object_data));
		} else if (type == "SLIDER") {
			level->objects.push_back(parseSlider(object_data));
		} else if (type == "PROVINCE") {
			ProvinceMV* province = parseProvince(object_data);
			province->addFlag(PROVINCE);
			level->objects.push_back(province);
// #define PROVINCE_DEBUG_TEXT
#ifdef PROVINCE_DEBUG_TEXT
			level->text_objects.push_back(province->getText());
#endif
		} else if (type == "UNIT") {
			Unit* unit = parseUnit(object_data);
			level->objects.push_back(unit);
			level->text_objects.push_back(unit->getText());
		} else if (type == "NATION") {
			Nation* nation = parseNation(object_data);
			level->objects.push_back(nation);
		} else if (type == "FIRE") {
			Fire* fire_object = parseFire(object_data);
			fire_object->objects = q;
			level->objects.push_back(fire_object);
		} else if (type == "STARS") {
			Stars* stars_object = parseStars(object_data);
			stars_object->objects = q;
			level->objects.push_back(stars_object);
		} else {
			level->objects.push_back(parseMoveable(object_data));
		}
	}

	info(ss << "Finished parsing " << level->objects.size() << " object(s).");
	delete bg_colour;
	return level;
}
