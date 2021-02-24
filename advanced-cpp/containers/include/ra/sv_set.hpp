#include <functional>

namespace ra::container {
	// A class representing a set of unique elements (which uses
	// a sorted array).
	template <class Key, class Compare = std::less<Key> >
	class sv_set {
		private:
			Key* start_; 	// first element
			Key* finish_;	// one past last element
			Key* end_;	// one past the end of allocated space
			Compare comp_;

		public:

		// A dummy type used to indicate that elements in a range
		// are both ordered and unique.
		struct ordered_and_unique_range {};

		// The type of the elements held by the container.
		// This is simply an alias for the template parameter Key.
		using value_type = Key;
		using key_type = Key;

		// The type of the function/functor used to compare two keys.
		// This is simply an alias for the template parameter Compare.
		using key_compare = Compare;

		// An unsigned integral type used to represent sizes.
		using size_type = std::size_t;

		// The mutable (random-access) iterator type for the
		// container.
		// This type must support all of the functionality associated
		// with a random-access iterator.
		using iterator = Key*;

		// The non-mutable (random-access) const_iterator type for
		// the container.
		// This type must support all of the functionality associated
		// with a random-access iterator.
		using const_iterator = const Key*;

		// Creates an empty set (i.e., a set containing no elements)
		// with a capacity of zero (i.e., no allocated storage for
		// elements).
		// Time complexity: Constant.
		sv_set() noexcept(std::is_nothrow_default_constructible_v<key_compare>);

		// Create a set consisting of the n elements in the
		// range starting at first, where the elements in the range
		// must be both unique and ordered with respect to the
		// comparison operation embodied by the key_compare type.
		// If the specified range is not both ordered and unique,
		// the behavior of this function is undefined.
		// Time complexity: Linear in n.
		// Note:
		// The parameter of type ordered_and_unique_range is always
		// ignored.
		// This parameter is only present to allow for future expansion
		// (i.e., adding a constructor that does not require an ordered
		// and unique range).
		// Note: The type InputIterator must meet the requirements of
		// an input iterator.
		template <class InputIterator>
		sv_set(ordered_and_unique_range, InputIterator first, std::size_t n);

		// Move construction.
		// Creates a new set by moving from the specified set other.
		// After construction, the source set (i.e., other) is
		// guaranteed to be empty.
		// Time complexity: Constant.
		sv_set(sv_set&& other) noexcept(std::is_nothrow_move_constructible_v<key_compare>);

		// Move assignment.
		// Assigns the value of the specified set other to *this
		// via a move operation.
		// After the move operation, the source set (i.e., other)
		// is guaranteed to be empty.
		// Time complexity: Linear in size().
		// Precondition: The objects *this and other are distinct.
		sv_set& operator=(sv_set&& other) noexcept(std::is_nothrow_move_assignable_v<key_compare>);

		// Copy construction.
		// Creates a new set by copying from the specified set other.
		// Time complexity: Linear in other.size().
		sv_set(const sv_set& other);

		// Copy assignment.
		// Assigns the value of the specified set other to *this.
		// Time complexity: Linear in size() and other.size().
		sv_set& operator=(const sv_set& other);

		// Erases all elements in the container and destroys the
		// container.
		// Time complexity: Linear in size().
		~sv_set();

		// Returns the comparison object for the container.
		// Time complexity: Constant.
		key_compare key_comp() const { return comp_; }

		// Returns an iterator referring to the first element in the
		// set if the set is not empty and end() otherwise.
		// Time complexity: Constant.
		const_iterator begin() const noexcept { return start_ == finish_ ? finish_ : start_; }
		iterator begin() noexcept { return start_ == finish_ ? finish_ : start_; }

		// Returns an iterator referring to the fictitious
		// one-past-the-end element for the set.
		// Time complexity: Constant.
		const_iterator end() const noexcept { return finish_; }
		iterator end() noexcept { return finish_; }

		// Returns the number of elements in the set (i.e., the size
		// of the set).
		// Time complexity: Constant.
		size_type size() const noexcept { return finish_ - start_; }

		// Returns the number of elements for which storage is
		// available (i.e., the capacity of the set). This value is
		// always at least as great as size().
		// Time complexity: Constant.
		size_type capacity() const noexcept { return end_ -  start_; }

		// Reserves storage in the container for at least n elements.
		// After this function has been called with a value of n, it
		// is guaranteed that no memory-allocation is needed as long
		// as the size of the container does not exceed n.
		// Calling this function has no effect if the capacity of the
		// container is already at least n (i.e., the capacity of
		// the container is never reduced by this function).
		// Time complexity: At most linear in size().
		void reserve(size_type n);

		// Reduces the capacity of the container to the container size.
		// If the capacity of the container is greater than its size,
		// the capacity is reduced to the size of the container.
		// Calling this function has no effect if the capacity of the
		// container does not exceed its size.
		// Time complexity: At most linear in size().
		void shrink_to_fit();

		// Inserts the element x in the set.
		// If the element x is already in the set, no insertion is
		// performed (since a set cannot contain duplicate values).
		// Return value:
		// The second (i.e., boolean) component of the returned pair
		// is true if and only if the insertion takes place; and the
		// first (i.e., iterator) component of the pair refers to
		// the element with key equivalent to the key of x
		// (i.e., the element inserted if insertion took place or
		// the element found with an equal key if insertion did not
		// take place).
		// Time complexity: Search logarithmic in size() plus
		// insertion linear in either number of elements with larger
		// keys than x (if size() < capacity()) or size()
		// (if size() == capacity()).
		std::pair<iterator, bool> insert(const key_type& x);

		// Erases the element referenced by pos from the container.
		// Returns an iterator referring to the element following the
		// erased one in the container if such an elements exists or
		// end() otherwise.
		// Time complexity: Linear in number of elements with larger
		// keys than x.
		iterator erase(const_iterator pos);

		// Swaps the contents of the container with the contents of the
		// container x.
		// Time complexity: Constant.
		void swap(sv_set& x) noexcept(std::is_nothrow_swappable_v<key_compare>);

		// Erases any elements in the container, yielding an empty container.
		// Time complexity: Linear in size().
		void clear() noexcept;

		// Searches the container for an element with the key k.
		// If an element is found, an iterator referencing the element
		// is returned; otherwise, end() is returned.
		// Time complexity: Logarithmic.
		iterator find(const key_type& k);
		const_iterator find(const key_type& k) const;

		private:
		iterator search(const key_type& k) const;
	};

	template <class Key, class Compare>
	sv_set<Key, Compare>::sv_set() noexcept(std::is_nothrow_default_constructible_v<key_compare>): 
		start_(nullptr), end_(nullptr), finish_(nullptr), comp_() {}


	template <class Key, class Compare>
	template <class InputIterator>
	sv_set<Key, Compare>::sv_set(ordered_and_unique_range, InputIterator first, std::size_t n): comp_() {
		start_ = static_cast<Key*>(::operator new(n * sizeof(Key)));
		end_ = start_ + n;

		try {
			// copy the n elements
			finish_ = std::uninitialized_copy_n(first, n, start_);
		} catch (...) {
			::operator delete(start_);
			throw;
		}
	}

	template <class Key, class Compare>
	sv_set<Key, Compare>::sv_set(sv_set&& other) 
		noexcept(std::is_nothrow_move_constructible_v<key_compare>) {

		this->start_ = other.start_;
		other.start_ = nullptr;

		this->end_ = other.end_;
		other.end_ = nullptr;

		this->finish_ = other.finish_;
		other.finish_ = nullptr;

		this->comp_ = std::move(other.comp_);
	}

	template <class Key, class Compare>
	sv_set<Key, Compare>& sv_set<Key, Compare>::operator=(sv_set&& other) 
		noexcept(std::is_nothrow_move_assignable_v<key_compare>) {

		if (this != &other) {
			// if there are elements then destroy them
			this->clear();

			// then free the mem from old this
			::operator delete(this->start_);
			this->start_ = other.start_;
			other.start_ = nullptr;

			this->end_ = other.end_;
			other.end_ = nullptr;

			this->finish_ = other.finish_;
			other.finish_ = nullptr;
					
			this->comp_ = std::move(other.comp_);
		}

		return *this;
	}

	template <class Key, class Compare>
	sv_set<Key, Compare>::sv_set(const sv_set& other) {
		this->start_ = static_cast<Key*>(operator new(sizeof(Key) * other.capacity()));
		
		try {
			this->finish_ = std::uninitialized_copy(other.start_, other.finish_, this->start_);
			this->end_ = this->start_ + other.capacity();
		} catch (...) {
			operator delete(this->start_);
			throw;
		}

		this->comp_ = other.comp_;
	}

	template <class Key, class Compare>
	sv_set<Key, Compare>& sv_set<Key, Compare>::operator=(const sv_set& other) {
		if (this != &other) {
			// if there are elements then destroy them
			this->clear();

			// then free the mem from old this
			::operator delete(this->start_);
			this->start_ = static_cast<Key*>(operator new(sizeof(Key) * other.capacity()));
			try {
				this->finish_ = std::uninitialized_copy(other.start_, other.finish_, this->start_);
				this->end_ = this->start_ + other.capacity();
			} catch (...) {
				operator delete(this->start_);
				throw;
			}

			this->comp_ = other.comp_;
		}

		return *this;
	}

	template <class Key, class Compare>
	void sv_set<Key, Compare>::reserve(size_type n) {
		if (capacity() < n) {
			auto new_start = static_cast<Key*>(operator new(sizeof(Key) * n));

			try {
				// move all of the items from the old location to a new longer one
				finish_ = std::uninitialized_move(start_, finish_, new_start);
			} catch(...) {
				operator delete(new_start);
				throw;
			}

			operator delete(start_);
			start_ = new_start;
			end_ = new_start + n;
		}
	}

	template <class Key, class Compare>
	void sv_set<Key, Compare>::shrink_to_fit() {
		if (capacity() > size()) {
			auto new_start = static_cast<Key*>(operator new(sizeof(Key) * size()));

			try {
				// move all of the items from the old location to a new longer one
				finish_ = std::uninitialized_move(start_, finish_, new_start);
			} catch (...) {
				operator delete(new_start);
				throw;
			}

			operator delete(start_);
			start_ = new_start;
			end_ = finish_;
		}
	}

	template <class Key, class Compare>
	std::pair<typename sv_set<Key, Compare>::iterator, bool>
		sv_set<Key, Compare>::insert(const key_type& x) {

		iterator k = find(x);
		if (k != finish_) {
			return std::pair(k, false);
		}

		// if the set is full
		if (size() == capacity()) {
			reserve(size() + 1);
		}

		iterator iter;
		for (iter = start_; iter != finish_; iter++) {
			if (comp_(*iter, x)) continue;
			break;
		}

		// shift all elements to the right
		std::move_backward(iter, finish_, finish_ + 1);
		// insert the element into the open spot
		std::uninitialized_fill_n(iter, 1, x);

		finish_ ++;
		return std::pair(iter, true);
	}

	template <class Key, class Compare>
	typename sv_set<Key, Compare>::iterator sv_set<Key, Compare>::erase(const_iterator pos) {
		// ik this is gross. const cast is bad
		auto orig_pos = const_cast<iterator>(pos);
		std::move(orig_pos + 1, finish_, orig_pos);
		std::destroy_at(--finish_);
		// shifted finish down one so if the original pos was last item then return finish else the orig pos
		// which is the same location as the deleted item.
		return orig_pos == finish_ ? finish_ : orig_pos;
	}

	template <class Key, class Compare>
	void sv_set<Key, Compare>::swap(sv_set& x) noexcept(std::is_nothrow_swappable_v<key_compare>) {
		auto start = x.start_;
		x.start_ = this->start_;
		this->start_ = start;

		auto finish = x.finish_;
		x.finish_ = this->finish_;
		this->finish_ = finish;

		auto end = x.end_;
		x.end_ = this->end_;
		this->end_ = end;

		std::swap(this->comp_, x.comp_);
	}

	template <class Key, class Compare>
	typename sv_set<Key, Compare>::iterator sv_set<Key, Compare>::search(const key_type& k) const {
		if (size() == 0) return finish_;

		auto left_bound = start_;
		auto right_bound = finish_ - 1;

		while (left_bound <= right_bound) {
			auto middle = left_bound + (right_bound - left_bound) / 2;

			// if k is equal to the middle
			if (!comp_(*middle, k) && !comp_(k, *middle)) {
				return middle;
			}

			// if k in upper half
			if (comp_(*middle, k)) {	// split to top half of array
				left_bound = middle + 1;
			} else {			// split to bottom half of array
				right_bound = middle - 1;
			}
		}

		return finish_;
	}

	template <class Key, class Compare>
	typename sv_set<Key, Compare>::iterator sv_set<Key, Compare>::find(const key_type& k) {
		return search(k);
	}

	template <class Key, class Compare>
	typename sv_set<Key, Compare>::const_iterator sv_set<Key, Compare>::find(const key_type& k) const {
		return search(k);
	}

	template <class Key, class Compare>
	void sv_set<Key, Compare>::clear() noexcept {
		if (size() > 0) {
			std::destroy(start_, finish_);
			finish_ = start_;
		}
	}

	template <class Key, class Compare>
	sv_set<Key, Compare>::~sv_set() {
		if (size()) {
			std::destroy(start_, finish_);
		}

		::operator delete(this->start_);
	}
}
