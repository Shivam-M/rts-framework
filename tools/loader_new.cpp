#include "loader_new.h"

#include "../defs/province_new.h"
#include "../defs/unit_new.h"
#include "../defs/nation_new.h"

#include "../assets/text_new.h"
#include "../shapes/circle_new.h"
#include "../assets/collidable_new.h"

json LoaderNew::ldata;

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

static map<int, ProvinceNew*> province_map;
static map<int, NationNew*> nation_map;
static map<int, UnitNew*> unit_map;

static json properties;
stringstream ss;

void LoaderNew::parseCommon(MoveableNew* moveable) {
	string script = getString("script");

	if (script != "") moveable->loadScript(script);
	moveable->setName(getString("name"));
	moveable->setSize(getFloat("width"), getFloat("height"));
	moveable->setLocation(getFloat("x"), getFloat("y"));
	moveable->setColour(Colour2::HexToRGB(getString("colour"), (getFloat("alpha"))));
	if (getString("alt_colour") != "") {
		moveable->setColour(Colour2::HexToRGB(getString("alt_colour"), (getFloat("alt_alpha"))));
	}

}

/*
Fire* LoaderNew::parseFire() {
	return nullptr;
}

Stars* LoaderNew::parseStars() {
	return nullptr;
}

Square* LoaderNew::parseSquare() {
	return nullptr;
}
*/

CircleNew* LoaderNew::parseCircle() {
	CircleNew* circle = new CircleNew();
	parseCommon(circle);
	return circle;
}

CollidableNew* LoaderNew::parseCollidable() {
	CollidableNew* collidable = new CollidableNew();
	parseCommon(collidable);
	return collidable;
}

MoveableNew* LoaderNew::parseMoveable() {
	MoveableNew* moveable = new MoveableNew();
	parseCommon(moveable);
	return moveable;
}

CollidableNew* LoaderNew::parseCustom() {
	CollidableNew* quad = new CollidableNew();
	parseCommon(quad);

	/*
	vector<Vector2> points;
	int c, d = 0;
	for (auto& points_data : properties["points"].items()) {
		c = 0;
		map<float, float> point = {};
		for (float point : points_data.value()) {
			points[c][d] = point;
			c++;
		} d++;
	}
	*/

	quad->addFlag(CUSTOM);
	// quad->setPoints(points[0], points[1], points[2], points[3]);
	// quad->setSize(abs(points[0][0] - points[1][0]), abs(points[0][1] - points[1][1])); // Temp
	if (getString("texture", "NULL") != "NULL") {
		quad->setTexture(Image::getImage(getString("texture")));
	}
	
	return quad;
}

TextNew* LoaderNew::parseText() {
	TextNew* text = new TextNew();
	text->setColour(Colour2::HexToRGB(getString("colour"), (getFloat("alpha"))));
	text->setContent(getString("content"));
	text->setFont(Fonts::getFont(getString("font"), getInt( "size"), getInt("font_custom")));
	text->setLocation(getFloat("x"), getFloat("y"));
	return text;
}

CollidableNew* LoaderNew::parseSlider() {
	CollidableNew* slider = new CollidableNew();
	parseCommon(slider);
	slider->addFlag(MOVING);
	return slider;
}

map<int, ProvinceNew*> LoaderNew::getProvinceMap() { return province_map; }

map<int, NationNew*> LoaderNew::getNationMap() { return nation_map; }

map<int, UnitNew*> LoaderNew::getUnitMap() { return unit_map; }

ProvinceNew* LoaderNew::parseProvince() {
	ProvinceNew* province = new ProvinceNew(getInt("id"), getString("name"));
	font_data font = Fonts::getFont(CONSOLAS_BOLD, 8);

	parseCommon(province);
	province->addFlag(TEXTURED);
	province->setTexture(Image::getImage("data/generated/" + to_string(province->getID()) + "_province.png"));
	province->setValue(getFloat("value"));
	province->setText(new TextNew(province->getLocation(), font, Colour2(189, 195, 199, 150), province->getName()));
	// province->setTextOffset(province->getSize()[0] / 4, province->getSize()[1] / 4);
	province_map[province->getID()] = province;

	if (province->getTexture()->image == nullptr) {
		if (getString("texture", "NULL") != "NULL") province->setTexture(Image::getImage(getString("texture", "NULL")));
	}

	return province;
}

UnitNew* LoaderNew::parseUnit() {
	UnitNew* unit = new UnitNew(getInt("id"), nullptr, getInt("size"), getFloat("skill"), province_map[getInt("province")]);
	parseCommon(unit);

	unit->location.x -= 1; // Offset x by -1 (sidescroller levelling)

	font_data font = Fonts::getFont(CONSOLAS_BOLD, 8);
	unit->setText(new TextNew(unit->getLocation(), font, Colour2(189, 195, 199, 250), unit->getName()));
	unit->setTextOffset(0, -0.0025);
	unit->addFlag(UNIT);

	unit_map[getInt("id")] = unit;
	return unit;
}

NationNew* LoaderNew::parseNation() {
	NationNew* nation = new NationNew(getInt("id"), getString("name"), province_map[getInt("capital")]);
	parseCommon(nation);

	for (const auto& element : getArray("provinces")) {
		ProvinceNew* province = province_map[(int)element];
		if (province == nullptr) continue;

		short* colour = rgb(getString("colour"));
		nation->setColour(Colour2::HexToRGB(getString("colour"), getFloat("alpha")));

		nation->addProvince(province);
		province->setColour(nation->getColour());

		info(ss << "Assigned province '" << province->getName() << "' (" << province->getID() << ") to nation '" << nation->getName() << "' (" << nation->getID() << ")");
	}

	for (const auto& element : getArray("units")) {
		UnitNew* unit = unit_map[(int)element];
		if (unit == nullptr) continue;
		nation->addUnit(unit);
		unit->setColour(nation->getColour());
		info(ss << "Assigned unit '" << unit->getName() << "' (" << unit->getID() << ") to nation '" << nation->getName() << "' (" << nation->getID() << ")");
	}

	nation_map[getInt("id")] = nation;
	return nation;
}

string LoaderNew::getVariable(string var) {
	auto target_value = ldata.find("variables");
	if (target_value == ldata.end()) return "";
	json vars = ldata["variables"];
	return vars[var];
}

string LoaderNew::getString(string key, string def) {
	if (properties.find(key) != properties.end()) {
		string value = properties[key];
		if (value[0] == 38) return getVariable(value.erase(0, 1)); // '&'
		return properties[key];
	} 
	return !def.empty() ? def : (string)DEFAULTS[key];
}

int LoaderNew::getInt(string key) { return properties.find(key) != properties.end() ? properties[key] : DEFAULTS[key]; }

float LoaderNew::getFloat(string key) { return properties.find(key) != properties.end() ? properties[key] : DEFAULTS[key]; }

json::array_t LoaderNew::getArray(string key) {
	auto target_value = properties.find(key);
	return target_value != properties.end() && target_value.value().is_array() ? properties[key] : DEFAULTS[key];
}

Level2* LoaderNew::load(string f, vector <MoveableNew*>* q, vector<TextNew*>* t, int identifier) {
	Level2* level = new Level2();
	ifstream level_file(f);
	json level_data = json::parse(level_file);

	info(ss << "Loading level... " << f << " [" << "PLACEHOLDER TEXT" << "]");

	MoveableNew* background = new MoveableNew(Vector2(), Vector2(1.0, 1.0), Colour2::HexToRGB(level_data["background"], level_data["background_alpha"]), Colour2::HexToRGB(level_data["alt_background"], level_data["alt_background_alpha"]));
	background->addFlag(UNEDITABLE);
	background->setName("Background " + to_string(identifier));

	auto target_value = level_data.find("image");
	if (target_value != level_data.end()) {
		background->addFlag(TEXTURED);
		background->setTexture(Image::getImage(level_data["image"]));
	}

	level->objects.push_back(background);
	ldata = level_data;

	for (auto& el : level_data["objects"].items()) {
		properties = el.value();
		string type = properties["type"];

		if (type == "COLLIDABLE") {
			level->objects.push_back(parseCollidable());
		}
		else if (type == "QUAD") {
			level->objects.push_back(parseCustom());
		}
		else if (type == "MOVEABLE") {
			level->objects.push_back(parseMoveable());
		}
		else if (type == "CIRCLE") {
			level->objects.push_back(parseCircle());
		}
		/*
		else if (type == "SQUARE") {
			level->objects.push_back(parseSquare());
		}*/
		else if (type == "TEXT") {
			level->text_objects.push_back(parseText());
		}
		else if (type == "SLIDER") {
			level->objects.push_back(parseSlider());
		}
		
		else if (type == "PROVINCE") {
			ProvinceNew* province = parseProvince();
			province->addFlag(PROVINCE);
			level->objects.push_back(province);
			// level->text_objects.push_back(province->getText());
		}
		else if (type == "UNIT") {
			UnitNew* unit = parseUnit();
			level->objects.push_back(unit);
			level->text_objects.push_back(unit->getText());
		}
		else if (type == "NATION") {
			NationNew* nation = parseNation();
			level->objects.push_back(nation);
		}
		/*
		else if (type == "FIRE") {
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

	info(ss << "Finished parsing " << level->objects.size() << " object(s).");
	level_file.close();
	return level;
}
