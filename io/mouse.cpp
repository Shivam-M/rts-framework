#include "mouse.h"
#include "../game_rts.h" // Change to game.h to make this agnostic

Game* Mouse::game = nullptr;
Mouse::Mouse(Game* instance) { Mouse::game = instance; }

static bool within(Vector2 location, Vector2 size, Vector2 point) {
	return point.x > location.x && point.x < location.x + size.x && point.y > location.y && point.y < location.y + size.y;
}

void Mouse::scroll_callback(GLFWwindow* window, double x, double y) {
	Render* render = &game->render;
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
	game->setButton(button, action);

	if (debug_control_scheme) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			if (game->selected_object) {
				game->selected_object->setLocation(game->cursor_position.x, game->cursor_position.y);
				game->t_Notification.setContent("Set location of " + game->selected_object->getName() + " to " + to_string(game->cursor_position.x) + ", " + to_string(game->cursor_position.y));
				log_t("Set location of " + game->selected_object->getName() + " to " + to_string(game->cursor_position.x) + ", " + to_string(game->cursor_position.y));
			}
		} else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
			game->original_position = game->cursor_position;
			game->selected_object = game->getObjectUnderMouse(); // hovered_object;
			game->t_Notification.setContent("");

			if (game->selected_object) {
				game->t_Notification.setContent("Selected " + game->selected_object->getName());
				log_t("Selected " + game->selected_object->getName());
			}
		} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			game->mouse_position = GLFW_PRESS ? game->cursor_position : 0;
		}
	} else { // RTS
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (action == GLFW_PRESS) {
				game->original_position = game->cursor_position;
				game->selected_object = nullptr;
				game->t_Notification.setContent("");
				game->selected_object = game->hovered_object;

				if (game->selected_object) {
					game->t_Notification.setContent("Selected " + game->selected_object->getName());
					log_t("Selected " + game->selected_object->getName());
				}
			}
		} else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			Moveable* moveable = game->getObjectUnderMouse();
			if (moveable == nullptr) return;
			if (game->selected_object) {
				if (game->selected_object->getFlags() & UNIT) {
					if (moveable->getFlags() & PROVINCE) {
						reinterpret_cast<GameRTS*>(game)->moveUnit(reinterpret_cast<Province*>(moveable));
					}
				} else {
					if (moveable->getFlags() & PROVINCE) {
						reinterpret_cast<GameRTS*>(game)->expandNation(reinterpret_cast<Province*>(moveable));
					}
				}
			}
			else {
				if (moveable->getFlags() & PROVINCE) {
					reinterpret_cast<GameRTS*>(game)->expandNation(reinterpret_cast<Province*>(moveable));
				}
			}
		} else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
			game->original_position = game->cursor_position;
		}
	}
}
