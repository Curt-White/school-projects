#ifndef SHAPES_HPP
#define SHAPES_HPP

#include "sim/window.hpp"
#include <ostream>

namespace sim {
	class Rectangle;

	class Shape {
		protected:
			int x_, y_;
			sim::Color c_;
		public:
			Shape(int x, int y, sim::Color c = sim::Color::BLACK);
			virtual void draw() const = 0;
			virtual void draw_border() const = 0;

			void set_color(sim::Color c);
			void set_position(int x, int y) { x_ = x; y_ = y; }
			sim::Color color() const;

			virtual bool is_intersecting(const sim::Rectangle &r) const = 0;
			virtual bool is_containing(const sim::Rectangle &r) const = 0;

			int x() const;
			int y() const;
	};

	class Rectangle: public Shape {
		private:
			int width_;
			int height_;
		public:
			Rectangle(int x = 1, int y = 1, int width = 1, int height = 1, sim::Color c = sim::Color::BLACK);
			void draw() const;
			void draw_border() const;

			int width() const;
			int height() const;

			// if two Rectangles are just colliding at all
			bool is_intersecting(const sim::Rectangle &r) const;
			//  if r is completely contained within this then true else false. Items cannot share borders
			bool is_containing(const sim::Rectangle &r) const;
			
			bool operator==(const Rectangle& lhs) const;
			friend std::ostream& operator<<(std::ostream &stream, const sim::Rectangle &r);
	};

	std::ostream& operator<<(std::ostream &stream, const sim::Rectangle &r);
}

#endif
