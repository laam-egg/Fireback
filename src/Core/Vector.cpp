#include "Vector.hpp"
#include <cmath>


Vector::Vector()
    : Vector(0, 0) {
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

Scalar Vector::getModule() const {
    return sqrt(x * x + y * y);
}

void Vector::rotate(Scalar a) {
	Scalar X = x;
	Scalar Y = y;
	x = X * cos(a) - Y * sin(a);
	y = Y * cos(a) + X * sin(a);
}

Scalar Vector::getAngleOfSlope() const {
    return atan2(y, x);
}

#include <limits>
Scalar Vector::getSlope() const {
	if (x == 0) return std::numeric_limits<Scalar>::max();
	return y / x;
}

bool Vector::isZeroVector() const {
	return (x == 0 && y == 0);
}

Vector getRotatedVector(Vector v, Scalar const& angle) {
	v.rotate(angle);
	return v;
}
