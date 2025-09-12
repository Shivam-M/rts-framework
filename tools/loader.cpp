#include "loader.h"

#include "../assets/province.h"
#include "../assets/unit.h"
#include "../assets/nation.h"
#include "../assets/particle_group.h"
#include "../assets/text.h"
#include "../assets/collidable.h"
#include "../assets/panel.h"
#include "../tools/ui_manager.h"
#include "../tools/text_renderer.h"
#include "../tools/fonts.h"
#include "../tools/image.h"

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
	{"unfixed",         false},
	{"uneditable",      false},
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
static string instance_suffix = "";

void Loader::parse_common(Moveable* moveable) {
	string script = get_string("script");
	if (script != "") moveable->load_script(script);
	moveable->add_flag(get_int("flags"));
	moveable->set_name(get_string("name"));
	moveable->set_size(get_float("width"), get_float("height"));
	moveable->set_location(get_float("x"), get_float("y"));
	moveable->set_colour(Colour::hex_to_rgb(get_string("colour"), (get_float("alpha"))));
	moveable->metadata = get_string("metadata");
	if (get_string("alt_colour") != "") {
	 	moveable->set_gradient_colour(Colour::hex_to_rgb(get_string("alt_colour"), (get_float("alt_alpha"))));
	}
	if (get_string("texture", "NULL") != "NULL") {
		moveable->set_texture(Image::get_image(get_string("texture")));
	}
	if (get_string("parent", "NULL") != "NULL") {
		Panel* parent_panel = panel_map[get_string("parent") + instance_suffix];
		parent_panel->add(moveable);
	}
	if (get_int("fixed")) {
		moveable->add_flag(FIXED_POS);
	}
	if (get_int("blend_type")) {
		Blend blend = Blend(get_int("blend_type"), get_float("blend_speed"), get_float("blend_size"), Vector2(0.5f, 0.5f), get_bool("blend_fixed"));
		moveable->set_blend(blend);
	}
	if (get_bool("uneditable")) {
		moveable->add_flag(UNEDITABLE);
	}
}

ParticleGroup* Loader::parse_stars(vector<Moveable*>* queue) { // TODO PARSE VALUES
	Moveable star = Moveable({}, { 0.0025f, 0.0025f * (16.0f / 9.0f) }, Colour(255, 255, 255, 150), Colour(255, 255, 255, 150));
	ColourShift colourshift = ColourShift(star.get_colour(), Colour(255, 255, 255, 0));
	colourshift.speed = 0.05f;
	colourshift.with_gradient = true;
	colourshift.fade_to_death = true;
	star.set_colour_shift(colourshift);
	ParticleGroup* stars = new ParticleGroup({ 0.0f, 0.0f }, { 1.0f, 1.0f }, star, 75, queue);
	parse_common(stars);
	return stars;
}

Collidable* Loader::parse_collidable() {
	Collidable* collidable = new Collidable();
	parse_common(collidable);
	return collidable;
}

Moveable* Loader::parse_moveable() {
	Moveable* moveable = new Moveable();
	parse_common(moveable);
	return moveable;
}

Collidable* Loader::parse_custom() {
	Collidable* quad = new Collidable();
	parse_common(quad);

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

	quad->add_flag(CUSTOM);
	// quad->setPoints(points[0], points[1], points[2], points[3]);
	// quad->set_size(abs(points[0][0] - points[1][0]), abs(points[0][1] - points[1][1])); // Temp
	if (get_string("texture") != "NULL") {
		quad->set_texture(Image::get_image(get_string("texture")));
	}
	return quad;
}

Moveable* Loader::parse_button() {
	Moveable* button = new Moveable();
	Text* text = parse_text();
	parse_common(button);

	Vector2 dimensions = TextRenderer::calculate_text_dimensions(text->get_font(), text->get_content(), text->get_scale());
	float x_offset = (button->size.x - (dimensions.x / WINDOW_WIDTH)) / 2.0f;
	float y_offset = (button->size.y + (dimensions.y / WINDOW_HEIGHT)) / 2.0f;

	text->colour.set_alpha(200);

	if (get_string("alt_colour") != "") {
		button->set_colour(Colour::hex_to_rgb(get_string("alt_colour"), (get_float("alt_alpha"))));
	}
	button->set_text(text);
	button->set_text_offset(x_offset, y_offset);
	button->set_button_action((BUTTON_ACTION)get_int("action"));
	if (get_int("fixed")) {
		button->add_flag(FIXED_POS);
	}
	return button;
}

Text* Loader::parse_text() {
	Text* text = new Text();
	parse_common(text);
	text->set_scale(get_float("scale"));
	string a = get_string("font");
	text->set_font(Fonts::get_font(get_string("font"), get_int("size"), get_int("font_custom")));
	text->set_content(get_string("content"));
	text->set_alignment((ALIGNMENT)get_int("alignment"));
	if (get_bool("unfixed")) {  // use existing fixed var
		text->remove_flag(FIXED_POS); // TODO: temp, no flag should be default in text constructor
	}
	return text;
}

Collidable* Loader::parse_slider() {
	Collidable* slider = new Collidable();
	parse_common(slider);
	slider->add_flag(MOVING);
	return slider;
}

Panel* Loader::parse_panel() {
	Panel* panel = new Panel();  // maybe store as a prefab?
	panel->set_priority(get_float("priority"));
	panel->add_flag(PANEL);
	parse_common(panel);
	string name = get_string("name");
	if (instance_suffix != "") {
		name += instance_suffix;
	}
	panel_map[name] = panel;
	UIManager::register_panel(name, panel);
	if (get_bool("draggable")) {
		panel->add_flag(DRAGGABLE);
	}
	return panel;
}

map<int, Province*>& Loader::get_province_map() { return province_map; }

map<int, Nation*>& Loader::get_nation_map() { return nation_map; }

map<int, Unit*>& Loader::get_unit_map() { return unit_map; }

Province* Loader::parse_province() {
	Province* province = new Province(get_int("id"));
	Font* font = Fonts::get_font("data/fonts/consolab.ttf", 8, true);

	parse_common(province);
	province->set_texture(Image::get_image("data/generated/" + to_string(province->get_id()) + "_province.png"));
	province->set_value(get_float("value"));
	province->set_text(new Text(province->get_location(), font, Colour(189, 195, 199, 150), province->get_name()));
	province->add_flag(PROVINCE);
	province_map[province->get_id()] = province;

	if (province->get_texture()->image == nullptr) {
		if (get_string("texture", "NULL") != "NULL") province->set_texture(Image::get_image(get_string("texture", "NULL")));
	}

	/*Blend blend = Blend(2, 1.0f, 1.0f, Vector2(-1.f, -1.5f), false);
	province->set_blend(blend);*/

	return province;
}

Unit* Loader::parse_unit() {
	Unit* unit = new Unit(get_int("id"), nullptr, get_int("size"), get_float("skill"), province_map[get_int("province")]);
	parse_common(unit);

	Font* font = Fonts::get_font("data/fonts/Cinzel-Bold.ttf", 16, true); // (189, 195, 199, 250)
	Text* unit_text = new Text(unit->get_location(), font, Colour(220, 221, 225, 200), unit->get_name(), 0.5f);
	unit_text->set_alignment(CENTRE);
	unit_text->add_flag(TEXT_BACKGROUND | UNSAVEABLE);
	unit_text->remove_flag(FIXED_POS);
	unit->set_text(unit_text);
	unit->set_text_offset(0, -0.0025);
	// unit->location.x -= 1; // Offset x by -1 (sidescroller levelling) -- unused?

	unit_map[get_int("id")] = unit;
	return unit;
}

Nation* Loader::parse_nation() {
	Nation* nation = new Nation(get_int("id"), get_string("name"), province_map[get_int("capital")]);
	parse_common(nation);
	nation->add_flag(NO_RENDER);

	for (const auto& element : get_array("provinces")) {
		Province* province = province_map[(int)element];
		if (province == nullptr) continue;

		nation->add_province(province);
		province->set_colour(nation->get_colour());
		log_t("Assigned province " CON_RED, province->get_name(), CON_NORMAL " (" CON_RED, province->get_id(), CON_NORMAL ") to nation " CON_RED, nation->get_name(), CON_NORMAL " (" CON_RED, nation->get_id(), CON_NORMAL ")");
	}

	for (const auto& element : get_array("units")) {
		Unit* unit = unit_map[(int)element];
		if (unit == nullptr) continue;
		nation->add_unit(unit);
		Colour col = nation->get_colour();
		col.set_alpha(200);
		unit->set_colour(col);
		// unit->initialise();
		log_t("Assigned unit " CON_RED, unit->get_name(), CON_NORMAL " (" CON_RED, unit->get_id(), CON_NORMAL ") to nation " CON_RED, nation->get_name(), CON_NORMAL " (" CON_RED, nation->get_id(), CON_NORMAL ")");
	}

	nation_map[get_int("id")] = nation;
	return nation;
}

string Loader::get_variable(const string& var) {
	auto target_value = current_level_data.find("variables");
	if (target_value == current_level_data.end()) return "";
	json vars = current_level_data["variables"];
	return vars[var];
}

string Loader::get_string(const string& key, const string& def) {
	if (properties.find(key) != properties.end()) {
		string value = properties[key];
		if (value[0] == 38) return get_variable(value.erase(0, 1)); // '&'
		return properties[key];
	} return !def.empty() ? def : (string)DEFAULTS[key];
}

template <typename T>
T Loader::get_value(const string& key) { return properties.find(key) != properties.end() ? properties[key] : DEFAULTS[key]; }

bool Loader::get_bool(const string& key) { return get_value<bool>(key); }
int Loader::get_int(const string& key) { return get_value<int>(key); }
float Loader::get_float(const string& key) { return get_value<float>(key); }

//template <typename T>
//T Loader::getJsonValue(string key, string inner_key) {
//	json main = getValue<json>(key);
//	return main.find(inner_key) != main.end() ? main[inner_key] : DEFAULTS[key][inner_key];
//}

json::array_t Loader::get_array(const string& key) {
	auto target_value = properties.find(key);
	return target_value != properties.end() && target_value.value().is_array() ? properties[key] : DEFAULTS[key];
}

Level* Loader::load_level(string f, vector <Moveable*>* q, vector<Text*>* t, const int& identifier, const string& instance_name) {  // todo: refactor weird mix of static and non-static
	Level* level = new Level();
	ifstream level_file(f);
	if (!level_file.is_open()) {
		log_t(CON_RED "ERROR! Level file does not exist: ", f);
		return nullptr;
	}
	json level_data = json::parse(level_file);

	instance_suffix = instance_name != "" ? "-" + instance_name : "";

	log_t("Loading level... " CON_RED + f + CON_NORMAL " [" + CON_RED + (string)level_data["level_name"] + CON_NORMAL + "]");

	bool no_background = false;
	auto target_value = level_data.find("no_background");
	if (target_value != level_data.end()) {
		if ((bool)level_data["no_background"]) {
			no_background = true;
		}
	}

	if (!no_background) {
		Moveable* background = new Moveable(
			Vector2(),
			Vector2(1.0, 1.0),
			Colour(level_data["background"], level_data["background_alpha"]),
			Colour(level_data["alt_background"], level_data["alt_background_alpha"])
		);
		background->add_flag(UNEDITABLE);
		background->set_name("Background " + to_string(identifier));

		Blend blend = Blend(1, 1.0f, 1.0f, Vector2(0.5f, 1.0f), false);
		background->set_blend(blend);

		auto target_value = level_data.find("image");
		if (target_value != level_data.end()) {
			background->set_texture(Image::get_image(level_data["image"]));
		}

		target_value = level_data.find("fixed_position");
		if (target_value != level_data.end()) {
			if ((bool)level_data["fixed_position"]) {
				background->add_flag(FIXED_POS);
			}
		}

		level->objects.push_back(background);
	}

	target_value = level_data.find("offset_positions");
	if (target_value != level_data.end()) {
		if (!(bool)level_data["offset_positions"]) {
			level->offset_positions = false;
		}
	}

	current_level_data = level_data;

	for (auto& el : level_data["objects"].items()) {
		properties = el.value();
		string type = properties["type"];

		if (type == "COLLIDABLE") {
			level->objects.push_back(parse_collidable());
		} else if (type == "PANEL") {
			level->objects.push_back(parse_panel());
		} else if (type == "STARS") {
			level->objects.push_back(parse_stars(q));
		} else if (type == "QUAD") {
			level->objects.push_back(parse_custom());
		} else if (type == "MOVEABLE") {
			level->objects.push_back(parse_moveable());
		} else if (type == "TEXT") {
			level->text_objects.push_back(parse_text());
		} else if (type == "BUTTON") {
			Moveable* button = parse_button();
			level->objects.push_back(button);
			if (button->text) {
				level->text_objects.push_back(button->text);
			}
		} else if (type == "SLIDER") {
			level->objects.push_back(parse_slider());
		} else if (type == "PROVINCE") {
			level->objects.push_back(parse_province());
		} else if (type == "NATION") {
			level->objects.push_back(parse_nation());
		} else if (type == "UNIT") {
			Unit* unit = parse_unit();
			level->objects.push_back(unit);
			level->text_objects.push_back(unit->get_text());
		} else {
			level->objects.push_back(parse_moveable());
		}
	}

	log_t("Finished parsing " CON_RED, level->objects.size(), CON_NORMAL " object(s).");
	level_file.close();
	return level;
}

void Loader::write_objects(vector<Moveable*> objects, vector<Text*> text_objects) {
	ifstream file("data/levels/debug/0.json");
	if (!file.is_open()) return;

	json level_data;
	file >> level_data;
	file.close();

	json delayed_additions = json::array();
	json objects_array = json::array();
	for (Moveable* m : objects) {
		if (m->has_flag(PANEL)) continue; // temp
		if (m->has_flag(UNSAVEABLE)) continue;
		if (m->has_flag(UI)) continue;
		if (m->get_name().find("Background") != string::npos) continue;
		json moveable_data = {
			{"type", "MOVEABLE"},
			{"x", m->location.x },
			{"y", m->location.y },
			{"width", m->size.x },
			{"height", m->size.y },
			{"colour", m->get_colour().get_hex() },
			{"alt_colour", m->get_gradient_colour().get_hex() },
			{"alpha", m->get_colour().get_w() / 255.0f},
			{"alt_alpha", m->get_gradient_colour().get_w() / 255.0f},
			{"name", m->get_name()},
			{"flags", m->get_flags() }
		};
		if (m->has_flag(BUTTON)) {
			moveable_data["type"] = "BUTTON";
			moveable_data["action"] = reinterpret_cast<Moveable*>(m)->get_button_action();

			Text* t = m->get_text();
			moveable_data["font"] = Fonts::get_properties(t->get_font()).first;
			moveable_data["font_custom"] = (int)(Fonts::get_properties(t->get_font()).second);
			moveable_data["size"] = t->get_font()->h;
			moveable_data["content"] = t->get_content();
			moveable_data["scale"] = t->get_scale();
		}
		// Note: in the level loader, units are pushed by -1 on the x-axis -- shouldn't be a problem if assigned to a province
		if (m->has_flag(TEXTURED)) {
			moveable_data["texture"] = m->get_texture()->path;
		}
		if (m->has_flag(PROVINCE)) {
			moveable_data["type"] = "PROVINCE";
			moveable_data["id"] = reinterpret_cast<Province*>(m)->get_id();
		}
		if (m->has_flag(UNIT)) {
			moveable_data["type"] = "UNIT";
			moveable_data["id"] = reinterpret_cast<Unit*>(m)->get_id();
			moveable_data["province"] = reinterpret_cast<Unit*>(m)->get_province()->get_id();
			moveable_data["skill"] = reinterpret_cast<Unit*>(m)->get_skill();
			moveable_data["size"] = reinterpret_cast<Unit*>(m)->get_amount();
		}
		if (m->has_flag(NATION)) {
			moveable_data["type"] = "NATION";
			moveable_data["id"] = reinterpret_cast<Nation*>(m)->get_id();
			moveable_data["capital"] = reinterpret_cast<Nation*>(m)->get_capital()->get_id();
			moveable_data["provinces"] = {};
			moveable_data["units"] = {};
			for (Unit* unit : reinterpret_cast<Nation*>(m)->get_army()) moveable_data["units"].push_back(unit->get_id());
			for (Province* province : reinterpret_cast<Nation*>(m)->get_provinces()) moveable_data["provinces"].push_back(province->get_id());
			delayed_additions.push_back(moveable_data);
			continue;
		}
		if (m->has_flag(COLLIDABLE)) moveable_data["type"] = "SLIDER";

		objects_array.push_back(moveable_data);
	}

	objects_array.insert(objects_array.end(), delayed_additions.begin(), delayed_additions.end());

	for (Text* t : text_objects) {
		if (t->has_flag(UI)) continue;
		if (t->has_flag(UNSAVEABLE)) continue;
		json moveable_data = {
			{"x", t->location.x },
			{"y", t->location.y },
			{"width", t->size.x },
			{"height", t->size.y },
			{"colour", t->get_colour().get_hex() },
			{"alpha", t->get_colour().get_w() / 255.0f},
			{"name", t->get_name()}
		};
		if (t->has_flag(TEXT)) {
			moveable_data["type"] = "TEXT";
			moveable_data["font"] = Fonts::get_properties(t->get_font()).first;
			moveable_data["font_custom"] = (int)(Fonts::get_properties(t->get_font()).second);
			moveable_data["size"] = t->get_font()->h;
			moveable_data["content"] = t->get_content();
			moveable_data["scale"] = t->get_scale();
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