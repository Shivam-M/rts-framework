#include "mouse_new.h"
#include <cmath>

GameNew* MouseNew::game = nullptr;
MouseNew::MouseNew(GameNew* instance) { MouseNew::game = instance; }

static bool within(Vector2 location, Vector2 size, Vector2 point) {
	return point.x > location.x && point.x < location.x + size.x && point.y > location.y && point.y < location.y + size.y;
}

void MouseNew::scroll_callback(GLFWwindow* window, double x, double y) {
	game->render.scale += 0.05 * y;
}

void MouseNew::callback(GLFWwindow* window, int button, int action, int mods) {
	double x, y;
	Vector2 resolution = game->render.resolution;
	Vector2 cursor;

	glfwGetCursorPos(window, &x, &y);
	cursor.set(x / resolution.x, y / resolution.y);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (game->selected_object) {
			game->selected_object->setLocation(cursor.x, cursor.y);
			game->t_Notification.setContent("Set location of " + game->selected_object->getName() + " to " + to_string(cursor.x) + ", " + to_string(cursor.y));
			info(game->ss << "Set location of " + game->selected_object->getName() + " to " + to_string(cursor.x) + ", " + to_string(cursor.y));
		}
	} else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (action == GLFW_PRESS) {
			game->original_position = cursor;
			game->mbDown = true;
			game->selected_object = nullptr;
			game->t_Notification.setContent("");

			vector<MoveableNew*> over_objects;
			float min_distance = 100;

			for (MoveableNew* moveable: game->objects)
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
				info(game->ss << "Selected " + game->selected_object->getName());
			}

		}
		else if (action == GLFW_RELEASE) {
			game->mbDown = false;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			game->mouse_position.x = cursor.x, game->mouse_position.y = cursor.y;
			game->rbDown = true;
		}
		else if (game->selected_object) {
			game->rbDown = false;
			game->mouse_position.x = -1, game->mouse_position.y = -1;
		}
	}
}
