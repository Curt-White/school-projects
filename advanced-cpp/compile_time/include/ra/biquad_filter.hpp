#include "ra/cexpr_math.hpp"

namespace ra::biquad {
    using namespace ra::cexpr_math;

    // Biquad filter coefficients class.
    template <class Real>
        struct biquad_filter_coefs{
        // The real number type used to represent the filter coefficients.
        using real = Real;

        // Creates a set of filter coefficients where the coefficients
        // a0, a1, a2, b0, b1, and b2 are initialized to a0_, a1_, a2_,
        // b0_, b1_, and b2_, respectively.
        constexpr biquad_filter_coefs(real a0_, real a1_, real a2_, real b0_,real b1_, real b2_):
        a0(a0_ / b0_), a1(a1_ / b0_), a2(a2_ / b0_), b0(1), b1(b1_ / b0_), b2(b2_ / b0_) {}

        // Creates a set of filter coefficients by copying from another set.
        // Since Real and OtherReal need not be the same, this constructor
        // can be used to convert between filter coefficients of different
        // types.
        template <class OtherReal>
        constexpr biquad_filter_coefs(const biquad_filter_coefs<OtherReal>& coefs):
        a0(coefs.a0_ / coefs.b0_), a1(coefs.a1_ / coefs.b0_), a2(coefs.a2_ / coefs.b0_),
        b0(1), b1(coefs.b1_ / coefs.b0_), b2(coefs.b2_ / coefs.b0_) {}


        // The filter coefficients a0, a1, a2, b0, b1, and b2.
        real a0;
        real a1;
        real a2;
        real b0;
        real b1;
        real b2;
    };

    template <class Real>
    constexpr Real omega(Real f) {
        return ra::cexpr_math::tan((ra::cexpr_math::pi<Real> / 2) * f);
    }

     // Returns the coefficients for a biquad lowpass filter with normalized
     // cutoff frequency f and Q factor q, where f in [0,1] and q > 0.
     // The filter coefficients are always normalized such that the
     // coefficient b0 is 1.
     // The type Real is a floating-point type.
     // All real arithmetic should be performed with the Real type.
     template <class Real>
     constexpr biquad_filter_coefs<Real> lowpass(Real f, Real q) {
        assert(f >= 0 && f <= 1 && q > 0);
        Real o = omega(f);

        return biquad_filter_coefs<Real>(
            sqr(o),
            2.0 * sqr(o),
            sqr(o),
            sqr(o) + (o / q) + 1.0,
            2.0 * (sqr(o) - 1.0),
            sqr(o) - (o / q) + 1.0
        );
     }

     // Returns the coefficients for a biquad highpass filter with
     // normalized cutoff frequency f and Q factor q, where f in [0,1]
     // and q > 0.
     // The filter coefficients are always normalized such that the
     // coefficient b0 is 1.
     // The type Real is a floating-point type.
     // All real arithmetic should be performed with the Real type.
     template <class Real>
     constexpr biquad_filter_coefs<Real> highpass(Real f, Real q) {
        assert(f >= 0 && f <= 1 && q > 0);
        Real o = omega(f);

        return biquad_filter_coefs<Real>(
            1.0,
            -2.0,
            1.0,
            sqr(o) + (o / q) + 1.0,
            2.0 * (sqr(o) - 1),
            sqr(o) - (o / q) + 1.0
        );
     }

     // Returns the coefficients for a biquad bandpass filter with
     // normalized cutoff frequency f and Q factor q, where f in [0,1]
     // and q > 0.
     // The filter coefficients are always normalized such that the
     // coefficient b0 is 1.
     // The type Real is a floating-point type.
     // All real arithmetic should be performed with the Real type.
     template <class Real>
     constexpr biquad_filter_coefs<Real> bandpass(Real f, Real q) {
        assert(f >= 0 && f <= 1 && q > 0);
        Real o = omega(f);

        return biquad_filter_coefs<Real>(
            o / q,
            0.0,
            - o / q,
            sqr(o) + (o / q) + 1.0,
            2.0 * (sqr(o) - 1.0),
            sqr(o) - (o / q) + 1.0
        );
     }

     // Returns the coefficients for a biquad low-frequency shelving
     // boost filter with normalized cutoff frequency f and gain-control
     // parameter a, where f in [0,1] and a >= 0.
     // For a gain in dB of G (where G > 0), choose a = 10 ˆ (G / 20).
     // The filter coefficients are always normalized such that the
     // coefficient b0 is 1.
     // The type Real is a floating-point type.
     // All real arithmetic should be performed with the Real type.
     template <class Real>
     constexpr biquad_filter_coefs<Real> lowshelf_boost(Real f, Real a) {
        assert(f >= 0 && f <= 1 && a >= 0);
        Real o = omega(f);

        return biquad_filter_coefs<Real>(
            (a * sqr(o)) + (sqrt(2.0 * a) * o) + 1.0,
            2.0 * ((a * sqr(o)) - 1.0),
            (a * sqr(o)) - (sqrt(2.0 * a) * o) + 1.0,
            sqr(o) + (sqrt(2.0) * o) + 1.0,
            2.0 * (sqr(o) - 1.0),
            sqr(o) - (sqrt(2.0) * o) + 1.0
        );
     }

     // Returns the coefficients for a biquad low-frequency shelving
     // cut filter with normalized cutoff frequency f and gain-control
     // parameter a, where f in [0,1] and a >= 0.
     // For a gain in dB of G (where G < 0), choose a = 10 ˆ (-G / 20).
     // The filter coefficients are always normalized such that the
     // coefficient b0 is 1.
     // The type Real is a floating-point type.
     // All real arithmetic should be performed with the Real type.
     template <class Real>
     constexpr biquad_filter_coefs<Real> lowshelf_cut(Real f, Real a) {
        assert(f >= 0 && f <= 1 && a >= 0);
        Real o = omega(f);

        return biquad_filter_coefs<Real>(
            sqr(o) + (sqrt(2.0) * o) + 1.0,
            2.0 * (sqr(o) - 1.0),
            sqr(o) - (sqrt(2.0) * o) + 1.0,
            (a * sqr(o)) + (sqrt(2.0 * a) * o) + 1.0,
            2.0 * ((a * sqr(o)) - 1.0),
            (a * sqr(o)) - (sqrt(2.0 * a) * o) + 1.0
        );
     }
}