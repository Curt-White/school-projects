#ifndef WINDOW_HPP
#define WINDOW_HPP

namespace sim {
	enum Color {
		GREEN = 1,
		BLUE = 2,
		RED = 3,
		BLACK = 4,
		YELLOW = 5,
	};

	enum Event {
		MOUSE_UP = 0,
		MOUSE_DOWN,
		MOUSE_CLICK,
		KEY_LEFT_ARROW,
		KEY_UP_ARROW,
		KEY_RIGHT_ARROW,
		KEY_DOWN_ARROW,
		ESCAPE,
		BACKSPACE,
		ENTER,
		QUIT,
		NONE
	};

	struct EventData {
		int x, y;
	};

	class Window {
		private:
			int height_;
			int width_;
		public:
			Window();
			~Window();

			Event event(EventData &e) const;
			int height() const;
			void update() const;
			int width() const;
	};
}

#endif
