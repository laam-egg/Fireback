#ifndef Vector_INCLUDED
#define Vector_INCLUDED


struct Vector; // forward declaration

using Scalar = float;
Vector operator*(Scalar k, Vector const& v);

struct Vector {
public:
    Scalar x, y;
    Vector();
    Vector(Scalar x, Scalar y);
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
};

inline Scalar abs(Vector const& v) {
	return v.getModule();
}

Vector getRotatedVector(Vector v, Scalar const& angle);

#endif // Vector_INCLUDED
