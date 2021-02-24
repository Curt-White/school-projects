#include <type_traits>
#include "boost/iterator/iterator_facade.hpp"
#include "parent_from_member.hpp"

namespace ra::intrusive {

	// Per-node list management information class.
	// The implementation-defined type that contains per-node list
	// management information (i.e., successor and predecessor).
	// This class has the list class template as a friend.
	// This type must contain pointers (of type list_hook*) to the
	// next and previous node in the list.
	class list_hook {
		private:
			list_hook* predecessor_;
			list_hook* successor_;

			template <class T, list_hook T::* Hook> friend class list;
		public:
		// The only allowable public members for this class
		// are the ones listed below.
		// The particular behavior of the following special
		// member functions are implementation defined and
		// may be defaulted if appropriate.
		list_hook() = default;
		list_hook(const list_hook&) = default;
		list_hook(list_hook&&) = default;
		list_hook& operator=(const list_hook&) = default;
		list_hook& operator=(list_hook&&) = default;
		~list_hook() = default;
	};

	// Intrusive doubly-linked list (with sentinel node).
	template <class T, list_hook T::* Hook>
	class list {
		private:
			list_hook sentinel_;
			std::size_t size_;
		public:

		template<class L>
		class list_iterator: public boost::iterator_facade<list_iterator<L>, L, boost::bidirectional_traversal_tag> {
			public:
			using base = boost::iterator_facade<list_iterator<L>, L, boost::bidirectional_traversal_tag>;
			using typename base::reference;
			using typename base::value_type;

			template <class OtherT, class = std::enable_if_t<std::is_convertible_v<OtherT*, L*>>>
			list_iterator (const list_iterator<OtherT>& other) : node_(other.node_) {}
			list_iterator (L* node = nullptr): node_(&(node->*Hook)) {}
			list_iterator (list_hook* node = nullptr): node_(node) {}

			private:
			friend class boost::iterator_core_access;
			// befriend iterator with other template types for comparisson
			template <class OtherT> friend class list_iterator;
			template <class OtherT> bool equal(const list_iterator<OtherT>& other) const { return node_ == other.node_; }			
			reference dereference() const { return *ra::util::parent_from_member(node_, Hook); }
			void increment() { node_ = node_->successor_; }
			void decrement() { node_ = node_->predecessor_; }
			list_hook* node_;
		};

		// The type of the elements in the list.
		using value_type = T;

		// The pointer-to-member associated with the list hook object.
		static constexpr list_hook T::* hook_ptr = Hook;

		// The type of a mutating reference to a node in the list.
		using reference = T&;

		// The type of a non-mutating reference to a node in the list.
		using const_reference = const T&;

		// The mutating (bidirectional) iterator type for the list.
		// This type must provide all of the functionality of a
		// bidirectional iterator.
		// The Boost Iterator library may be used to implement this
		// type.
		using iterator = list_iterator<T>;

		// The non-mutating (bidirectional) iterator type for the list.
		// This type must provide all of the functionality of a
		// bidirectional iterator.
		// The Boost Iterator library may be used to implement this
		// type.
		using const_iterator = list_iterator<const T>;

		// An unsigned integral type used to represent sizes.
		using size_type = std::size_t;

		// Creates an empty list.
		// Time complexity: Constant.
		list();

		// Erases any elements from the list and then destroys the list.
		// Time complexity: Either linear or constant.
		~list();

		// Move construction.
		// The elements in the source list (i.e., other) are moved from
		// the source list to the destination list (i.e., *this), preserving
		// their relative order.
		// After the move, the source list is empty.
		// Time complexity: Constant.
		list(list&& other);

		// Move assignment.
		// The elements in the source list (i.e., other) are moved from
		// the source list to the destination list (i.e., *this),
		// preserving their relative order.
		// If *this is not empty, *this is cleared before the move operation
		// is applied.
		// After the move, the source list is empty.
		// Precondition: The objects *this and other are distinct.
		// Time complexity: Constant or linear in size of *this.
		list& operator=(list&& other);

		// Do not allow the copying of lists.
		list(const list&) = delete;
		list& operator=(const list&) = delete;

		// Swaps the elements of *this and x.
		// Swapping the elements of a list with itself has no effect.
		// Time complexity: Constant.
		void swap(list& x);

		// Returns the number of elements in the list.
		// Time complexity: Constant.
		size_type size() const { return size_; }

		// Inserts an element in the list before the element referred to
		// by the iterator pos.
		// An iterator that refers to the inserted element is returned.
		// Time complexity: Constant.
		iterator insert(iterator pos, value_type& value);
		
		// Erases the element in the list at the position specified by the
		// iterator pos.
		// An iterator that refers to element following the erased element
		// is returned if such an element exists; otherwise, end() is
		// returned.
		// Time complexity: Constant.
		iterator erase(iterator pos);

		// Inserts the element with the value x at the end of the list.
		// Time complexity: Constant.
		void push_back(value_type& x);

		// Erases the last element in the list.
		// Precondition: The list is not empty.
		// Time complexity: Constant.
		void pop_back();

		// Returns a reference to the last element in the list.
		// Precondition: The list is not empty.
		// Time complexity: Constant.
		reference back() { return *ra::util::parent_from_member(sentinel_.predecessor_, Hook); }
		const_reference back() const { return *ra::util::parent_from_member(sentinel_.predecessor_, Hook); }

		// Erases any elements from the list, yielding an empty list.
		// Time complexity: Either linear or constant.
		void clear();

		// Returns an iterator referring to the first element in the list
		// if the list is not empty and end() otherwise.
		// Time complexity: Constant.
		const_iterator begin() const { return sentinel_.successor_;  }
		iterator begin() { return sentinel_.successor_;  }

		// Returns an iterator referring to the fictitious one-past-the-end
		// element.
		// Time complexity: Constant.
		// the const cast is permittable since the resulting iterator is constant and cannot modify
		const_iterator end() const { return const_iterator(const_cast<list_hook*>(&sentinel_)); }
		iterator end() { return &sentinel_; }
	};

	template <class T, list_hook T::* Hook>
	list<T, Hook>::list(): size_(0) {
		sentinel_.predecessor_ = &sentinel_;
		sentinel_.successor_ = &sentinel_;
	}

	template <class T, list_hook T::* Hook>
	list<T, Hook>::~list() {
		// have to destroy every element?
	}

	template <class T, list_hook T::* Hook>
	void list<T, Hook>::push_back(value_type& x) {
		(x.*Hook).predecessor_ = sentinel_.predecessor_;
		sentinel_.predecessor_->successor_ = &(x.*Hook);
		(x.*Hook).successor_ = &sentinel_;
		sentinel_.predecessor_ = &(x.*Hook);
		size_ ++;
	}

	template <class T, list_hook T::* Hook>
	void list<T, Hook>::pop_back() {
		if (size_ > 0) {
			list_hook *removed_node = sentinel_.predecessor_;
			removed_node->predecessor_->successor_ = &sentinel_;
			sentinel_.predecessor_ = removed_node->predecessor_;
			removed_node->predecessor_ = nullptr;
			removed_node->successor_ = nullptr;
			size_--;
		}
	}

	template <class T, list_hook T::* Hook>
	void list<T, Hook>::swap(list& x) {
		auto temp = x.sentinel_;
		auto ot_size = x.size_;
	
		x.sentinel_ = this->sentinel_;
		this->sentinel_ = temp;

		x.size_ = this->size_;
		this->size_ = ot_size;
	}

	template <class T, list_hook T::* Hook>
	typename list<T, Hook>::iterator list<T, Hook>::insert(iterator pos, value_type& value) {
		list_hook *new_value = &(value.*Hook);
		// link new node to previous node
		pos->hook.predecessor_->successor_ = new_value;
		new_value->predecessor_ = pos->hook.predecessor_;

		// link new node to its successor
		pos->hook.predecessor_ = new_value;
		new_value->successor_ = &(pos->hook);
		size_++;

		return new_value;
	}

	template <class T, list_hook T::* Hook>
	typename list<T, Hook>::iterator list<T, Hook>::erase(list<T, Hook>::iterator pos) {
		list_hook *new_value = pos->hook.successor_;
		list_hook *removed_node = &(pos->hook);

		removed_node->predecessor_->successor_ = removed_node->successor_;
		removed_node->successor_->predecessor_ = removed_node->predecessor_;
		removed_node->predecessor_ = nullptr;
		removed_node->successor_ = nullptr;
		size_--;

		return new_value;
	}

	template <class T, list_hook T::* Hook>
	list<T, Hook>& list<T, Hook>::operator=(list&& other) {
		this->sentinel_ = other.sentinel_;
		this->size_ = other.size_;
		other.clear();
		return *this;
	}

	template <class T, list_hook T::* Hook>
	list<T, Hook>::list(list&& other) {
		this->size_ = other.size();
		this->sentinel_ = other.sentinel_;
		other.clear();
	}

	template <class T, list_hook T::* Hook>
	void list<T, Hook>::clear() {
		sentinel_.predecessor_ = &sentinel_;
		sentinel_.successor_ = &sentinel_;
		size_ = 0;
	}
}
