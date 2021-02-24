#include "boost/math/constants/constants.hpp"
#include <numeric>
#include <stdexcept>

namespace ra::cexpr_math {

    // The math constant pi.
    // The type T is a floating-point type.
    template <class T>
    constexpr T pi = boost::math::constants::pi<T>();

    // Returns the absolute value of x.
    // The type T is a floating-point type.
    template <class T>
    constexpr T abs(T x) {
        return (x  < 0) ? (x * -1) : (x);
    }

    // Returns the square of x.
    // The type T is a floating-point type.
    template <class T>
    constexpr T sqr(T x) {
        return (x * x);
    }

    // Returns the cube of x.
    // The type T is a floating-point type.
    template <class T>
    constexpr T cube(T x) {
        return (x * x * x);
    }

    // Returns the remainder after division when x is divided by y.
    // In particular, x - n y is returned where n is the result obtained by
    // dividing x by y and then rounding (to an integer value) toward zero.
    // If y is zero, an exception of type std::overflow_error is thrown.
    // The type T is a floating-point type.
    template <class T>
    constexpr T mod(T x, T y) {
        if (y == 0) {
            throw std::overflow_error("Attempted divide 0");
        }

        T n = (int) (x / y);
        return (x - n * y);
    }

    // Returns the sine of x.
    // Note that a particular algorithm must be used to implement this
    // function.
    // The type T is a floating-point type.
    template <class T>
    constexpr T sin(T x) {
        if (abs(x) < 0.000001) {
            return x;
        }

        T next = mod(abs(x), (2 * pi<T>));
        T value = 3 * sin(next / 3) - 4 * cube(sin(next / 3));

        // since sin is an odd func we can negate the result
        if (x < 0) {
            return -1 * value;
        }

        return value;
    }

    // Returns the cosine of x.
    // Note that a particular algorithm must be used to implement this
    // function.
    // The type T is a floating-point type.
    template <class T>
    constexpr T cos(T x) {
        return sin(x + (pi<T> / 2));
    }

    // Returns the tangent of x.
    // Note that a particular algorithm must be used to implement this
    // function.
    // If the tangent of x is infinite, an exception of type
    // std::overflow_error is thrown.
    // The type T is a floating-point type.
    template <class T>
    constexpr T tan(T x) {
        T sine_x = sin(x);
        T cosine_x = cos(x);

        if (cosine_x == 0) {
            throw std::overflow_error("Divide by zero error, cos(x) is 0");
        }

        return (sine_x / cosine_x);
    }

    // Returns the square root of x.
    // If x is negative, an exception of type std::domain_error is thrown.
    // Note that a particular algorithm must be used to implement this
    // function.
    // The type T is a floating-point type.
    template <class T>
    constexpr T sqrt(T x) {
        if (x < 0) {
            throw std::domain_error("X must be a positive value");
        }
        // formula in loop below does not support negative values
        if (x == 0) {
            return 0;
        }

        const T c = x;
        T xn = x;
        while (true) {
            T xnp1 = xn - (sqr(xn) - c) / (2 * xn);
            if (abs(xnp1 - xn) <= std::numeric_limits<T>::epsilon()) {
                break;
            }

            xn = xnp1;
        }

        return xn;
    }
}
