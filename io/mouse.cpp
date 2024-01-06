//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include "../game.h"
#include "mouse.h"


Game* Mouse::game = nullptr;
Mouse::Mouse() {}

Mouse::Mouse(Game* g) {
	this->game = g;
}

void Mouse::scroll_callback(GLFWwindow* window, double x, double y) {
	game->render.scale += 0.05 * y;
	return;

	// Experimental pointer zoom
	double mouseX, mouseY, relX, relY;
	int dimensions[2] = { 1280, 720 };
	glfwGetWindowSize(game->window, &dimensions[0], &dimensions[1]);
	glfwGetCursorPos(window, &mouseX, &mouseY);

	relX = (2.0 * mouseX - dimensions[0]) / dimensions[0];
	relY = (2.0 * mouseY - dimensions[1]) / dimensions[1];

	double original_scale = game->render.scale;
	game->render.scale += 0.05 * y;

	double scale_diff = game->render.scale - original_scale;
	game->render.offsets[0] -= relX * scale_diff;
	game->render.offsets[1] -= relY * scale_diff;
}

void Mouse::callback(GLFWwindow* window, int button, int action, int mods) {
	double x, y, relx, rely;

	int dimensions[2] = { 1280, 720 };
	glfwGetWindowSize(game->window, &dimensions[0], &dimensions[1]);
	glfwGetCursorPos(window, &x, &y);
	relx = x / dimensions[0], rely = y / dimensions[1];
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (game->selected_object) game->selected_object->setLocation(relx, rely);
		if (game->selected_button) game->selected_button->setColour(231, 76, 60, 255);
		if (game->selected_object) {
			game->t_Notification.setContent("Set location of " + game->selected_object->getName() + " to " + to_string(relx) + ", " + to_string(rely));

			stringstream ss;
			info(ss << "Set location of " + game->selected_object->getName() + " to " + to_string(relx) + ", " + to_string(rely));
		}
	} else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (action == GLFW_PRESS) {
			game->original_position[0] = relx, game->original_position[1] = rely;
			game->mbDown = true;
			game->selected_object = nullptr;
			game->t_Notification.setContent("");

			vector<Moveable*> over_objects;

			Moveable* selected_object = nullptr;
			double min_distance = 999;

			for (Moveable* m : game->objects)
				if (!(m->getFlags() & UNEDITABLE || m->getFlags() & FIXED_POS)) {
					double* location = m->getLocation();
					double* size = m->getSize();
					if (game->isWithin(location[0], location[1], location[0] + size[0], location[1] + size[1], relx, rely)) {
						double center_x = location[0] + size[0] / 2.0;
						double center_y = location[1] + size[1] / 2.0;
						double distance = sqrt((center_x - relx) * (center_x - relx) + (center_y - rely) * (center_y - rely));

						if (distance < min_distance) {
							min_distance = distance;
							game->selected_object = m;
						}
					}
				}

			if (game->selected_object) {
				game->t_Notification.setContent("Selected " + game->selected_object->getName());
				stringstream ss;
				info(ss << "Selected " + game->selected_object->getName());
			}

		} else if (action == GLFW_RELEASE) {
			game->mbDown = false;
		}
	} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			game->mouse_position[0] = relx, game->mouse_position[1] = rely;
			game->rbDown = true;
		} else if (game->selected_object) {
			game->rbDown = false;
			game->mouse_position[0] = -1, game->mouse_position[1] = -1;
		}
	}
}

void secondary_controls(GLFWwindow* window, int button, int action, int mods) {

}