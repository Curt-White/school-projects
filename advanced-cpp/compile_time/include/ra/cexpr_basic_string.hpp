#include <array>
#include <stdexcept>

namespace ra::cexpr {
// A basic string class template for use in constexpr contexts.
	template <class T, std::size_t M>
	class cexpr_basic_string {
	private:
		T str_data_[M + 1];
		std::size_t length_;
	public:
		// An unsigned integral type used to represent sizes.	
		using size_type = std::size_t;	

		// The type of each character in the string (i.e., an alias for
		// the template parameter T).
		using value_type = T;

		// The type of a mutating pointer to each character in the string.
		using pointer = T*;

		// The type of a non-mutating pointer to each character in the string.
		using const_pointer = const T*;

		// The type of a mutating reference to a character in the string.
		using reference = T&;

		// The type of a non-mutating reference to a character in the string.
		using const_reference = const T&;

		// A mutating iterator type for the elements in the string.
		using iterator = pointer;

		// A non-mutating iterator type for the elements in the string.
		using const_iterator = const_pointer;

		// Creates an empty string (i.e., a string containing no characters).
		constexpr cexpr_basic_string(): str_data_{'\0'}, length_(0) {}

		// Explicitly default some special members.
		constexpr cexpr_basic_string(const cexpr_basic_string&) = default;
		constexpr cexpr_basic_string& operator=(const cexpr_basic_string&) = default;
		~cexpr_basic_string() = default;

		// Creates a string with the contents given by the
		// null-terminated character array pointed to by s.
		// If the string does not have sufficient capacity to hold
		// the character data provided, an exception of type
		// std::runtime_error is thrown.
		constexpr cexpr_basic_string(const value_type* s): str_data_{}, length_(0) {
			int i = 0;
			for (const value_type *curr = s; *curr != '\0'; curr++) {
				if (i == M) throw std::runtime_error("Provided string longer than container");
				str_data_[i] = *curr;
				++i;
			}

			str_data_[i] = '\0';
            length_ = i;
		}

		// Creates a string with the contents specified by the characters
		// in the iterator range [first, last).
		// If the string does not have sufficient capacity to hold
		// the character data provided, an exception of type
		// std::runtime_error is thrown.
		constexpr cexpr_basic_string(const_iterator first, const_iterator last): str_data_{}, length_(0) {
		    int i = 0;
            for (const value_type *curr = first; curr != last; curr++) {
                if (i == M) throw std::runtime_error("Provided string longer than container");
                str_data_[i] = *curr;
                ++i;
            }

            str_data_[i] = '\0';
            length_ = i;
		}

		// Returns the maximum number of characters that can be held by a
		// string of this type.
		// The value returned is the template parameter M.
		static constexpr size_type max_size() { return M; }

		// Returns the maximum number of characters that the string can
		// hold. The value returned is always the template parameter M.
		constexpr size_type capacity() const { return M; }

		// Returns the number of characters in the string (excluding the
		// dummy null character).
		constexpr size_type size() const { return length_; }

		// Returns a pointer to the first character in the string.
		// The pointer that is returned is guaranteed to point to a
		// null-terminated character array.
		// The user of this class shall not alter the dummy null
		// character stored at data() + size().
		value_type* data() { return str_data_; }
		const value_type* data() const { return str_data_; }

		// Returns an iterator referring to the first character in the string.
		constexpr iterator begin() { return str_data_; }
		constexpr const_iterator begin() const { return str_data_; }

		// Returns an iterator referring to the fictitious
		// one-past-the-end character in the string.
		constexpr iterator end() { return &str_data_[length_]; }
		constexpr const_iterator end() const { return &str_data_[length_]; }

		// Returns a reference to the i-th character in the string if i
		// is less than the string size; and returns a reference to the
		// dummy null character if i equals the string size.
		// Precondition: The index i is such that i >= 0 and i <= size().
		constexpr reference operator[](size_type i) {
            if (!(i >= 0 && i <= length_)) {
                throw std::runtime_error("Index out of bounds");
            }

            return str_data_[i];
		}

		constexpr const_reference operator[](size_type i) const {
            if (!(i >= 0 && i <= length_)) {
                throw std::runtime_error("Index out of bounds");
            }

            return str_data_[i];
		}

		// Appends (i.e., adds to the end) a single character to the
		// string. If the size of the string is equal to the capacity,
		// the string is not modified and an exception of type
		// std::runtime_error is thrown.
		constexpr void push_back(const T& x) {
		    if (length_ == M) throw std::runtime_error("String is full");
		    str_data_[length_] = x;
		    str_data_[++length_] = '\0';
		}

		// Erases the last character in the string.
		// If the string is empty, an exception of type std::runtime_error
		// is thrown.
		constexpr void pop_back() {
            if (length_ == 0) throw std::runtime_error("String is empty");
            --length_;
            str_data_[length_] = '\0';
		}

		// Appends (i.e., adds to the end) to the string the
		// null-terminated string pointed to by s.
		// Precondition: The pointer s must be non-null.
		// If the string has insufficient capacity to hold the new value
		// resulting from the append operation, the string is not modified
		// and an exception of type std::runtime_error is thrown.
		constexpr cexpr_basic_string& append(const value_type* s) {
		    if (s == nullptr) {
		        throw std::runtime_error("Null pointer passed");
		    }

		    int s_len = 0;
            for (const value_type *curr = s; *curr != '\0'; curr++, s_len++);
            if ((length_ + s_len) >  M) {
                throw std::runtime_error("String exceeds container length");
            }

            for (int i = 0; i < s_len; i++) {
                str_data_[length_ + i] = s[i];
            }

            str_data_[length_ + s_len] = '\0';
            length_ += s_len;
            return *this;
		}

		// Appends (i.e., adds to the end) to the string another
		// cexpr_basic_string with the same character type (but
		// possibly a different maximum size).
		// If the string has insufficient capacity to hold the new value
		// resulting from the append operation, the string is not modified
		// and an exception of type std::runtime_error is thrown.
		template <size_type OtherM>
		constexpr cexpr_basic_string& append(const cexpr_basic_string<value_type, OtherM>& other) {
            int ot_len = other.size();
            int this_len = length_;
		    if ((this_len + ot_len) > M) {
                throw std::runtime_error("Concat will exceed container size");
            }

            for (int i = 0; i < ot_len; i++) {
                str_data_[this_len + i] = other[i];
            }

			str_data_[length_ + ot_len] = '\0';
            length_ += ot_len;
			return *this;
		}

		// Erases all of the characters in the string, yielding an empty
		// string.
		constexpr void clear() {
            length_ = 0;
            str_data_[0] = '\0';
		}
	};

	template <std::size_t M>
	using cexpr_string = cexpr_basic_string<char, M>;

	constexpr std::size_t num_digits(std::size_t n) {
        int digits = 0;
        while (n) {
            n /= 10;
            digits++;
        }

        return digits;
	}

	// Convert the integer n into a string value
	constexpr std::size_t to_string(std::size_t n, char* buffer, std::size_t size, char** end) {
        int digits = num_digits(n);
        char *local_buffer = buffer;

        if (digits + 1 > size) {
            throw std::runtime_error("String exceeds buffer size");
        }

        std::size_t val = n;
        for (int i = 0; i < digits; i++) {
            local_buffer[digits - 1 - i] = (char)((val % 10) + 48);
            val /= 10;
        }

        local_buffer[digits] = '\0';
        if (end != nullptr) {
            *end = &local_buffer[digits];
        }

        return digits;
	}
 }