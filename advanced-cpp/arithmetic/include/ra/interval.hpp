#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cfenv>
#include <numeric>
#include <cassert>

namespace ra::math {
	template <class T> class interval;
	template <class T> std::ostream& operator<<(std::ostream&, const interval<T>&);
	template <class T> bool operator<(const interval<T>& lhs, const interval<T>& rhs);

	struct indeterminate_result : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	// takes 4 different values of type T and returns the max of those values
	template <class T>
	T max4(T v1, T v2, T v3, T v4) {
		return std::max(v1, std::max(v2, std::max(v3, v4)));
	}

	// takes 4 different values of type T and returns the min of those values
	template <class T>
	T min4(T v1, T v2, T v3, T v4) {
		return std::min(v1, std::min(v2, std::min(v3, v4)));
	}

	class rounding_mode_saver {
		private:
			int mode_;	
		public:
		// Save the rounding mode.
		rounding_mode_saver(): mode_(std::fegetround()) {}
		// Restore the rounding mode to the value that was saved at
		// the time of construction.
		~rounding_mode_saver() {
			int res = std::fesetround(mode_);
			if (res != 0) {
				// not possible to get here since rounding mode always valid
			}
		}

		// The type is neither movable nor copyable.
		rounding_mode_saver(rounding_mode_saver&&) = delete;
		rounding_mode_saver(const rounding_mode_saver&) = delete;
		rounding_mode_saver& operator=(rounding_mode_saver&&) = delete;
		rounding_mode_saver& operator=(const rounding_mode_saver&) = delete;
	};

	template <class T>
	class interval {
		public:
			using real_type = T;

			struct statistics {
				// The total number of indeterminate results encountered.
				unsigned long indeterminate_result_count;
				// The total number of interval arithmetic operations.
				unsigned long arithmetic_op_count;
			};

			interval(real_type value = 0): lower_bound_(value), upper_bound_(value) {}
			
			interval(real_type lb, real_type ub): lower_bound_(lb), upper_bound_(ub) {
				assert(lb <= ub);
			}

			interval<T>& operator+=(const interval<T> &rhs) {
				rounding_mode_saver rms;

				std::fesetround(FE_DOWNWARD);
				lower_bound_ += rhs.lower_bound_;

				std::fesetround(FE_UPWARD);
				upper_bound_ += rhs.upper_bound_;

				interval<T>::stats_.arithmetic_op_count += 1;

				return *this;
			}

			interval<T>& operator-=(const interval<T> &rhs) {
				rounding_mode_saver rms;
				real_type rhs_ub = rhs.upper_bound_, rhs_lb = rhs.lower_bound_;

				std::fesetround(FE_DOWNWARD);
				lower_bound_ -= rhs_ub;

				std::fesetround(FE_UPWARD);
				upper_bound_ -= rhs_lb;

				interval<T>::stats_.arithmetic_op_count += 1;

				return *this;
			}

			interval<T>& operator*=(const interval<T> &rhs) {
				rounding_mode_saver rms;
				real_type rhs_ub = rhs.upper_bound_, rhs_lb = rhs.lower_bound_;
				real_type lhs_ub = upper_bound_, lhs_lb = lower_bound_;

				auto up = [](T a, T b) { std::fesetround(FE_UPWARD); return a * b; };
				auto down = [](T a, T b) { std::fesetround(FE_DOWNWARD); return a * b; };

				auto new_bound = [&](T a1, T b1, T a2, T b2) { 
					lower_bound_ = down(a1, b1);
					upper_bound_ = up(a2, b2);
				};

				// all cases for multiplication
				// all use new bound and up/down lanmbdas above for less repetition
				if (lhs_ub <= 0) {
					if (rhs_ub <= 0) { 
						new_bound(lhs_ub, rhs_ub, lhs_lb, rhs_lb);
					} else if (rhs_lb < 0 && 0 < rhs_ub) {
						new_bound(lhs_lb, rhs_ub, lhs_lb, rhs_lb);
					} else {
						new_bound(lhs_lb, rhs_ub, lhs_ub, rhs_lb);
					}

				} else if (lhs_lb < 0 && 0 < lhs_ub) {
					if (rhs_ub <= 0) {
						new_bound(lhs_ub, rhs_lb, lhs_lb, rhs_lb);
					} else if (rhs_lb < 0 && 0 < rhs_ub) {
						lower_bound_ = std::min(down(lhs_lb, rhs_ub), down(lhs_ub, rhs_lb));
						upper_bound_ = std::max(up(lhs_lb, rhs_lb), up(lhs_ub, rhs_ub));
					} else {
						new_bound(lhs_lb, rhs_ub, lhs_ub, rhs_ub);
					}

				} else {
					if (rhs_ub <= 0) { 
						new_bound(lhs_ub, rhs_lb, lhs_lb, rhs_ub);
					} else if (rhs_lb < 0 && 0 < rhs_ub) {
						new_bound(lhs_ub, rhs_lb, lhs_ub, rhs_ub);
					} else {
						new_bound(lhs_lb, rhs_lb, lhs_ub, rhs_ub);
					}
				}

				interval<T>::stats_.arithmetic_op_count += 1;

				return *this;
			}

			real_type lower() const { return lower_bound_; }
			real_type upper() const { return upper_bound_; }

			bool is_singleton() const { return lower_bound_ == upper_bound_ ? true : false; }
			
			// get the sign of the interval and throw exception if interval sign is indeterminate
			int sign() const { 
				if (upper_bound_ < 0) return -1;
				else if (lower_bound_ > 0) return 1;
				else if (lower_bound_ == 0 && upper_bound_ == 0) return 0;
				else {
					interval<T>::stats_.indeterminate_result_count += 1;
					throw indeterminate_result("Solution is indeterminate");
				}
			}

			// clear the static stats counts
			static void clear_statistics() {
				stats_.indeterminate_result_count = 0;
				stats_.arithmetic_op_count = 0;
			}

			// get the current static stats counts across the 
			static void get_statistics(statistics &stats) {
				stats = stats_;
			}
		
			friend std::ostream& operator<< <>(std::ostream&, const interval<T>&);
			friend bool operator< <>(const interval<T>& lhs, const interval<T>& rhs);
		private:
			real_type upper_bound_;
			real_type lower_bound_;
			static statistics stats_;
	};

	template <class T>
	typename interval<T>::statistics interval<T>::stats_ = { 0, 0 };

	template <class T>
	interval<T> operator+(const interval<T>& lhs, const interval<T>& rhs) {
		interval<T> n(lhs.lower(), lhs.upper());
		n += rhs;
		return n;
	}

	template <class T>
	interval<T> operator-(const interval<T>& lhs, const interval<T>& rhs) {
		interval<T> n(lhs.lower(), lhs.upper());
		n -= rhs;
		return n;
	}

	template <class T>
	interval<T> operator*(const interval<T>& lhs, const interval<T>& rhs) {
		interval<T> n(lhs.lower(), lhs.upper());
		n *= rhs;
		return n;
	}

	template <class T>
	bool operator<(const interval<T>& lhs, const interval<T>& rhs) {
		if (lhs.upper_bound_ < rhs.lower_bound_) return true;
		else if (lhs.lower_bound_ >= rhs.upper_bound_) return false;
		else {
			interval<T>::stats_.indeterminate_result_count += 1;
			throw indeterminate_result("Solution is indeterminate");
		}
	}

	template <class T> 
	std::ostream& operator<<(std::ostream& stream, const interval<T>& i) {
		stream << std::setprecision(std::numeric_limits<T>::digits10) 
						<< "[" << i.lower_bound_ << "," << i.upper_bound_ << "]";
		return stream;
	}
}