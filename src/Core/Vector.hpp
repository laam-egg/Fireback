#ifndef Vector_INCLUDED
#define Vector_INCLUDED

#include <type_traits>

struct Vector; // forward declaration

using Scalar = float;
Vector operator*(Scalar k, Vector const& v);

struct Vector {
public:
    Scalar x, y;
    Vector();

	// Instead of
	// Vector(Scalar x, Scalar y)
	// I wrote this silly code so that VS2022 would not produce the warning "possible loss of data"
	// when parameters passed in are not Scalar but, say, int.
	template<typename T, typename U,
		typename std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>, bool> = true>
	Vector(T x, U y) {
		this->x = static_cast<Scalar>(x);
		this->y = static_cast<Scalar>(y);
	}

    Vector(Vector const& other);
    Vector operator+(Vector const& other) const;
    Vector operator-(Vector const& other) const;
    Vector operator*(Scalar k) const;
    Vector operator/(Scalar k) const;
    friend Vector operator*(Scalar k, Vector const& v);

    void operator+=(Vector const& other);
    void operator-=(Vector const& other);
    void operator*=(Scalar const& k);

    bool operator==(Vector const& other);
    bool operator!=(Vector const& other);

	Scalar getModule() const;

	void rotate(Scalar angle);

	Scalar getAngleOfSlope() const;

	/**
 	 * ATTENTION: In case vector.x = 0, the current implementation of this function
	 * would return the maximum possible value of Scalar type, in an attempt to
	 * denote INFINITY.
	 */
	Scalar getSlope() const;

	bool isZeroVector() const;
};

inline Scalar abs(Vector const& v) {
	return v.getModule();
}

Vector getRotatedVector(Vector v, Scalar const& angle);

#endif // Vector_INCLUDED
