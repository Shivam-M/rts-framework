#include "loader.h"

#include "../defs/province.h"
#include "../defs/unit.h"
#include "../defs/nation.h"

#include "../assets/text.h"
#include "../assets/collidable.h"
#include "../shapes/circle.h"

json Loader::level_data;

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
	{"texture",			"NULL"},
	{"variables",		nullptr},
	{"image",			nullptr},
	{"value",			0.10},
	{"id",				-1},
	{"provinces",		json::array()},
	{"units",			json::array()},
	{"neighbours",		json::array()}
};

static map<int, Province*> province_map;
static map<int, Nation*> nation_map;
static map<int, Unit*> unit_map;

static json properties;

void Loader::parseCommon(Moveable* moveable) {
	string script = getString("script");
	if (script != "") moveable->loadScript(script);
	moveable->setName(getString("name"));
	moveable->setSize(getFloat("width"), getFloat("height"));
	moveable->setLocation(getFloat("x"), getFloat("y"));
	moveable->setColour(Colour::HexToRGB(getString("colour"), (getFloat("alpha"))));
	if (getString("alt_colour") != "") {
	 	moveable->setGradientColour(Colour::HexToRGB(getString("alt_colour"), (getFloat("alt_alpha"))));
	}
}

/* Fire* Loader::parseFire() { return nullptr; }

Stars* Loader::parseStars() { return nullptr; }

Square* Loader::parseSquare() { return nullptr;} */

Circle* Loader::parseCircle() {
	Circle* circle = new Circle();
	parseCommon(circle);
	return circle;
}

Collidable* Loader::parseCollidable() {
	Collidable* collidable = new Collidable();
	parseCommon(collidable);
	return collidable;
}

Moveable* Loader::parseMoveable() {
	Moveable* moveable = new Moveable();
	parseCommon(moveable);
	return moveable;
}

Collidable* Loader::parseCustom() {
	Collidable* quad = new Collidable();
	parseCommon(quad);

	/* vector<Vector2> points;
	int c, d = 0;
	for (auto& points_data : properties["points"].items()) {
		c = 0;
		map<float, float> point = {};
		for (float point : points_data.value()) {
			points[c][d] = point;
			c++;
		} d++;
	} */

	quad->addFlag(CUSTOM);
	// quad->setPoints(points[0], points[1], points[2], points[3]);
	// quad->setSize(abs(points[0][0] - points[1][0]), abs(points[0][1] - points[1][1])); // Temp
	if (getString("texture") != "NULL") {
		quad->setTexture(Image::getImage(getString("texture")));
	}
	return quad;
}

Text* Loader::parseText() {
	Text* text = new Text();
	parseCommon(text);
	text->setContent(getString("content"));
	text->setFont(Fonts::getFont(getString("font"), getInt( "size"), getInt("font_custom")));
	return text;
}

Collidable* Loader::parseSlider() {
	Collidable* slider = new Collidable();
	parseCommon(slider);
	slider->addFlag(MOVING);
	return slider;
}

map<int, Province*> Loader::getProvinceMap() { return province_map; }

map<int, Nation*> Loader::getNationMap() { return nation_map; }

map<int, Unit*> Loader::getUnitMap() { return unit_map; }

Province* Loader::parseProvince() {
	Province* province = new Province(getInt("id"));
	Font* font = Fonts::getFont(CONSOLAS_BOLD, 8);

	parseCommon(province);
	province->setTexture(Image::getImage("data/generated/" + to_string(province->getID()) + "_province.png"));
	province->setValue(getFloat("value"));
	province->setText(new Text(province->getLocation(), font, Colour(189, 195, 199, 150), province->getName()));
	province_map[province->getID()] = province;

	if (province->getTexture()->image == nullptr) {
		if (getString("texture", "NULL") != "NULL") province->setTexture(Image::getImage(getString("texture", "NULL")));
	}

	return province;
}

Unit* Loader::parseUnit() {
	Unit* unit = new Unit(getInt("id"), nullptr, getInt("size"), getFloat("skill"), province_map[getInt("province")]);
	parseCommon(unit);

	// Font* font = Fonts::getFont(CONSOLAS_BOLD, 16); // (189, 195, 199, 250)
	Font* font = Fonts::getFont("data/fonts/Cinzel-Bold.ttf", 16, true); // (189, 195, 199, 250)
	Text* unit_text = new Text(unit->getLocation(), font, Colour(255, 255, 255, 255), unit->getName(), 0.5f);
	unit_text->addFlag(TEXT_BACKGROUND);
	unit_text->removeFlag(FIXED_POS);
	unit->setText(unit_text);
	unit->setTextOffset(0, -0.0025);
	unit->location.x -= 1; // Offset x by -1 (sidescroller levelling)

	unit_map[getInt("id")] = unit;
	return unit;
}

Nation* Loader::parseNation() {
	Nation* nation = new Nation(getInt("id"), getString("name"), province_map[getInt("capital")]);
	parseCommon(nation);

	for (const auto& element : getArray("provinces")) {
		Province* province = province_map[(int)element];
		if (province == nullptr) continue;

		nation->addProvince(province);
		province->setColour(nation->getColour());
		log_t("Assigned province " CON_RED, province->getName(), CON_NORMAL " (" CON_RED, province->getID(), CON_NORMAL ") to nation " CON_RED, nation->getName(), CON_NORMAL " (" CON_RED, nation->getID(), CON_NORMAL ")");
	}

	for (const auto& element : getArray("units")) {
		Unit* unit = unit_map[(int)element];
		if (unit == nullptr) continue;
		nation->addUnit(unit);
		Colour col = nation->getColour();
		col.setW(200);
		unit->setColour(col);
		// unit->initiate();
		log_t("Assigned unit " CON_RED, unit->getName(), CON_NORMAL " (" CON_RED, unit->getID(), CON_NORMAL ") to nation " CON_RED, nation->getName(), CON_NORMAL " (" CON_RED, nation->getID(), CON_NORMAL ")");
	}

	nation_map[getInt("id")] = nation;
	return nation;
}

string Loader::getVariable(string var) {
	auto target_value = level_data.find("variables");
	if (target_value == level_data.end()) return "";
	json vars = level_data["variables"];
	return vars[var];
}

string Loader::getString(string key, string def) {
	if (properties.find(key) != properties.end()) {
		string value = properties[key];
		if (value[0] == 38) return getVariable(value.erase(0, 1)); // '&'
		return properties[key];
	}  return !def.empty() ? def : (string)DEFAULTS[key];
}

int Loader::getInt(string key) { return properties.find(key) != properties.end() ? properties[key] : DEFAULTS[key]; }

float Loader::getFloat(string key) { return properties.find(key) != properties.end() ? properties[key] : DEFAULTS[key]; }

json::array_t Loader::getArray(string key) {
	auto target_value = properties.find(key);
	return target_value != properties.end() && target_value.value().is_array() ? properties[key] : DEFAULTS[key];
}

Level* Loader::load_font(string f, vector <Moveable*>* q, vector<Text*>* t, int identifier) {
	Level* level = new Level();
	ifstream level_file(f);
	json level_data = json::parse(level_file);

	log_t("Loading level... " CON_RED + f + CON_NORMAL " [" + CON_RED + to_string(level_data["name"]) + CON_NORMAL + "]");

	Moveable* background = new Moveable(Vector2(), Vector2(1.0, 1.0),
		Colour(level_data["background"], level_data["background_alpha"]), 
		Colour(level_data["alt_background"], level_data["alt_background_alpha"]));
	background->addFlag(UNEDITABLE);
	background->setName("Background " + to_string(identifier));

	auto target_value = level_data.find("image");
	if (target_value != level_data.end()) {
		background->setTexture(Image::getImage(level_data["image"]));
	}

	level->objects.push_back(background);
	level_data = level_data;

	for (auto& el : level_data["objects"].items()) {
		properties = el.value();
		string type = properties["type"];

		if (type == "COLLIDABLE") {
			level->objects.push_back(parseCollidable());
		} else if (type == "QUAD") {
			level->objects.push_back(parseCustom());
		} else if (type == "MOVEABLE") {
			level->objects.push_back(parseMoveable());
		} else if (type == "CIRCLE") {
			level->objects.push_back(parseCircle());
		} /*else if (type == "SQUARE") {
			level->objects.push_back(parseSquare());
		}*/ else if (type == "TEXT") {
			level->text_objects.push_back(parseText());
		} else if (type == "SLIDER") {
			level->objects.push_back(parseSlider());
		} else if (type == "PROVINCE") {
			Province* province = parseProvince();
			province->addFlag(PROVINCE);
			level->objects.push_back(province);
			// level->text_objects.push_back(province->getText());
		} else if (type == "UNIT") {
			Unit* unit = parseUnit();
			level->objects.push_back(unit);
			level->text_objects.push_back(unit->getText());
		} else if (type == "NATION") {
			Nation* nation = parseNation();
			level->objects.push_back(nation);
		}
		/* else if (type == "FIRE") {
			Fire* fire_object = parseFire(object_data);
			fire_object->objects = q;
			level->objects.push_back(fire_object);
		}
		else if (type == "STARS") {
			Stars* stars_object = parseStars(object_data);
			stars_object->objects = q;
			level->objects.push_back(stars_object);
		} */
		else {
			level->objects.push_back(parseMoveable());
		}
	}

	log_t("Finished parsing ", level->objects.size(), " object(s).");
	level_file.close();
	return level;
}
