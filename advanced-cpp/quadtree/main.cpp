#include "unistd.h"

#include "sim/shapes.hpp"
#include "sim/window.hpp"
#include "sim/quad.hpp"

#include "ncurses.h"

#include <vector>
#include <iostream>

int main(int argc, char *argv[]) {	
	sim::Window main_window;
	std::vector<sim::Rectangle> items;
	sim::Rectangle screen_bound(0, 0, main_window.width() - 2, main_window.height() - 2, sim::Color::GREEN);
	sim::Rectangle *selected = nullptr;
	sim::EventData mde;	// most recent mouse down location
	int manual = 0;
	bool edit_mode = false;

	/**
	 * This is the main loop of the entire program. Appologies this section is pretty messy
	 */ 
	while (1) {
		sim::EventData e;
		sim::Event event = main_window.event(e);
		
		// Screen will only be updated if an event is received
		if (event != sim::Event::NONE) {
			clear();
			switch (event) {
				case sim::Event::MOUSE_DOWN: {
					// check through all of the items and see if there is one that collides with the mouse click
					for (int i = 0; i < items.size(); i++) {
						if (items[i].is_intersecting(sim::Rectangle(e.x, e.y, 1, 1))) {
							items[i].set_color(sim::Color::RED);
							selected = &items[i];
							edit_mode = true;
							break;
						}
					}

					mde = e;
					break;
				}
				case sim::Event::MOUSE_UP:
					if (!edit_mode) {
						auto width = e.x - mde.x; 
						auto height = e.y - mde.y;
						// create an item if the item has the proper dimensions, this disables dragging up and to left to create items
						if (width > 0 && height > 0) {
							items.push_back(sim::Rectangle(mde.x, mde.y, e.x - mde.x, e.y - mde.y, sim::Color::BLUE));
						}
					}

					break;
				case sim::Event::ESCAPE:	// deselect selected items
					if (edit_mode && selected != nullptr) {
						selected->set_color(sim::Color::BLUE);
						edit_mode = false;
						selected = nullptr;
					}
					break;

				// The remainder of the items in the switch are for controlling the movement of objects			
				case sim::Event::KEY_DOWN_ARROW: {
					if (selected != nullptr && (selected->y() + selected->height()) < screen_bound.height()) {
						selected->set_position(selected->x(), selected->y() + 1);
					}
					break;
				}
				case sim::Event::KEY_UP_ARROW: {
					if (selected != nullptr && selected->y() > screen_bound.y()) {
						selected->set_position(selected->x(), selected->y() - 1);
					}
					break;
				}
				case sim::Event::KEY_LEFT_ARROW: {
					if (selected != nullptr && selected->x() > screen_bound.x()) {
						selected->set_position(selected->x() - 1, selected->y());
					}
					break;
				}
				case sim::Event::KEY_RIGHT_ARROW: {
					if (selected != nullptr && (selected->x() + selected->width()) < screen_bound.width()) {
						selected->set_position(selected->x() + 1, selected->y());
					}
					break;
				}
				case sim::Event::QUIT: {
					endwin();
					exit(EXIT_SUCCESS);
				}
				default:
					break;
			}

			// insert all of the items from the list into the quadtree
			sim::QuadTree <sim::Rectangle*, 5, 5> tree(screen_bound);
			for (int i = 0; i < items.size(); i++) {
				try {
					tree.insert(&items[i]);
				} catch (sim::full_tree_error e) {
					printw("\nThis branch is maximally subdivided, cannot add item");
					items.pop_back();
				}
			}
			
			// loop through all items in the list and query the quad tree for possible collisions
			for (int i = 0; i < items.size(); i++) {
				std::vector<sim::Rectangle*> matches;
				auto item = items[i];
				tree.search(&item, matches);
				bool is_colliding = false;

				for (auto rect : matches) {
					if (item.is_intersecting(*rect)) {
						if (*rect == item) continue;
						is_colliding = true;
					} 
				}

				// if items are colliding paint them yellow
				if (is_colliding && &items[i] != selected) {
					item.set_color(sim::Color::YELLOW);
				} else if (&items[i] != selected) {
					item.set_color(sim::Color::BLUE);
				}
				item.draw();
			}

			// always draw selected on top of other boxes
			if (selected != nullptr) selected->draw();

			tree.draw();
			main_window.update();
		}
	}
}
