#pragma once

struct Vector2 {
	float x, y;
	Vector2(float x = 0.0, float y = 0.0) : x(x), y(y) {}
	void set(float x_value, float y_value) { x = x_value, y = y_value; }
	Vector2 operator*(const float& number) { return Vector2(x * number, y * number); }
	Vector2 operator+(const float& number) { return Vector2(x + number, y + number); }
	Vector2 operator+(const Vector2& other) { return Vector2(x + other.x, y + other.y); }
};

class Vector4 {
	private:
		

	public:
		float x_, y_, z_, w_;

		Vector4(float x = 0.0, float y = 0.0, float z = 0.0, float w = 0.0) : x_(x), y_(y), z_(z), w_(w) {}

		float getX() { return x_; }
		float getY() { return y_; }
		float getZ() { return z_; }
		float getW() { return w_; }

		void setX(float x) { x_ = x; }
		void setY(float y) { y_ = y; }
		void setZ(float z) { z_ = z; }
		void setW(float w) { w_ = w; }

		void setAll(float x, float y, float z, float w) { x_ = x; y_ = y; z_ = z; w_ = w; }

		Vector4 operator+(const Vector4& other) const { return Vector4(x_ + other.x_, y_ + other.y_, z_ + other.z_, w_ + other.w_); }
		Vector4 operator+(const float& number) const { return Vector4(x_ + number, y_ + number, z_ + number, w_ + number); }

		Vector4 operator-(const Vector4& other) const { return Vector4(x_ - other.x_, y_ - other.y_, z_ - other.z_, w_ - other.w_); }
		Vector4 operator-(const float& number) const { return Vector4(x_ - number, y_ - number, z_ - number, w_ - number); }

		Vector4 operator*(const Vector4& other) const { return Vector4(x_ * other.x_, y_ * other.y_, z_ * other.z_, w_ * other.w_); }
		Vector4 operator*(const float& number) const { return Vector4(x_ * number, y_ * number, z_ * number, w_ * number); }

		Vector4 operator/(const Vector4& other) const { return Vector4(x_ / other.x_, y_ / other.y_, z_ / other.z_, w_ / other.w_); }
		Vector4 operator/(const float& number) const { return Vector4(x_ / number, y_ / number, z_ / number, w_ / number); }

		bool operator==(const Vector4& other) const { return x_ == other.x_ && y_ == other.y_ && z_ == other.z_ && w_ == other.w_; }
		bool operator!=(const Vector4& other) const { return x_ != other.x_ || y_ != other.y_ || z_ != other.z_ || w_ != other.w_; }

		bool operator<(const Vector4& other) const { return magnitude2() < other.magnitude2(); }
		bool operator<=(const Vector4& other) const { return magnitude2() <= other.magnitude2(); }

		bool operator>(const Vector4& other) const { return magnitude2() > other.magnitude2(); }
		bool operator>=(const Vector4& other) const { return magnitude2() >= other.magnitude2(); }

		/*
		bool operator<(const Vector4& other) const { return x_ < other.x_ && y_ < other.y_ && z_ < other.z_ && w_ < other.w_; }
		bool operator<=(const Vector4& other) const { return x_ <= other.x_ && y_ <= other.y_ && z_ <= other.z_ && w_ <= other.w_; }

		bool operator>(const Vector4& other) const { return x_ > other.x_ && y_ > other.y_ && z_ > other.z_ && w_ > other.w_; }
		bool operator>=(const Vector4& other) const { return x_ >= other.x_ && y_ >= other.y_ && z_ >= other.z_ && w_ >= other.w_; }
		*/

		float magnitude() const { return sqrt(x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_); } // rm sqrt
		float magnitude2() const {
			float mag = 0.f;
			mag += x_ <= 0 ? -(x_ * x_) : (x_ * x_);
			mag += y_ <= 0 ? -(y_ * y_) : (y_ * y_);
			mag += z_ <= 0 ? -(z_ * z_) : (z_ * z_);
			mag += w_ <= 0 ? -(w_ * w_) : (w_ * w_);
			return sqrt(mag);
		}

		Vector4 normalise() {
			float mag = magnitude();
			return (mag != 0.0) ? *this / mag : *this;
		}
};

#define COLOUR_WHITE Colour(255, 255, 255, 255)
#define COLOUR_BLACK Colour(0, 0, 0, 255)
#define COLOUR_INVIS Colour(0, 0, 0, 0)

class Colour: public Vector4 { 
	public:
		static Colour HexToRGB(string hex, float alpha = 1.0f) { 
			int r, g, b;
			sscanf_s(hex.c_str(), "%02x%02x%02x", &r, &g, &b);
			return Colour(r, g, b, alpha * 255); 
		}
		static string RGBToHex(Vector4 rgb) { return ""; }

		Colour(const Vector4& vec) : Vector4(vec) {}

		Colour() {}

		Colour(string hex, float alpha = 1) {
			*this = HexToRGB(hex, alpha);
		}

		Colour(float r, float g, float b, float a = 255.f) {
			setX(r);
			setY(g);
			setZ(b);
			setW(a);
		}

		string getHex() { return ""; }
		Vector4 getRGBAf() { return *this / 255.0f; }
};
