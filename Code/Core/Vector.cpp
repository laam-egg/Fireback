#include "Vector.hpp"
#include <cmath>


Vector::Vector()
    : Vector(0, 0) {
}

Vector::Vector(Scalar x, Scalar y) {
    this->x = x;
    this->y = y;
}

Vector::Vector(Vector const& other) {
    x = other.x;
    y = other.y;
}

Vector Vector::operator+(Vector const& other) const {
    return Vector(x + other.x, y + other.y);
}

Vector Vector::operator-(Vector const& other) const {
    return Vector(x - other.x, y - other.y);
}

Vector Vector::operator*(Scalar k) const {
    return Vector(x * k, y * k);
}

Vector Vector::operator/(Scalar k) const {
    return Vector(x / k, y / k);
}

Vector operator*(Scalar k, Vector const& v) {
    return v * k;
}

void Vector::operator+=(Vector const& other) {
    x += other.x;
    y += other.y;
}

void Vector::operator-=(Vector const& other) {
    x -= other.x;
    y -= other.y;
}

void Vector::operator*=(Scalar const& k) {
    x *= k;
    y *= k;
}

bool Vector::operator==(Vector const& other) {
    return (x == other.x) && (y == other.y);
}

bool Vector::operator!=(Vector const& other) {
    return !(*this == other);
}

#include <type_traits>
Scalar Vector::getModule() const {
	if constexpr (std::is_same_v<Scalar, float>) {
		return std::sqrtf(x * x + y * y);
	} else {
		return std::sqrt(x * x + y * y);
	}
}

void Vector::rotate(Scalar a) {
	Scalar X = x;
	Scalar Y = y;
	x = X * cos(a) - Y * sin(a);
	y = Y * cos(a) + X * sin(a);
}

Scalar Vector::getAngleOfSlope() const {
	if (x == 0) return 0; // not really, but suit our purpose.
	if constexpr (std::is_same_v<Scalar, float>) {
		return std::atan2f(y, x);
	} else {
		return std::atan2(y, x);
	}
}

Vector getRotatedVector(Vector v, Scalar const& angle) {
	v.rotate(angle);
	return v;
}
