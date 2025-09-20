#pragma once

#include <string>

using namespace std;

struct Vector2 {
	float x, y;

	constexpr Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

	Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
	Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
	Vector2& operator*=(const Vector2& other) { x *= other.x; y *= other.y; return *this; }
	Vector2& operator/=(const Vector2& other) { x /= other.x; y /= other.y; return *this; }

	Vector2& operator+=(float number) { x += number;  y += number; return *this; }
	Vector2& operator-=(float number) { x -= number;  y -= number; return *this; }
	Vector2& operator*=(float number) { x *= number;  y *= number; return *this; }
	Vector2& operator/=(float number) { x /= number;  y /= number; return *this; }

	Vector2 operator+(const Vector2& other) const { return { x + other.x, y + other.y }; }
	Vector2 operator-(const Vector2& other) const { return { x - other.x, y - other.y }; }
	Vector2 operator*(const Vector2& other) const { return { x * other.x, y * other.y }; }
	Vector2 operator/(const Vector2& other) const { return { x / other.x, y / other.y }; }

	Vector2 operator+(float number) const { return { x + number, y + number }; }
	Vector2 operator-(float number) const { return { x - number, y - number }; }
	Vector2 operator*(float number) const { return { x * number, y * number }; }
	Vector2 operator/(float number) const { return { x / number, y / number }; }
};


struct Vector4 {
	float x, y, z, w;

	constexpr Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f) : x(x), y(y), z(z), w(w) {}
	float magnitude() const { return x * x + y * y + z * z + w * w; }
	float sum() const { return x + y + z + w; }
	Vector4 normalise() {
		float mag = magnitude();
		return (mag != 0.0f) ? *this / mag : *this;
	}

	Vector4& operator+=(const Vector4& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this;}
	Vector4& operator-=(const Vector4& other) {x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this;}
	Vector4& operator*=(const Vector4& other) { x *= other.x; y *= other.y; z *= other.z; w *= other.w; return *this;}
	Vector4& operator/=(const Vector4& other) { x /= other.x; y /= other.y; z /= other.z; w /= other.w; return *this; }

	Vector4& operator+=(float number) { x += number; y += number; z += number; w += number; return *this; }
	Vector4& operator-=(float number) { x -= number; y -= number; z -= number; w -= number; return *this; }
	Vector4& operator*=(float number) { x *= number; y *= number; z *= number; w *= number; return *this; }
	Vector4& operator/=(float number) { x /= number; y /= number; z /= number; w /= number; return *this; }

	Vector4 operator+(const Vector4& other) const { return { x + other.x, y + other.y, z + other.z, w + other.w }; }
	Vector4 operator-(const Vector4& other) const { return { x - other.x, y - other.y, z - other.z, w - other.w }; }
	Vector4 operator*(const Vector4& other) const { return { x * other.x, y * other.y, z * other.z, w * other.w }; }
	Vector4 operator/(const Vector4& other) const { return { x / other.x, y / other.y, z / other.z, w / other.w }; }

	Vector4 operator+(float number) const { return { x + number, y + number, z + number, w + number }; }
	Vector4 operator-(float number) const { return { x - number, y - number, z - number, w - number }; }
	Vector4 operator*(float number) const { return { x * number, y * number, z * number, w * number }; }
	Vector4 operator/(float number) const { return { x / number, y / number, z / number, w / number }; }

	bool operator==(const Vector4& other) const { return x == other.x && y == other.y && z == other.z && w == other.w;}
	bool operator!=(const Vector4& other) const { return !(*this == other); }
	bool operator<(const Vector4& other) const { return sum() < other.sum(); }
	bool operator<=(const Vector4& other) const { return sum() <= other.sum(); }
	bool operator>(const Vector4& other) const { return sum() > other.sum(); }
	bool operator>=(const Vector4& other) const { return sum() >= other.sum(); }
};


struct Colour : public Vector4 {
	using Vector4::Vector4;

	float& r = x;
	float& g = y;
	float& b = z;
	float& a = w;

	static Colour from_hex(const string& hex, float alpha = 1.0f);
	static string to_hex(const Colour& rgb);

	Colour(const string& hex, float alpha = 255.0f) : Colour(from_hex(hex, alpha)) {}
	Colour(const Colour& other) : Vector4(other), r(x), g(y), b(z), a(w) {}
	Colour(const Vector4& vector) : Vector4(vector), r(x), g(y), b(z), a(w) {}

	Colour& operator=(const Colour& other) { Vector4::operator=(other); return *this; }

	bool is_null() const { return r <= -1 && g <= -1 && b <= -1 && a <= -1; }
	string as_hex() const { return to_hex(*this); }
	Colour with_alpha(float alpha) const { return Colour(r, g, b, alpha); }
};


constexpr Colour COLOUR_WHITE { 255, 255, 255, 255 };
constexpr Colour COLOUR_BLACK { 0, 0, 0, 255 };
constexpr Colour COLOUR_NULL { -1, -1, -1, -1 };
