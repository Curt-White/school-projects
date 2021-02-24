#include "ncurses.h"
#include <csignal>

#include "sim/window.hpp"

namespace sim {
	Window::Window() {
		initscr(); // Initialize the main window
		start_color();

		init_pair(GREEN, COLOR_BLACK, COLOR_GREEN);
		init_pair(BLUE, COLOR_BLACK, COLOR_BLUE);
		init_pair(RED, COLOR_BLACK, COLOR_RED);
		init_pair(BLACK, COLOR_BLACK, COLOR_BLACK);
		init_pair(YELLOW, COLOR_BLACK, COLOR_YELLOW);

		noecho();
		curs_set(FALSE); // hide the cursor

		printw("Welcome to the collision simulator. Use Shift-Q to quit.");
		refresh();

		getmaxyx(stdscr, height_, width_);
		keypad(stdscr, TRUE);
		mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
	}

	Window::~Window() {
		endwin();
	}

	Event Window::event(EventData &e) const {
		int c = wgetch(stdscr);

		switch (c) {
			case KEY_MOUSE:
				MEVENT me;
				if (getmouse(&me) == OK) {
					e.x = me.x;
					e.y = me.y;

					switch (me.bstate) {
						case 1: return MOUSE_UP;
						case 2: return MOUSE_DOWN;
						case 4: return MOUSE_CLICK;
					}
				}

				break;
			case KEY_LEFT: return KEY_LEFT_ARROW;
			case KEY_RIGHT: return KEY_RIGHT_ARROW;
			case KEY_UP: return KEY_UP_ARROW;
			case KEY_DOWN: return KEY_DOWN_ARROW;
			case KEY_BACKSPACE: return BACKSPACE;
			case KEY_ENTER: return ENTER;
			case 27: return ESCAPE;
			case 81: return QUIT;
		}

		return NONE;
	}

	int Window::height() const {
		return height_;
	}

	void Window::update() const {
		refresh();
	}

	int Window::width() const {
		return width_;
	}

}
