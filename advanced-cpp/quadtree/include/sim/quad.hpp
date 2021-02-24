#ifndef QUAD_HPP
#define QUAD_HPP

#include "boost/iterator/iterator_facade.hpp"

#include "sim/shapes.hpp"
#include <new>
#include <iostream>
#include <vector>

namespace sim {
	template <class T, int D = 5, int I = 5>
	class QuadTree;
 
	template <class T, int D, int I>	
	std::ostream& operator<<(std::ostream& stream, const QuadTree<T, D, I>& tree);

	struct full_tree_error : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	/**
	 *	T - the type being held in the tree
	 *  D - is the maximum depth of the tree
	 *  I - is the maximum number of items in a quadrant before splitting
	 * 
	 * This template should only be instantiated with items that are derivatives of the
	 * shape class because it is necessary that they have the collision checking functions.
	*/
	template <class T, int D, int I>
	class QuadTree {
		private:
			std::vector<T> items_;
			sim::Rectangle bounds_;
			int depth_; // this is the level of the current quadtree (how deeply nested)

			QuadTree *parent_;
			QuadTree *top_right_child_;
			QuadTree *top_left_child_;
			QuadTree *bottom_left_child_;
			QuadTree *bottom_right_child_;

			void subdivide();
		public:
			enum struct Quadrant : int {
				TOP_RIGHT,
				TOP_LEFT,
				BOTTOM_LEFT,
				BOTTOM_RIGHT,
				NONE
			};

			/**
			 *  This is the quad tree iterator which is a relatively complicated piece of code which loops through
			 *  the quad tree recursively visiting the TR, TL, BL, BR quadrants of each subtree. It is only a forward iterator.
			 */ 
			template <class L>
			class quad_tree_iterator: public boost::iterator_facade<quad_tree_iterator<L>, L, boost::forward_traversal_tag> {
				public:
					using base = boost::iterator_facade<quad_tree_iterator<L>, L, boost::forward_traversal_tag>;
					using typename base::reference;
					using typename base::value_type;

					template <class OtherT, class = std::enable_if_t<std::is_convertible_v<OtherT*, L*>>>
					quad_tree_iterator (const quad_tree_iterator<OtherT>& other): tree_(other.tree_), total_len_(other.total_len_), counter_(other.counter_) {
							for (int i = 0; i < D; i++) { iter_index_.quad[i] = other.iter_index_.quad[i]; }
							iter_index_.item = other.iter_index_.item;
							iter_index_.level = other.iter_index_.level;
					}
					quad_tree_iterator (QuadTree<T, D, I>* tree): tree_(tree), iter_index_{ { Quadrant::NONE }, 0, 0 }, total_len_(tree->size()), counter_(0) {
						std::fill_n(&iter_index_.quad[0], D, Quadrant::NONE);
					}

				private:
					template <class E, int F, int G> friend class QuadTree;
					friend class boost::iterator_core_access;
					template <class OtherT> friend class quad_tree_iterator;

					template <class OtherT> bool equal(const quad_tree_iterator<OtherT>& other) const { 
						return (other.counter_ == counter_);
					}

					reference dereference() const { 
						return tree_->items_[iter_index_.item];
					}

					void increment() {
						// if there is still items in the current tree
						++iter_index_.item;
						++counter_;
						if (iter_index_.item < tree_->size_level()) {
							return;
						}

						iter_index_.item = 0;
						do {
							// if current level has been iterated
							switch (iter_index_.quad[iter_index_.level]) {
								case Quadrant::NONE:
									if (tree_->get_top_right() == nullptr) {
										if (iter_index_.level == 0) { 
											iter_index_.item = 0;
										} else {
											--iter_index_.level;
											tree_ = tree_->get_parent();
											iter_index_.item = tree_->size_level(); // since in BR subtree, must have seen all items already
										}
									} else {
										iter_index_.quad[iter_index_.level] = Quadrant::TOP_RIGHT;
										tree_ = tree_->get_top_right();
										++iter_index_.level;
										iter_index_.item = 0;
									}

									break;
								case Quadrant::TOP_RIGHT:
									iter_index_.quad[iter_index_.level] = Quadrant::TOP_LEFT;
									tree_ = tree_->get_top_left();
									++iter_index_.level;
									iter_index_.item = 0;
									break;

								case Quadrant::TOP_LEFT:
									iter_index_.quad[iter_index_.level] = Quadrant::BOTTOM_LEFT;
									tree_ = tree_->get_bottom_left();
									++iter_index_.level;
									iter_index_.item = 0;
									break;

								case Quadrant::BOTTOM_LEFT:
									iter_index_.quad[iter_index_.level] = Quadrant::BOTTOM_RIGHT;
									tree_ = tree_->get_bottom_right();
									++iter_index_.level;
									iter_index_.item = 0;
									break;

								case Quadrant::BOTTOM_RIGHT:
									if (iter_index_.level == 0) {
										// uncommenting the below allows the iterator to become circular
										// std::fill_n(&iter_index_.quad[0], D, Quadrant::NONE);
										// iter_index_.item = 0;
										return;
									}

									--iter_index_.level;
									tree_ = tree_->get_parent();
									iter_index_.item = tree_->size_level(); // since in BR subtree, must have seen all items already

									break;
							}
						// loop until the tree has atleast a single item in it
						} while (tree_->size_level() == 0 || iter_index_.item == tree_->size_level());
					}

					struct index {
						Quadrant quad[D];
						int item;
						int level;
					};

					int total_len_, counter_;
					index iter_index_;
					QuadTree<T, D, I>* tree_;
			};

			using iterator = quad_tree_iterator<T>;
			using const_iterator = quad_tree_iterator<const T>;

			QuadTree(const sim::Rectangle& bounds, int depth = 1, QuadTree<T, D, I>* parent = nullptr);
			QuadTree(QuadTree<T, D, I>&& tree) = delete;	// temporarily delete the move/copy constructors
			QuadTree(const QuadTree<T, D, I>& tree) = delete;
			~QuadTree();

			iterator begin() { return iterator(this); }
			const_iterator begin() const { return const_iterator(this); }
			
			iterator end() { auto it = iterator(this); it.counter_ = this->size(); return it; }
			const_iterator end() const { auto it = const_iterator(this); it.counter_ = this->size(); return it; }

			void search(const T& bound, std::vector<T>& list) const;

			int size() const;
			int size_level() const { return items_.size(); }
			int level() const { return this->top_left_child_ != nullptr ? this->top_left_child_->level() : this->depth_; }
			int max_level() const { return D; }
			int max_items_per_level() const { return I; }

			const sim::Rectangle& bound() const { return bounds_; }

			const QuadTree<T, D, I>* get_parent() const { return parent_; }
			const QuadTree<T, D, I>* get_bottom_left() const { return bottom_left_child_; }
			const QuadTree<T, D, I>* get_bottom_right() const { return bottom_right_child_; }
			const QuadTree<T, D, I>* get_top_left() const { return top_left_child_; }
			const QuadTree<T, D, I>* get_top_right() const { return top_right_child_; }

			QuadTree<T, D, I>* get_parent() { return parent_; }
			QuadTree<T, D, I>* get_bottom_left() { return bottom_left_child_; }
			QuadTree<T, D, I>* get_bottom_right() { return bottom_right_child_; }
			QuadTree<T, D, I>* get_top_left() { return top_left_child_; }
			QuadTree<T, D, I>* get_top_right() { return top_right_child_; }

			Quadrant find_quadrant(const T s) const;
			void insert(const T item);

			// draw all of the boxes which are part of the quad tree
			void draw() const;
			friend std::ostream& operator<< <>(std::ostream& stream, const QuadTree<T, D, I>& tree);
	};

	/******************************************
	* Implementation
	******************************************/
	template <class T, int D, int I>
	QuadTree<T, D, I>::QuadTree(const sim::Rectangle& bounds, int depth, QuadTree<T, D, I>* parent):
		top_left_child_(nullptr), top_right_child_(nullptr), bottom_left_child_(nullptr),
		bottom_right_child_(nullptr), bounds_(bounds), depth_(depth), parent_(parent) {}

	template <class T, int D, int I>
	QuadTree<T, D, I>::~QuadTree() {
		delete top_left_child_;
		delete top_right_child_;
		delete bottom_left_child_;
		delete bottom_right_child_;
	}

	template <class T, int D, int I>
	std::ostream& operator<<(std::ostream& stream, const QuadTree<T, D, I>& tree) {
		auto space = std::string(tree.depth_, '	');

		// print out basic information about this tree
		stream << space << "Level: " << tree.depth_ << "\n" << space << "Total Items: " << tree.items_.size() << "\n" << space << "Bounds: " << tree.bounds_ << "\n" << space << "Items: ";
		for (int i = 0; i < tree.items_.size(); i++) stream << *tree.items_[i] << (i != tree.items_.size() - 1 ? " | " : "");
		if (tree.items_.size() == 0) stream << "None";
		stream << std::endl;

		// recursively call the function on all of the sub-trees to show all data in the tree
		if (tree.top_left_child_ != nullptr) stream << space <<  "Top Left: " << std::endl <<  *tree.top_left_child_;
		if (tree.top_right_child_ != nullptr) stream << space << "Top Right: " << std::endl << *tree.top_right_child_;
		if (tree.bottom_left_child_ != nullptr) stream << space << "Bottom Left: " << std::endl << *tree.bottom_left_child_;
		if (tree.bottom_right_child_ != nullptr) stream << space << "Bottom Right: " << std::endl << *tree.bottom_right_child_;

		return stream;
	}

	template <class T, int D, int I>
	int sim::QuadTree<T, D, I>::size() const {
		int total_size = items_.size();

		if (top_left_child_ != nullptr) {
			total_size += top_left_child_->size();
			total_size += top_right_child_->size();
			total_size += bottom_left_child_->size();
			total_size += bottom_right_child_->size();
		}

		return total_size;
	}

	template <class T, int D, int I>
	void QuadTree<T, D, I>::search(const T& bound, std::vector<T>& list) const {
		for (auto item : items_) list.push_back(item);

		// If there are some sub-tree items
		if (top_left_child_ != nullptr) {
			auto quad = find_quadrant(bound);
			switch (quad) {
				case Quadrant::TOP_RIGHT: this->top_right_child_->search(bound, list); break;
				case Quadrant::TOP_LEFT: this->top_left_child_->search(bound, list); break;
				case Quadrant::BOTTOM_LEFT: this->bottom_left_child_->search(bound, list); break;
				case Quadrant::BOTTOM_RIGHT: this->bottom_right_child_->search(bound, list); break;
				case Quadrant::NONE: {
					this->top_right_child_->search(bound, list);
					this->top_left_child_->search(bound, list);
					this->bottom_left_child_->search(bound, list);
					this->bottom_right_child_->search(bound, list);
				} break;
			}
		}
	}

	template <class T, int D, int I>
	typename QuadTree<T, D, I>::Quadrant QuadTree<T, D, I>::find_quadrant(const T s) const {
		if (top_right_child_ == nullptr) return Quadrant::NONE;
		
		bool is = top_right_child_->bound().is_containing(*s);
		if (is) return Quadrant::TOP_RIGHT;

		is = top_left_child_->bound().is_containing(*s);
		if (is) return Quadrant::TOP_LEFT;

		is = bottom_left_child_->bound().is_containing(*s);
		if (is) return Quadrant::BOTTOM_LEFT;

		is = bottom_right_child_->bound().is_containing(*s);
		if (is) return Quadrant::BOTTOM_RIGHT;

		// this case will return even if the item is inside but its borders are on the containers
		return Quadrant::NONE;
	}

	/**
	 *  subdivide the current quadtree into four more subtrees. Each of the subtrees is assigned thee correct
	 *  sizes. Before this function is called, all of a tree's subtrees are null.
	 */ 
	template <class T, int D, int I>
	void QuadTree<T, D, I>::subdivide() {
		int hw = bounds_.width() / 2;
		int hh = bounds_.height() / 2;
	
		top_right_child_ = new QuadTree<T, D, I> (
			sim::Rectangle(bounds_.x() + hw, bounds_.y(), (bounds_.width() - hw), hh + 1, bounds_.color()), depth_ + 1, this);

		top_left_child_ = new QuadTree<T, D, I> (
			sim::Rectangle(bounds_.x(), bounds_.y(), hw + 1, hh + 1, bounds_.color()), depth_ + 1, this);

		bottom_right_child_ = new QuadTree<T, D, I> (
			sim::Rectangle(bounds_.x() + hw, bounds_.y() + hh, (bounds_.width() - hw), (bounds_.height() - hh), bounds_.color()), depth_ + 1, this);

		bottom_left_child_ = new QuadTree<T, D, I> (
			sim::Rectangle(bounds_.x(), bounds_.y() + hh, hw + 1, (bounds_.height() - hh), bounds_.color()), depth_ + 1, this);
	}

	/**
	 * 	This is used to insert new items into the quadtree. If a subtree is already at its max depth then
	 *  this function with throw a full_tree_error.
	 */ 
	template <class T, int D, int I>
	void QuadTree<T, D, I>::insert(const T item) {
		if (items_.size() == I && depth_ == D) {
			throw full_tree_error("Quad is full");
		}

		if (top_left_child_ != nullptr) { // If there are subtrees try those first
			Quadrant q = find_quadrant(item);

			switch (q) {
				case Quadrant::TOP_RIGHT: top_right_child_->insert(item); return;
				case Quadrant::TOP_LEFT: top_left_child_->insert(item); return;
				case Quadrant::BOTTOM_LEFT: bottom_left_child_->insert(item); return;
				case Quadrant::BOTTOM_RIGHT: bottom_right_child_->insert(item); return;
				case Quadrant::NONE: break; // Item is on or crossing a boundary and must not be placed in sub-tree
			}
		}

		// insert the item into the current tree
		items_.push_back(item);
		
		// if the previous item brought the tree to a full state then re-organize the tree
		if (items_.size() == I) {
			if (depth_ < D) {
				subdivide();
			}

			std::vector<T> keep;
			for (auto item : items_) {
				Quadrant q = find_quadrant(item);
				switch (q) {
					case Quadrant::TOP_RIGHT: top_right_child_->insert(item); break;
					case Quadrant::TOP_LEFT: top_left_child_->insert(item); break;
					case Quadrant::BOTTOM_LEFT: bottom_left_child_->insert(item); break;
					case Quadrant::BOTTOM_RIGHT: bottom_right_child_->insert(item); break;
					case Quadrant::NONE: keep.push_back(item);
				}
			}
			items_ = keep;
		}	
	}

	template <class T, int D, int I>
	void QuadTree<T, D, I>::draw() const {
		if (top_right_child_ != nullptr) {
			top_left_child_->draw();
			top_right_child_->draw();
			bottom_left_child_->draw();
			bottom_right_child_->draw();
		}

		bounds_.draw_border();
	}

}


#endif
