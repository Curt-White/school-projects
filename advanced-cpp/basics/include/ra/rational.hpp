#include <iostream>
#include <limits>
#include <numeric>
#include<ctype.h>

namespace ra::math {
    template <class T> class rational;
    template <class T> std::ostream& operator<<(std::ostream& stream, const rational<T>& arg);
    template <class T> std::istream& operator>>(std::istream& stream, rational<T>& arg);

    template <class T> 
    class rational {
        private:
            T num;
            T denom;

	    void format();
	    
        public:
            using int_type = T;

            rational();
            rational(int_type num, int_type denom = 1);

            int_type numerator() const;
            int_type denominator() const;

            int_type truncate() const;
            bool is_integer() const;

            rational<T>& operator+=(const rational<T>& rhs);
            rational<T>& operator-=(const rational<T>& rhs);
            rational<T>& operator*=(const rational<T>& rhs);
            rational<T>& operator/=(const rational<T>& rhs);

            bool operator!() const;
            bool operator!=(const rational<T>& rhs) const;
            bool operator==(const rational<T>& rhs) const;

            bool operator>(const rational<T>& rhs) const;
            bool operator<(const rational<T>& rhs) const;
            bool operator>=(const rational<T>& rhs) const;
            bool operator<=(const rational<T>& rhs) const;

            rational<T>& operator++();     //prefix
            rational<T>& operator--();

            rational<T> operator++(int);   //post fix
            rational<T> operator--(int);


            friend std::ostream& operator<< <>(std::ostream& stream, const rational<T>& arg);
            friend std::istream& operator>> <>(std::istream& stream, rational<T>& arg);
    };

    template <class T> rational<T>::rational(): num(0), denom(1) {}

    template <class T> rational<T>::rational(int_type num, int_type denom) {
        if (denom == 0) {
            this->num = std::numeric_limits<T>::max();;
            this->denom = 1;
        } else {
            this->num = num;
            this->denom = denom;
	    format(); 
        }
    }

    // format the rational number via reduction and moving negative values to the numerator
    template <class T> void rational<T>::format() {	
    	const int div = std::gcd(num, denom);
	num /= div;
	denom /= div;
        if (denom < 0) {
      	    num *= -1;
            denom *= -1;
        }
    }

    template <class T> typename 
    rational<T>::int_type rational<T>::numerator() const { return num; }

    template <class T> typename 
    rational<T>::int_type rational<T>::denominator() const { return denom; }

    // Get the largest integer value from the rational value
    template <class T> typename 
    rational<T>::int_type rational<T>::truncate() const { return num / denom; }

    // returns true if the rational number is an integer
    template <class T> bool rational<T>::is_integer() const { 
        return num % denom == 0 ? true : false;
    }

    template <class T> rational<T>& rational<T>::operator+=(const rational<T>& rhs) {
	const T lcm = std::lcm(rhs.denominator(), denom);
	const T left_mod = lcm / denom;
	const T right_mod = lcm / rhs.denominator();

	num = left_mod * num + right_mod * rhs.numerator();
	denom = lcm;
	format();
	return *this;
    }

    template <class T> rational<T>& rational<T>::operator-=(const rational<T>& rhs) {
    	const T lcm = std::lcm(rhs.denominator(), denom);
	const T left_mod = lcm / denom;
	const T right_mod = lcm / rhs.denominator();
    
	num = left_mod * num - right_mod * rhs.numerator();
	denom = lcm;
	format();
	return *this;
    }

    template <class T> rational<T>& rational<T>::operator*=(const rational<T>& rhs) {
    	num = num * rhs.numerator();
	denom = denom * rhs.denominator();
	format();
	return *this;
    }

    template <class T> rational<T>& rational<T>::operator/=(const rational<T>& rhs) {
    	if (rhs.numerator() == 0) {
	    num = std::numeric_limits<T>::max();
            denom = 1;
	} else {
	    num = num * rhs.denominator();
	    denom = denom * rhs.numerator();
	}
	
	format();
	return *this;
    }

    // return true if the rational number is equal to zero
    template <class T> bool rational<T>::operator!() const {
    	return num == 0 ? true : false;
    }

    template <class T> bool rational<T>::operator!=(const rational<T>& rhs) const {
        if (num != rhs.numerator() || denom != rhs.denominator()) {
	    return true;
	}

	return false;
    }

    template <class T> bool rational<T>::operator==(const rational<T>& rhs) const {
   	if (num == rhs.numerator() && denom == rhs.denominator()) {
	    return true;
	}

	return false;
    }

    template <class T> bool rational<T>::operator>(const rational<T>& rhs) const {
        const T lcm = std::lcm(denom, rhs.denominator());
	const T left_mod = lcm / denom;
	const T right_mod = lcm / rhs.denominator();

	return (left_mod * num) > (right_mod * rhs.numerator()) ? true : false;	
    }

    template <class T> bool rational<T>::operator<(const rational<T>& rhs) const {
    	const T lcm = std::lcm(denom, rhs.denominator());
	const T left_mod = lcm / denom;
	const T right_mod = lcm / rhs.denominator();

	return (left_mod * num) < (right_mod * rhs.numerator()) ? true : false;	
    }

    template <class T> bool rational<T>::operator>=(const rational<T>& rhs) const {
        const T lcm = std::lcm(denom, rhs.denominator());
	const T left_mod = lcm / denom;
	const T right_mod = lcm / rhs.denominator();

	return (left_mod * num) >= (right_mod * rhs.numerator()) ? true : false;	
    }

    template <class T> bool rational<T>::operator<=(const rational<T>& rhs) const {
        const T lcm = std::lcm(denom, rhs.denominator());
	const T left_mod = lcm / denom;
	const T right_mod = lcm / rhs.denominator();

	return (left_mod * num) <= (right_mod * rhs.numerator()) ? true : false;	
    }

    template <class T> rational<T>& rational<T>::operator++() {
    	num += denom;
	format();
	return *this;
    }

    template <class T> rational<T>& rational<T>::operator--() {
    	num -= denom;
	format();
    	return *this;
    }

    template <class T> rational<T> rational<T>::operator++(int) {
    	rational<T> old = *this;
	num += denom;
	format();
	return old;
    }

    template <class T> rational<T> rational<T>::operator--(int) {
    	rational<T> old = *this;
	num -= denom;
	format();
	return old;
    }

    // return a copy of the rational number
    template <class T> rational<T> operator+(const rational<T>& op) {
    	return rational<T>(op.numerator(), op.denominator());
    }

    // return a negative version of the rational number
    template <class T> rational<T> operator-(const rational<T>& op) {
    	return rational<T>(-1 * op.numerator(), op.denominator());
    }

    template <class T>
    rational<T> operator+(const rational<T>& lhs, const rational<T>& rhs) {
    	const T lcm = std::lcm(lhs.denominator(), rhs.denominator());
	const T left_mod = lcm / lhs.denominator();
	const T right_mod = lcm / rhs.denominator();
	return rational<T>(left_mod * lhs.numerator() + right_mod * rhs.numerator(), lcm);
    }

    template <class T>
    rational<T> operator-(const rational<T>& lhs, const rational<T>& rhs) {
	const T lcm = std::lcm(lhs.denominator(), rhs.denominator());
	const T left_mod = lcm / lhs.denominator();
	const T right_mod = lcm / rhs.denominator();
	return rational<T>(left_mod * lhs.numerator() - right_mod * rhs.numerator(), lcm);  
    }

    template <class T>
    rational<T> operator*(const rational<T>& lhs, const rational<T>& rhs) {
   	return rational<T>(lhs.numerator() * rhs.numerator(), lhs.denominator() * rhs.denominator()); 
    }

    // divide 2 rational numbers. If the rhs is 0 then the result will be set to the maximum value for type T
    template <class T>
    rational<T> operator/(const rational<T>& lhs, const rational<T>& rhs) {
	T new_num = 0;
	T new_denom = 0;

    	if (rhs.numerator() == 0) {
	    new_num = std::numeric_limits<T>::max();
            new_denom = 1;
	} else {
	    new_num = lhs.numerator() * rhs.denominator();
	    new_denom = lhs.denominator() * rhs.numerator();
	}
	
	return rational<T>(new_num, new_denom);
    }

    // stream inserter using the format '-?{numerator}/{denominator}'
    template <class T>
    std::ostream& operator<<(std::ostream& stream, const rational<T>& arg) {
    	stream << arg.numerator() << "/" << arg.denominator();
	return stream;
    }


    // stream extractor using the format '-?{numerator}/{denominator}', stream error set otherwise
    template <class T>
    std::istream& operator>>(std::istream& stream, rational<T>& arg) {
	if (!isdigit(stream.peek()) && stream.peek() != '-') {
	    stream.setstate(std::ios_base::failbit);
	    return stream;	   
	}

	char sep;
	stream >> arg.num;
	if (stream.peek() != '/') {
	    stream.setstate(std::ios_base::failbit);
	    return stream;	   
	}
	stream >> sep;
	if (!isdigit(stream.peek()) && stream.peek() != '-') {
	    stream.setstate(std::ios_base::failbit);
	    return stream;	   
	}
	
	stream >> arg.denom;
	if (!stream.eof()) {
	    stream.setstate(std::ios_base::failbit);
	    return stream;	   
	}

	arg.format();
	return stream;
    }
}
