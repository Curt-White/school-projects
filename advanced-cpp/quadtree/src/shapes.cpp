#include "ncurses.h"
#include <ostream>

#include "sim/shapes.hpp"

/******************************************
* Shape Object Base Class 
******************************************/

sim::Shape::Shape(int x, int y, sim::Color c): x_(x), y_(y), c_(c) {}

void sim::Shape::set_color(sim::Color c) {
	c_ = c;
}

int sim::Shape::x() const { return x_; }

int sim::Shape::y() const { return y_; }

sim::Color sim::Shape::color() const { return c_; }

/******************************************
* Rectange Shape Object
******************************************/

sim::Rectangle::Rectangle(int x, int y, int width, int height, sim::Color c):
	Shape(x, y, c), width_(width <= 0 ? 1 : width), height_(height <= 0 ? 1 : height) {}

// This draw function is implemented specifically for drawing with ncurses
void sim::Rectangle::draw() const {
	attron(COLOR_PAIR(c_));

	for (int x = x_; x < x_ + width_; x++) {
		for (int y = y_; y < y_ + height_; y++) {
			mvprintw(y, x, " ");
		}
	}

	attroff(COLOR_PAIR(c_));
}

int sim::Rectangle::width() const { return width_; }

int sim::Rectangle::height() const { return height_; }

void sim::Rectangle::draw_border() const {
	attron(COLOR_PAIR(c_));

	for (int x = x_; x < x_ + width_; x++) {
		for (int y = y_; y < y_ + height_; y++) {
			if (x == x_ || (x == x_ + width_ - 1) || y == y_ || (y == y_ + height_ - 1)) {
				mvprintw(y, x, " ");
			}
		}
	}

	attroff(COLOR_PAIR(c_));
}

bool sim::Rectangle::is_intersecting(const sim::Rectangle &r) const {
	if ((x_ < r.x_ + r.width_) && (x_ + width_ > r.x_) && (y_ < r.y_ + r.height_) && (y_ + height_ > r.y_)) {
		return true;
	}
		
	return false;
}

bool sim::Rectangle::is_containing(const sim::Rectangle &r) const {
	if (x_ < r.x_ && (r.x_ + r.width_) < (x_ + width_)
		&& y_ < r.y_ && (r.y_ + r.height_) < (y_ + height_)) {

		return true;
	}

	return false;
}

bool sim::Rectangle::operator==(const sim::Rectangle& lhs) const {
	return (x_ == lhs.x_ && y_ == lhs.y_ && width_ == lhs.width_ && height_ == lhs.height_);
}

std::ostream& sim::operator<<(std::ostream &stream, const sim::Rectangle &r) {
	stream << "x: " << r.x_ << ", y: " << r.y_ << " width: " << r.width_ << " height: " << r.height_;
	return stream;
}
