#include "CGAL/Cartesian.h"
#include "CGAL/MP_Float.h"
#include "ra/interval.hpp"

namespace ra::geometry {

	// A geometry kernel with robust predicates.
	template <class R>
	class Kernel {
		public:
			// The type used to represent real numbers.
			using Real = R;

			// The type used to represent points in two dimensions.
			using Point = typename CGAL::Cartesian<R>::Point_2;

			// The type used to represent vectors in two dimensions.
			using Vector = typename CGAL::Cartesian<R>::Vector_2;

			// The possible outcomes of an orientation test.
			enum class Orientation : int {
				right_turn = -1,
				collinear = 0,
				left_turn = 1,
			};
			// The possible outcomes of an oriented-side-of test.
			enum class Oriented_side : int {
				on_negative_side = -1,
				on_boundary = 0,
				on_positive_side = 1,
			};

			// The set of statistics maintained by the kernel.
			struct Statistics {
				// The total number of orientation tests.
				std::size_t orientation_total_count;
				// The number of orientation tests requiring exact
				// arithmetic.
				std::size_t orientation_exact_count;
				// The total number of preferred-direction tests.
				std::size_t preferred_direction_total_count;
				// The number of preferred-direction tests requiring
				// exact arithmetic.
				std::size_t preferred_direction_exact_count;
				// The total number of side-of-oriented-circle tests.
				std::size_t side_of_oriented_circle_total_count;
				// The number of side-of-oriented-circle tests
				// requiring exact arithmetic.
				std::size_t side_of_oriented_circle_exact_count;
			};

			// Since a kernel object is stateless, construction and
			// destruction are trivial.
			Kernel() = default;
			~Kernel() = default;

			// The kernel type is both movable and copyable.
			// Since a kernel object is stateless, a copy/move operation
			// is trivial.
			Kernel(const Kernel&) = default;
			Kernel& operator=(const Kernel&) = default;
			Kernel(Kernel&&) = default;
			Kernel& operator=(Kernel&&) = default;

			// Determines how the point c is positioned relative to the
			// directed line through the points a and b (in that order).
			// Precondition: The points a and b have distinct values.
			Orientation orientation(const Point& a, const Point& b, const Point& c);

			// Determines how the point d is positioned relative to the
			// oriented circle passing through the points a, b, and c
			// (in that order).
			// Precondition: The points a, b, and c are not collinear.
			Oriented_side side_of_oriented_circle(const Point& a,
				const Point& b, const Point& c, const Point& d);

			// Determines if, compared to the orientation of line
			// segment cd, the orientation of the line segment ab is
			// more close, equally close, or less close to the
			// orientation of the vector v.
			// The value returned is 1, 0, or -1 if, compared to the
			// orientation of cd, the orientation of ab is more close,
			// equally close, or less close to the orientation of v,
			// respectively.
			// Precondition: The points a and b have distinct values; the
			// points c and d have distinct values; the vector v is not
			// the zero vector.
			int preferred_direction(const Point& a, const Point& b,
			const Point& c, const Point& d, const Vector& v);

			// Tests if the quadrilateral with vertices a, b, c, and d
			// specified in CCW order is strictly convex.
			// Precondition: The vertices a, b, c, and d have distinct
			// values and are specified in CCW order.
			bool is_strictly_convex_quad(const Point& a, const Point& b,
			const Point& c, const Point& d);

			// Tests if the flippable edge, with endpoints a and c and
			// two incident faces abc and acd, is locally Delaunay.
			// Precondition: The points a, b, c, and d have distinct
			// values; the quadrilateral abcd must be strictly convex.
			bool is_locally_delaunay_edge(const Point& a, const Point& b,
			const Point& c, const Point& d);

			// Tests if the flippable edge, with endpoints a and c and
			// two incident faces abc and acd, has the preferred-directions
			// locally-Delaunay property with respect to the first and
			// second directions u and v.
			// Precondition: The points a, b, c, and d have distinct values;
			// the vectors u and v are not zero vectors; the vectors u and
			// v are neither parallel nor orthogonal.
			bool is_locally_pd_delaunay_edge(const Point& a,
			const Point& b, const Point& c, const Point& d,
			const Vector& u, const Vector& v);

			// Clear (i.e., set to zero) all kernel statistics.
			static void clear_statistics() {
				stats_ = { 0, 0, 0, 0, 0, 0 };
			}
 
			// Get the current values of the kernel statistics.
			static void get_statistics(Statistics& statistics) {
				statistics = stats_;
			}
		private:
			template <class P> 
			P gen_preferred_direction(const Point& a, const Point& b, const Point& c, const Point& d, const Vector& v);

			template <class P>
			P gen_side_of_oriented_circle(const Point& a, const Point& b, const Point& c, const Point& d);
			
			static Statistics stats_;
	};

	template <class R>
	typename Kernel<R>::Statistics Kernel<R>::stats_ = { 0, 0, 0, 0, 0, 0 };

	// used to compute the determinant of a 3d matrix
	template <class P> P determinant_3d(const P mat[3][3]) {
	return mat[0][0] * ( mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2] )
		- mat[1][0] * ( mat[0][1] * mat[2][2] - mat[2][1] * mat[0][2] )
		+ mat[2][0] * ( mat[0][1] * mat[1][2] - mat[1][1] * mat[0][2] );
	}

	template <class R>
	typename Kernel<R>::Orientation Kernel<R>::orientation(const Point& a, const Point& b, const Point& c) {
		stats_.orientation_total_count += 1;
		try {
			ra::math::interval<R> ax(a.x()), ay(a.y()), 
				bx(b.x()), by(b.y()), cx(c.x()), cy(c.y());

			// compute the determinant
			ra::math::interval<R> res = (ax - cx) * (by - cy) - (bx - cx) * (ay - cy);
			
			if (res.sign() == 0) {
				return  Orientation::collinear;
			} else if (res.sign() == -1) {
				return Orientation::right_turn;
			}

			return Orientation::left_turn;
		} catch (const ra::math::indeterminate_result &e) { // the case where the interval is indeterminate
			stats_.orientation_exact_count += 1;
			CGAL::MP_Float ax(a.x()), ay(a.y()), bx(b.x()), by(b.y()), cx(c.x()), cy(c.y());

			CGAL::MP_Float res = (ax - cx) * (by - cy) - (bx - cx) * (ay - cy);
			if (res.sign() == 0) {
				return  Orientation::collinear;
			} else if (res.sign() == -1) {
				return Orientation::right_turn;
			}

			return Orientation::left_turn;
		}
	}

	// This is a templated function that perfroms the same computation for either the MP_Float or the interval
	template <class R>
	template <class P>
	P Kernel<R>::gen_side_of_oriented_circle(const Point& a, const Point& b, const Point& c, const Point& d) {
		P ax(a.x()), ay(a.y()), bx(b.x()), by(b.y()), cx(c.x()), cy(c.y()), dx(d.x()), dy(d.y());
		
		auto sqadd = [] (P a__, P b__) { return a__ * a__ + b__ * b__; };
		const P mat[3][3] = {
			{ ax - dx, ay - dy, sqadd(ax, ay) - sqadd(dx, dy) }, 
			{ bx - dx, by - dy, sqadd(bx, by) - sqadd(dx, dy) },
			{ cx - dx, cy - dy, sqadd(cx, cy) - sqadd(dx, dy) }
		};

		// This determinant function is at the top of the file
		return determinant_3d<P>(mat);
	}

	template <class R>
	typename Kernel<R>::Oriented_side Kernel<R>::side_of_oriented_circle(const Point& a,
		const Point& b, const Point& c, const Point& d) {
		try {
			stats_.side_of_oriented_circle_total_count += 1;
			ra::math::interval<R> res = gen_side_of_oriented_circle< ra::math::interval<R> >(a, b, c, d);
			if (res.sign() == 0) {
				return  Oriented_side::on_boundary;
			} else if (res.sign() == -1) {
				return Oriented_side::on_negative_side;
			}

			return Oriented_side::on_positive_side;
		} catch (const ra::math::indeterminate_result &e) { // in the case the interval sign is indeterminate
			stats_.side_of_oriented_circle_exact_count += 1;
			CGAL::MP_Float res = gen_side_of_oriented_circle< CGAL::MP_Float >(a, b, c, d);
			
			if (res == 0) {
				return Oriented_side::on_boundary;
			} else if (res < 0) {
				return Oriented_side::on_negative_side;
			}

			return Oriented_side::on_positive_side;
		}
	}

	// this is a generic function which will compute for both the float and the interval
	template <class R>
	template <class P>
	P Kernel<R>::gen_preferred_direction(const Point& a, const Point& b,
					const Point& c, const Point& d, const Vector& v) {
		const P ax(a.x()), ay(a.y()), bx(b.x()), by(b.y()), cx(c.x()), cy(c.y()), dx(d.x()), dy(d.y()),
			vx(v.x()), vy(v.y());
		
		auto isqr = [] (auto a) { return a * a; }; //simple sqare function

		// compute the equation (|d-c|^2 * (b-a) * v - |b-a|^2 * (d-c) * v)
		// first term |d-c|^2
		P dcx = dx - cx;
		P dcy = dy - cy;
		P dc = isqr(dcx) + isqr(dcy);

		// second term (b-a) * v 
		P bax = bx - ax;
		P bay = by - ay;
		P bav = isqr(bax * vx + bay * vy);

		// third term |b-a|^2 *
		P ba = isqr(bax) + isqr(bay); 
		// last term (d-c) * v
		P dcv = isqr(dcx * vx + dcy * vy);

		return (dc * bav - ba * dcv);
	}

	template <class R>
	int Kernel<R>::preferred_direction(const Point& a, const Point& b,
		const Point& c, const Point& d, const Vector& v) {

		try {
			stats_.preferred_direction_total_count += 1;
			ra::math::interval<R> res = gen_preferred_direction< ra::math::interval<R> >(a, b, c, d, v);
			return res.sign();
		} catch (const ra::math::indeterminate_result &e) { // if the sign of interval is nondeterministic
			stats_.preferred_direction_exact_count += 1;
			CGAL::MP_Float res = gen_preferred_direction< CGAL::MP_Float >(a, b, c, d, v);
			if (res > 0) {
				return 1;
			} else if (res == 0) {
				return 0;
			}
			
			return -1;
		}
	}

	template <class R>
	bool Kernel<R>::is_strictly_convex_quad(const Point& a, const Point& b, const Point& c, const Point& d) {
		// 4 checks by traversing ccw around points
		Orientation o = orientation(a, b, c);
		if (o != Orientation::left_turn) return false;

		o = orientation(b, c, d);
		if (o != Orientation::left_turn) return false;

		o = orientation(c, d, a);
		if (o != Orientation::left_turn) return false;

		o = orientation(d, a, b);
		if (o != Orientation::left_turn) return false;

		return true;
	}

	template <class R>
	bool Kernel<R>::is_locally_delaunay_edge(const Point& a, const Point& b, const Point& c, const Point& d) {
		if (side_of_oriented_circle(a, b, c, d) != Oriented_side::on_positive_side) {
			return true;
		}

		return false;
	}

	template <class R>
	bool Kernel<R>::is_locally_pd_delaunay_edge(const Point& a, const Point& b, const Point& c, const Point& d,
			const Vector& u, const Vector& v) {
	
		auto res = side_of_oriented_circle(a, b, c, d);
		if (res == Oriented_side::on_negative_side) {
			return true;
		} else if (res == Oriented_side::on_positive_side) {
			return false;
		} else {
			// perform alpha function
			auto pref_res = preferred_direction(a, c, b, d, u);
			auto pref_res2 = preferred_direction(a, c, b, d, v);
			if (pref_res > 0) {
				return true;
			} else if (pref_res == 0 && pref_res2 > 0) {
				return true;
			} else {
				return false;
			}
		}
	}
}