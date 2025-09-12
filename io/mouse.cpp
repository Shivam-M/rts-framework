#include "mouse.h"

#include "../tools/common.h"
#include "../tools/render.h"
#include "../assets/text.h"
#include "../game_rts.h" // Change to game.h to make this agnostic

Game* Mouse::game = nullptr;
Mouse::Mouse(Game* instance) { Mouse::game = instance; }

void Mouse::scroll_callback(GLFWwindow* window, double x, double y) {
	Render* render = game->render;
	Vector2 offsets = render->offsets;
	Vector2 cursor = game->cursor_position;
	float scale = render->scale;
	float new_scale = scale + 0.125f * y;

	if (new_scale < 1.0f) {
		new_scale = 1.0f;
		render->scale = new_scale;
		render->offsets = Vector2(0.0f, 0.0f);
	} else {
		render->offsets.x = cursor.x - (cursor.x - offsets.x) * (new_scale / scale);
		render->offsets.y = cursor.y - (cursor.y - offsets.y) * (new_scale / scale);
		render->scale = new_scale;
	}
}

void Mouse::callback(GLFWwindow* window, int button, int action, int mods) {
	game->set_button(button, action);

	if (debug_control_scheme) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			if (game->selected_object) {
				game->selected_object->set_location(game->cursor_position.x, game->cursor_position.y);
				game->t_Notification->set_content("Set location of " + game->selected_object->get_name() + " to " + to_string(game->cursor_position.x) + ", " + to_string(game->cursor_position.y));
				log_t("Set location of " + game->selected_object->get_name() + " to " + to_string(game->cursor_position.x) + ", " + to_string(game->cursor_position.y));
			}
		} else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
			game->original_position = game->cursor_position;
			game->selected_object = game->get_object_under_mouse(); // hovered_object;
			game->t_Notification->set_content("");

			if (game->selected_object) {
				game->t_Notification->set_content("Selected " + game->selected_object->get_name());
				log_t("Selected " CON_RED + game->selected_object->get_name());
			}
		} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			game->mouse_position = GLFW_PRESS ? game->cursor_position : 0;
		}
	} else { // RTS
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			game->original_position = game->cursor_position;
			game->selected_object = nullptr;
			game->t_Notification->set_content("");
			game->selected_object = game->hovered_object;

			if (game->selected_object) {
				game->t_Notification->set_content("Selected " + game->selected_object->get_name());
				log_t("Selected " CON_RED + game->selected_object->get_name());
			}
		} else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			Moveable* moveable = game->get_object_under_mouse();
			if (moveable == nullptr) return;
			if (game->selected_object) {
				if (game->selected_object->has_flag(UNIT)) {
					if (moveable->has_flag(PROVINCE)) {
						reinterpret_cast<GameRTS*>(game)->move_unit(reinterpret_cast<Province*>(moveable));
					}
				} else {
					if (moveable->has_flag(PROVINCE)) {
						reinterpret_cast<GameRTS*>(game)->expand_nation(reinterpret_cast<Province*>(moveable));
					}
				}
			}
			else {
				if (moveable->has_flag(PROVINCE)) {
					reinterpret_cast<GameRTS*>(game)->expand_nation(reinterpret_cast<Province*>(moveable));
				}
			}
		} else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
			game->original_position = game->cursor_position;
		}
	}
}
