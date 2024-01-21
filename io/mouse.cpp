#include "mouse.h"
#include <cmath>

Game* Mouse::game = nullptr;
Mouse::Mouse(Game* instance) { Mouse::game = instance; }

static bool within(Vector2 location, Vector2 size, Vector2 point) {
	return point.x > location.x && point.x < location.x + size.x && point.y > location.y && point.y < location.y + size.y;
}

void Mouse::scroll_callback(GLFWwindow* window, double x, double y) {
	game->render.scale += 0.05 * y;
}

void Mouse::callback(GLFWwindow* window, int button, int action, int mods) {
	game->setButton(button, action);
	Vector2 resolution = game->render.resolution;
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	Vector2 cursor(x / game->render.resolution.x, y / game->render.resolution.y);

	if (debug_control_scheme) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			if (game->selected_object) {
				game->selected_object->setLocation(cursor.x, cursor.y);
				game->t_Notification.setContent("Set location of " + game->selected_object->getName() + " to " + to_string(cursor.x) + ", " + to_string(cursor.y));
				log_t("Set location of " + game->selected_object->getName() + " to " + to_string(cursor.x) + ", " + to_string(cursor.y));
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			if (action == GLFW_PRESS) {
				game->original_position = cursor;
				game->selected_object = nullptr;
				game->t_Notification.setContent("");

				vector<Moveable*> over_objects;
				float min_distance = 100;
				for (Moveable* moveable : game->objects)
					if (!(moveable->getFlags() & UNEDITABLE || moveable->getFlags() & FIXED_POS)) {
						Vector2 location = moveable->getLocation(), size = moveable->getSize();
						if (within(location, size, cursor)) {
							Vector2 centre = moveable->getCentre();
							float distance = sqrt(pow(centre.x - cursor.x, 2) + pow(centre.y - cursor.y, 2));
							if (distance < min_distance) {
								min_distance = distance;
								game->selected_object = moveable;
							}
						}
					}

				if (game->selected_object) {
					game->t_Notification.setContent("Selected " + game->selected_object->getName());
					log_t("Selected " + game->selected_object->getName());
				}
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			game->mouse_position = GLFW_PRESS ? cursor : 0;
		}
	}
	else {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (action == GLFW_PRESS) {
				game->original_position = cursor;
				game->selected_object = nullptr;
				game->t_Notification.setContent("");
				game->selected_object = game->getObjectUnderMouse();

				if (game->selected_object) {
					game->t_Notification.setContent("Selected " + game->selected_object->getName());
					log_t("Selected " + game->selected_object->getName());
				}
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			Moveable* moveable = game->getObjectUnderMouse();
			if (moveable == nullptr) return;
			if (game->selected_object) {
				if (game->selected_object->getFlags() & UNIT) {
					if (moveable->getFlags() & PROVINCE) {
						game->moveUnit(reinterpret_cast<Province*>(moveable));
					}
				}
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
			if (action == GLFW_PRESS) {
				game->original_position = cursor;
			}
		}
	}
}
