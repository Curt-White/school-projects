#include "ra/random.hpp"
#include <assert.h>

namespace ra::random {
    using lcg = linear_congruential_generator;

    lcg::linear_congruential_generator(int_type a, int_type c, 
        int_type m, int_type s): a(a), c(c), m(m) {
	assert(m > 0);
	assert(a < m && a > 0);
	assert(c >= 0 && c < m);
        seed(s);
	assert(x >= 0 && x < m);
    }

    // return the minimum value that the generator can make 
    lcg::int_type lcg::min() const { return c == 0 ? 1 : 0; }

    // return the maximum value that can be generated
    lcg::int_type lcg::max() const { return m - 1; }
    
    lcg::int_type lcg::multiplier() const { return a; }

    lcg::int_type lcg::increment() const { return c; }

    lcg::int_type lcg::modulus() const { return m; }

    lcg::int_type lcg::default_seed() { return 1; }

    // seed/initialize to x0 state of the generator with new seed
    void lcg::seed(lcg::int_type s) {
        if (c % m == 0 && s % m == 0) {
            x = 1;
        } else {
            x = s;
        }
    }

    // skip the next n steps in the generation process
    void lcg::discard(unsigned long long n) {
        for (int i = 0; i < n; i++) {
            (*this)();
        }
    }

    // lcgs are equal if they have the same state, mult, increment and modulus
    bool lcg::operator==(const lcg &rhs) const {
        if (a == rhs.a && c == rhs.c && m == rhs.m && x == rhs.x) {
            return true;
        }

        return false;
    }

    bool lcg::operator!=(const lcg &rhs) const {
        if (a != rhs.a || c != rhs.c || m != rhs.m || x != rhs.x) {
            return true;
        }

        return false;
    }

    // generate the next item in the sequence
    lcg::int_type lcg::operator()() {
        x = (a * x + c) % m;
        return x;
    }

    // print the generator in the format '{a} {c} {m} {x}'
    std::ostream& operator<<(std::ostream& stream, const linear_congruential_generator& lcg) {
        stream << lcg.a << " " << lcg.c << " " << lcg.m << " " << lcg.x;
        return stream; 
    }
}
