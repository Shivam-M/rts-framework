#include "loader.h"

#include "../assets/province.h"
#include "../assets/unit.h"
#include "../assets/nation.h"

#include "../assets/particle_group.h"
#include "../assets/text.h"
#include "../assets/collidable.h"

#include "../tools/ui_manager.h"

json Loader::current_level_data;

const json DEFAULTS = {
	{"colour",			"FFFFFF"},
	{"alpha",			1},
	{"alt_colour",		""},
	{"alt_alpha",		1.0f},
	{"secondary_colour",""},
	{"secondary_alpha",	1.0f},
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
	{"scale",			1.0f},
	{"texture",			"NULL"},
	{"variables",		nullptr},
	{"image",			nullptr},
	{"parent",			nullptr},
	{"value",			0.10f},
	{"id",				-1},
	{"flags",			0},
	{"provinces",		json::array()},
	{"units",			json::array()},
	{"neighbours",		json::array()},
	{"offset",			true},
	{"fixed",			0},
	{"priority",		0.00},
	{"alignment",		0},
	{"metadata",		""},
	{"draggable",		false},
	{"action",			0},
	{"blend_type",		0},
	{"blend_time",		1.0f},
	{"blend_speed",		1.0f},
	{"blend_size",		1.0f},
	{"blend_fixed",		false},
	{"blend_direction",	json::array()}
	// TODO: See header json methods
	//{"blend",			{
	//						{"type",	0},
	//						{"time",	1.0f},
	//						{"speed",	1.0f},
	//						{"size",	1.0f},
	//						{"fixed",	false}
	//						// {"direction", {0.5f, 0.5f}},
	//					}}
};

static map<int, Province*> province_map;
static map<int, Nation*> nation_map;
static map<int, Unit*> unit_map;
static map<string, Panel*> panel_map;

static json properties;

void Loader::parseCommon(Moveable* moveable) {
	string script = getString("script");
	if (script != "") moveable->loadScript(script);
	moveable->addFlag(getInt("flags"));
	moveable->setName(getString("name"));
	moveable->setSize(getFloat("width"), getFloat("height"));
	moveable->setLocation(getFloat("x"), getFloat("y"));
	moveable->setColour(Colour::HexToRGB(getString("colour"), (getFloat("alpha"))));
	moveable->metadata = getString("metadata");
	if (getString("alt_colour") != "") {
	 	moveable->setGradientColour(Colour::HexToRGB(getString("alt_colour"), (getFloat("alt_alpha"))));
	}
	if (getString("texture", "NULL") != "NULL") {
		moveable->setTexture(Image::getImage(getString("texture")));
	}
	if (getString("parent", "NULL") != "NULL") {
		Panel* parent_panel = panel_map[getString("parent")];
		parent_panel->add(moveable);
	}
	if (getInt("fixed")) {
		moveable->addFlag(FIXED_POS);
	}
	if (getInt("blend_type")) {
		Blend blend = Blend(getInt("blend_type"), getFloat("blend_speed"), getFloat("blend_size"), Vector2(0.5f, 0.5f), getBool("blend_fixed"));
		moveable->setBlend(blend);
	}
}

ParticleGroup* Loader::parseStars(vector<Moveable*>* queue) { // TODO PARSE VALUES
	Moveable star = Moveable({}, { 0.0025f, 0.0025f * (16.0f / 9.0f) }, Colour(255, 255, 255, 150), Colour(255, 255, 255, 150));
	ColourShift colourshift = ColourShift(star.getColour(), Colour(255, 255, 255, 0));
	colourshift.speed = 0.05f;
	colourshift.with_gradient = true;
	colourshift.fade_to_death = true;
	star.setColourShift(colourshift);
	ParticleGroup* stars = new ParticleGroup({ 0.0f, 0.0f }, { 1.0f, 1.0f }, star, 75, queue);
	parseCommon(stars);
	return stars;
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

Moveable* Loader::parseButton() {
	Moveable* button = new Moveable();
	Text* text = parseText();
	parseCommon(button);

	Vector2 dimensions = TextRenderer::calculate_text_dimensions(text->getFont(), text->getContent(), text->getScale());
	float x_offset = (button->size.x - (dimensions.x / WINDOW_WIDTH)) / 2.0f;
	float y_offset = (button->size.y + (dimensions.y / WINDOW_HEIGHT)) / 2.0f;

	text->colour.setW(200);

	if (getString("alt_colour") != "") {
		button->setColour(Colour::HexToRGB(getString("alt_colour"), (getFloat("alt_alpha"))));
	}
	button->setText(text);
	button->setTextOffset(x_offset, y_offset);
	button->setButtonAction((BUTTON_ACTION)getInt("action"));
	if (getInt("fixed")) {
		button->addFlag(FIXED_POS);
	}
	return button;
}

Text* Loader::parseText() {
	Text* text = new Text();
	parseCommon(text);
	text->setScale(getFloat("scale"));
	string a = getString("font");
	text->setFont(Fonts::getFont(getString("font"), getInt("size"), getInt("font_custom")));
	text->setContent(getString("content"));
	text->setAlignment((ALIGNMENT)getInt("alignment"));
	return text;
}

Collidable* Loader::parseSlider() {
	Collidable* slider = new Collidable();
	parseCommon(slider);
	slider->addFlag(MOVING);
	return slider;
}

Panel* Loader::parsePanel() {
	Panel* panel = new Panel();
	panel->setPriority(getFloat("priority"));
	panel->addFlag(PANEL);
	parseCommon(panel);
	panel_map[getString("name")] = panel;
	UIManager::Register(getString("name"), panel);
	if (getBool("draggable")) {
		panel->addFlag(DRAGGABLE);
	}
	return panel;
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
	province->addFlag(PROVINCE);
	province_map[province->getID()] = province;

	if (province->getTexture()->image == nullptr) {
		if (getString("texture", "NULL") != "NULL") province->setTexture(Image::getImage(getString("texture", "NULL")));
	}

	Blend blend = Blend(2, 1.0f, 1.0f, Vector2(-1.f, -1.5f), false);
	province->setBlend(blend);

	return province;
}

Unit* Loader::parseUnit() {
	Unit* unit = new Unit(getInt("id"), nullptr, getInt("size"), getFloat("skill"), province_map[getInt("province")]);
	parseCommon(unit);

	Font* font = Fonts::getFont("data/fonts/Cinzel-Bold.ttf", 16, true); // (189, 195, 199, 250)
	Text* unit_text = new Text(unit->getLocation(), font, Colour(220, 221, 225, 200), unit->getName(), 0.5f);
	unit_text->addFlag(TEXT_BACKGROUND | UNSAVEABLE);
	unit_text->removeFlag(FIXED_POS);
	unit->setText(unit_text);
	unit->setTextOffset(0, -0.0025);
	// unit->location.x -= 1; // Offset x by -1 (sidescroller levelling) -- unused?

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
	auto target_value = current_level_data.find("variables");
	if (target_value == current_level_data.end()) return "";
	json vars = current_level_data["variables"];
	return vars[var];
}

string Loader::getString(string key, string def) {
	if (properties.find(key) != properties.end()) {
		string value = properties[key];
		if (value[0] == 38) return getVariable(value.erase(0, 1)); // '&'
		return properties[key];
	} return !def.empty() ? def : (string)DEFAULTS[key];
}

template <typename T>
T Loader::getValue(string key) { return properties.find(key) != properties.end() ? properties[key] : DEFAULTS[key]; }

bool Loader::getBool(string key) { return getValue<bool>(key); }
int Loader::getInt(string key) { return getValue<int>(key); }
float Loader::getFloat(string key) { return getValue<float>(key); }

//template <typename T>
//T Loader::getJsonValue(string key, string inner_key) {
//	json main = getValue<json>(key);
//	return main.find(inner_key) != main.end() ? main[inner_key] : DEFAULTS[key][inner_key];
//}

json::array_t Loader::getArray(string key) {
	auto target_value = properties.find(key);
	return target_value != properties.end() && target_value.value().is_array() ? properties[key] : DEFAULTS[key];
}

Level* Loader::load_level(string f, vector <Moveable*>* q, vector<Text*>* t, int identifier) {
	Level* level = new Level();
	ifstream level_file(f);
	json level_data = json::parse(level_file);

	log_t("Loading level... " CON_RED + f + CON_NORMAL " [" + CON_RED + (string)level_data["level_name"] + CON_NORMAL + "]");

	Moveable* background = new Moveable(
		Vector2(),
		Vector2(1.0, 1.0),
		Colour(level_data["background"], level_data["background_alpha"]), 
		Colour(level_data["alt_background"], level_data["alt_background_alpha"])
	);
	background->addFlag(UNEDITABLE);
	background->setName("Background " + to_string(identifier));

	Blend blend = Blend(1, 1.0f, 1.0f, Vector2(0.5f, 1.0f), false);
	background->setBlend(blend);

	auto target_value = level_data.find("image");
	if (target_value != level_data.end()) {
		background->setTexture(Image::getImage(level_data["image"]));
	}

	target_value = level_data.find("fixed_position");
	if (target_value != level_data.end()) {
		if ((bool)level_data["fixed_position"]) {
			background->addFlag(FIXED_POS);
		}
	}

	target_value = level_data.find("offset_positions");
	if (target_value != level_data.end()) {
		if (!(bool)level_data["offset_positions"]) {
			level->offset_positions = false;
		}
	}

	level->objects.push_back(background);
	current_level_data = level_data;

	for (auto& el : level_data["objects"].items()) {
		properties = el.value();
		string type = properties["type"];

		if (type == "COLLIDABLE") {
			level->objects.push_back(parseCollidable());
		} else if (type == "PANEL") {
			level->objects.push_back(parsePanel());
		} else if (type == "STARS") {
			level->objects.push_back(parseStars(q));
		} else if (type == "QUAD") {
			level->objects.push_back(parseCustom());
		} else if (type == "MOVEABLE") {
			level->objects.push_back(parseMoveable());
		} else if (type == "TEXT") {
			level->text_objects.push_back(parseText());
		} else if (type == "BUTTON") {
			level->objects.push_back(parseButton());
		} else if (type == "SLIDER") {
			level->objects.push_back(parseSlider());
		} else if (type == "PROVINCE") {
			level->objects.push_back(parseProvince());
		} else if (type == "NATION") {
			level->objects.push_back(parseNation());
		} else if (type == "UNIT") {
			Unit* unit = parseUnit();
			level->objects.push_back(unit);
			level->text_objects.push_back(unit->getText());
		} else {
			level->objects.push_back(parseMoveable());
		}
	}

	log_t("Finished parsing " CON_RED, level->objects.size(), CON_NORMAL " object(s).");
	level_file.close();
	return level;
}

void Loader::writeObjects(vector<Moveable*> objects, vector<Text*> text_objects) {
	ifstream file("data/levels/debug/0.json");
	if (!file.is_open()) return;

	json level_data;
	file >> level_data;
	file.close();

	json delayed_additions = json::array();
	json objects_array = json::array();
	for (Moveable* m : objects) {
		if (m->hasFlag(DEBUG)) continue;
		if (m->hasFlag(PANEL)) continue; // temp
		if (m->hasFlag(UNSAVEABLE)) continue;
		if (m->hasFlag(UI)) continue;
		if (m->getName().find("Background") != string::npos) continue;
		json moveable_data = {
			{"type", "MOVEABLE"},
			{"x", m->location.x },
			{"y", m->location.y },
			{"width", m->size.x },
			{"height", m->size.y },
			{"colour", m->getColour().getHex() },
			{"alt_colour", m->getGradientColour().getHex() },
			{"alpha", m->getColour().getW() / 255.0f},
			{"alt_alpha", m->getGradientColour().getW() / 255.0f},
			{"name", m->getName()},
			{"flags", m->getFlags() }
		};
		if (m->hasFlag(BUTTON)) {
			moveable_data["type"] = "BUTTON";
			moveable_data["action"] = reinterpret_cast<Moveable*>(m)->getButtonAction();

			Text* t = m->getText();
			moveable_data["font"] = Fonts::getProperties(t->getFont()).first;
			moveable_data["font_custom"] = (int)(Fonts::getProperties(t->getFont()).second);
			moveable_data["size"] = t->getFont()->h;
			moveable_data["content"] = t->getContent();
			moveable_data["scale"] = t->getScale();
		}
		// Note: in the level loader, units are pushed by -1 on the x-axis -- shouldn't be a problem if assigned to a province
		if (m->hasFlag(TEXTURED)) {
			moveable_data["texture"] = m->getTexture()->path;
		}
		if (m->hasFlag(PROVINCE)) {
			moveable_data["type"] = "PROVINCE";
			moveable_data["id"] = reinterpret_cast<Province*>(m)->getID();
		}
		if (m->hasFlag(UNIT)) {
			moveable_data["type"] = "UNIT";
			moveable_data["id"] = reinterpret_cast<Unit*>(m)->getID();
			moveable_data["province"] = reinterpret_cast<Unit*>(m)->getProvince()->getID();
			moveable_data["skill"] = reinterpret_cast<Unit*>(m)->getSkill();
			moveable_data["size"] = reinterpret_cast<Unit*>(m)->getAmount();
		}
		if (m->hasFlag(NATION)) {
			moveable_data["type"] = "NATION";
			moveable_data["id"] = reinterpret_cast<Nation*>(m)->getID();
			moveable_data["capital"] = reinterpret_cast<Nation*>(m)->getCapital()->getID();
			moveable_data["provinces"] = {};
			moveable_data["units"] = {};
			for (Unit* unit : reinterpret_cast<Nation*>(m)->getOwnedUnits()) moveable_data["units"].push_back(unit->getID());
			for (Province* province : reinterpret_cast<Nation*>(m)->getOwnedProvinces()) moveable_data["provinces"].push_back(province->getID());
			delayed_additions.push_back(moveable_data);
			continue;
		}
		if (m->hasFlag(COLLIDABLE)) moveable_data["type"] = "SLIDER";

		objects_array.push_back(moveable_data);
	}

	objects_array.insert(objects_array.end(), delayed_additions.begin(), delayed_additions.end());

	for (Text* t : text_objects) {
		if (t->hasFlag(DEBUG)) continue;
		if (t->hasFlag(UI)) continue;
		if (t->hasFlag(UNSAVEABLE)) continue;
		json moveable_data = {
			{"x", t->location.x },
			{"y", t->location.y },
			{"width", t->size.x },
			{"height", t->size.y },
			{"colour", t->getColour().getHex() },
			{"alpha", t->getColour().getW() / 255.0f},
			{"name", t->getName()}
		};
		if (t->hasFlag(TEXT)) {
			moveable_data["type"] = "TEXT";
			moveable_data["font"] = Fonts::getProperties(t->getFont()).first;
			moveable_data["font_custom"] = (int)(Fonts::getProperties(t->getFont()).second);
			moveable_data["size"] = t->getFont()->h;
			moveable_data["content"] = t->getContent();
			moveable_data["scale"] = t->getScale();
		}
		objects_array.push_back(moveable_data);
	}
	log_t(objects_array.dump(4));

	ofstream level_file("data/levels/debug/0.json");
	if (!level_file.is_open()) return;

	level_data["objects"].clear();
	level_data["objects"] = objects_array;
	level_file << level_data.dump(4);
	level_file.close();
}