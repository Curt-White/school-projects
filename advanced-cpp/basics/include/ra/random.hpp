#include <iostream>

namespace ra::random {
    class linear_congruential_generator {
        public:
            typedef unsigned long long int int_type;

            int_type multiplier() const;
            int_type increment() const;
            int_type modulus() const;
            
            int_type min() const;
            int_type max() const;

            static int_type default_seed();

            void seed(int_type s);
            void discard(unsigned long long n);

            int_type operator()();
            bool operator==(const linear_congruential_generator &rhs) const;
            bool operator!=(const linear_congruential_generator &rhs) const;

            linear_congruential_generator(int_type a, int_type c, int_type m, int_type s = default_seed());

            friend std::ostream& operator<<(std::ostream& stream, const linear_congruential_generator& lcg);
        private:
            int_type a;
            int_type c;
            int_type m;
            int_type x;
    };

    std::ostream& operator<<(std::ostream& stream, const linear_congruential_generator& lcg);
}