#pragma once

#include <string>

using namespace std;

struct Vector2 {
	float x, y;
	Vector2(const float& x = 0.0f, const float& y = 0.0f) : x(x), y(y) {}
	void set(const float& x_value, const float& y_value) { x = x_value, y = y_value; }
	Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
	Vector2& operator+=(const float& number) { x += number;  y += number;  return *this; }
	Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
	Vector2& operator-=(const float& number) { x -= number;  y -= number;  return *this; }
	Vector2& operator*=(const Vector2& other) { x *= other.x; y *= other.y; return *this; }
	Vector2& operator*=(const float& number) { x *= number;  y *= number;  return *this; }
	Vector2& operator/=(const Vector2& other) { x /= other.x; y /= other.y; return *this; }
	Vector2& operator/=(const float& number) { x /= number;  y /= number;  return *this; }

	Vector2 operator+(const Vector2& other) const { Vector2 result = *this; result += other; return result; }
	Vector2 operator+(const float& number)  const { Vector2 result = *this; result += number; return result; }
	Vector2 operator-(const Vector2& other) const { Vector2 result = *this; result -= other; return result; }
	Vector2 operator-(const float& number)  const { Vector2 result = *this; result -= number; return result; }
	Vector2 operator*(const Vector2& other) const { Vector2 result = *this; result *= other; return result; }
	Vector2 operator*(const float& number)  const { Vector2 result = *this; result *= number; return result; }
	Vector2 operator/(const Vector2& other) const { Vector2 result = *this; result /= other; return result; }
	Vector2 operator/(const float& number)  const { Vector2 result = *this; result /= number; return result; }
};

class Vector4 {
	private:
		

	public:
		float x_, y_, z_, w_;

		Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f) : x_(x), y_(y), z_(z), w_(w) {}

		float get_x() const { return x_; }
		float get_y() const { return y_; }
		float get_z() const { return z_; }
		float get_w() const { return w_; }

		void set_x(float x) { x_ = x; }
		void set_y(float y) { y_ = y; }
		void set_z(float z) { z_ = z; }
		void set_alpha(float w) { w_ = w; }

		void set_all(float x, float y, float z, float w) { x_ = x; y_ = y; z_ = z; w_ = w; }

		Vector4& operator+=(const Vector4& other) {
			x_ += other.x_; y_ += other.y_; z_ += other.z_; w_ += other.w_;
			return *this;
		}

		Vector4& operator+=(const float& number) {
			x_ += number; y_ += number; z_ += number; w_ += number;
			return *this;
		}

		Vector4& operator-=(const Vector4& other) {
			x_ -= other.x_; y_ -= other.y_; z_ -= other.z_; w_ -= other.w_;
			return *this;
		}

		Vector4& operator-=(const float& number) {
			x_ -= number; y_ -= number; z_ -= number; w_ -= number;
			return *this;
		}

		Vector4& operator*=(const Vector4& other) {
			x_ *= other.x_; y_ *= other.y_; z_ *= other.z_; w_ *= other.w_;
			return *this;
		}

		Vector4& operator*=(const float& number) {
			x_ *= number; y_ *= number; z_ *= number; w_ *= number;
			return *this;
		}

		Vector4& operator/=(const Vector4& other) {
			x_ /= other.x_; y_ /= other.y_; z_ /= other.z_; w_ /= other.w_;
			return *this;
		}

		Vector4& operator/=(const float& number) {
			x_ /= number; y_ /= number; z_ /= number; w_ /= number;
			return *this;
		}

		Vector4 operator+(const Vector4& other) const { Vector4 result = *this; result += other; return result; }
		Vector4 operator+(const float& number) const { Vector4 result = *this; result += number; return result; }
		Vector4 operator-(const Vector4& other) const { Vector4 result = *this; result -= other; return result; }
		Vector4 operator-(const float& number) const { Vector4 result = *this; result -= number; return result; }
		Vector4 operator*(const Vector4& other) const { Vector4 result = *this; result *= other; return result; }
		Vector4 operator*(const float& number) const { Vector4 result = *this; result *= number; return result; }
		Vector4 operator/(const Vector4& other) const { Vector4 result = *this; result /= other; return result; }
		Vector4 operator/(const float& number) const { Vector4 result = *this; result /= number; return result; }

		bool operator==(const Vector4& other) const { return x_ == other.x_ && y_ == other.y_ && z_ == other.z_ && w_ == other.w_; }
		bool operator!=(const Vector4& other) const { return x_ != other.x_ || y_ != other.y_ || z_ != other.z_ || w_ != other.w_; }
		
		bool operator<(const Vector4& other) const { return sum() < other.sum(); }
		bool operator<=(const Vector4& other) const { return sum() <= other.sum(); }

		bool operator>(const Vector4& other) const { return sum() > other.sum(); }
		bool operator>=(const Vector4& other) const { return sum() >= other.sum(); }
		
		/*bool operator<(const Vector4& other) const { return x_ < other.x_ && y_ < other.y_ && z_ < other.z_ && w_ < other.w_; }
		bool operator<=(const Vector4& other) const { return x_ <= other.x_ && y_ <= other.y_ && z_ <= other.z_ && w_ <= other.w_; }

		bool operator>(const Vector4& other) const { return x_ > other.x_ && y_ > other.y_ && z_ > other.z_ && w_ > other.w_; }
		bool operator>=(const Vector4& other) const { return x_ >= other.x_ && y_ >= other.y_ && z_ >= other.z_ && w_ >= other.w_; }*/

		float magnitude() const { return sqrt(x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_); } // rm sqrt
		float magnitude2() const {
			float mag = 0.f;
			mag += x_ <= 0 ? -(x_ * x_) : (x_ * x_);
			mag += y_ <= 0 ? -(y_ * y_) : (y_ * y_);
			mag += z_ <= 0 ? -(z_ * z_) : (z_ * z_);
			mag += w_ <= 0 ? -(w_ * w_) : (w_ * w_);
			return sqrt(mag);
		}

		float sum() const {
			return x_ + y_ + z_ + w_;
		}

		Vector4 normalise() {
			float mag = magnitude();
			return (mag != 0.0f) ? *this / mag : *this;
		}
};

#define COLOUR_WHITE Colour(255, 255, 255, 255)
#define COLOUR_BLACK Colour(0, 0, 0, 255)
#define COLOUR_INVIS Colour(0, 0, 0, 0)
#define COLOUR_NULL  Colour(-1, -1, -1, -1)

class Colour: public Vector4 { 
	public:
		static Colour hex_to_rgb(const string& hex, const float& alpha = 1.0f);
		static string rgb_to_hex(Vector4 rgb);

		Colour(const Vector4& vec) : Vector4(vec) {}

		Colour() {}

		Colour(const string& hex, const float& alpha = 1.0f) { *this = hex_to_rgb(hex, alpha); }

		Colour(const float& r, const float& g, const float& b, const float& a = 255.0f) { set_x(r).set_y(g).set_z(b).set_alpha(a); }

		Colour& set_x(const float& x) {
			x_ = x;
			return *this;
		}

		Colour& set_y(const float& y) {
			y_ = y;
			return *this;
		}

		Colour& set_z(const float& z) {
			z_ = z;
			return *this;
		}

		Colour& set_alpha(const float& w) {
			w_ = w;
			return *this;
		}

		bool is_null() const {
			return (x_ <= -1 && y_ <= -1 && z_ <= -1 && w_ <= -1);
		}

		string get_hex() { return rgb_to_hex(*this); }
		Vector4 get_float() { return *this / 255.0f; }
};
