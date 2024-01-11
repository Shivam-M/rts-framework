#ifndef COLOUR_2_H
#define COLOUR_2_H
#pragma once


struct Vector2 {
	double x, y;
	Vector2(double x = 0.0, double y = 0.0) : x(x), y(y) {}
	void set(double x_value, double y_value) { x = x_value, y_value; }
};

class Vector4 {
	private:
		double x_, y_, z_, w_;

	public:
		Vector4(double x = 0.0, double y = 0.0, double z = 0.0, double w = 0.0) : x_(x), y_(y), z_(z), w_(w) {}

		double getX() { return x_; }
		double getY() { return y_; }
		double getZ() { return z_; }
		double getW() { return w_; }

		void setX(double x) { x_ = x; }
		void setY(double y) { y_ = y; }
		void setZ(double z) { z_ = z; }
		void setW(double w) { w_ = w; }

		void setAll(double x, double y, double z, double w) { x_ = x; y_ = y; z_ = z; w_ = w; }

		Vector4 operator+(const Vector4& other) const { return Vector4(x_ + other.x_, y_ + other.y_, z_ + other.z_, w_ + other.w_); }
		Vector4 operator+(const double& number) const { return Vector4(x_ + number, y_ + number, z_ + number, w_ + number); }

		Vector4 operator-(const Vector4& other) const { return Vector4(x_ - other.x_, y_ - other.y_, z_ - other.z_, w_ - other.w_); }
		Vector4 operator-(const double& number) const { return Vector4(x_ - number, y_ - number, z_ - number, w_ - number); }

		Vector4 operator*(const Vector4& other) const { return Vector4(x_ * other.x_, y_ * other.y_, z_ * other.z_, w_ * other.w_); }
		Vector4 operator*(const double& number) const { return Vector4(x_ * number, y_ * number, z_ * number, w_ * number); }

		Vector4 operator/(const Vector4& other) const { return Vector4(x_ / other.x_, y_ / other.y_, z_ / other.z_, w_ / other.w_); }
		Vector4 operator/(const double& number) const { return Vector4(x_ / number, y_ / number, z_ / number, w_ / number); }
		bool operator==(const Vector4& other) const { return x_ == other.x_ && y_ == other.y_ && z_ == other.z_ && w_ == other.w_; }
		bool operator!=(const Vector4& other) const { return x_ != other.x_ || y_ != other.y_ || z_ != other.z_ || w_ != other.w_; }

		double magnitude() const { return sqrt(x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_); }

		Vector4 normalise() {
			double mag = magnitude();
			return (mag != 0.0) ? *this / mag : *this;
		}
};

class Colour2: public Vector4 { 
	public:
		static Vector4 HexToRGB(string hex) { return Vector4(0, 0, 0, 255); }
		static string RGBToHex(Vector4 rgb) { return ""; }

		Colour2(const Vector4& vec) : Vector4(vec) {}

		Colour2() {}

		Colour2(string hex) {
			setHex(hex);
		}

		Colour2(short r, short g, short b, short a = 255) {
			setX(r);
			setY(g);
			setZ(b);
			setW(a);
		}

		void setHex(string hex) {
			// rgba_ = HexToRGB(hex);
		}

		string getHex() { return ""; }
		Vector4 getRGBAf() { return *this / 255.0; }
};

#endif