#pragma once


class Vector3 {
	private:
		double x_, y_, z_;

	public:
		Vector3(double x, double y, double z) : x_(x), y_(y), z_(z) {}

		double getX() { return x_; }
		double getY() { return y_; }
		double getZ() { return z_; }

		void setX(double x) { x_ = x; }
		void setY(double y) { y_ = y; }
		void setZ(double z) { z_ = z; }

		void setAll(double x, double y, double z) { x_ = x; y_ = y; z_ = z; }

		bool operator==(const Vector3& other) const { return x_ == other.x_ && y_ == other.y_ && z_ == other.z_; }
		Vector3 operator/(const Vector3& other) const { return Vector3(x_ / other.x_, y_ / other.y_, z_ / other.z_); }
		Vector3 operator/(const double& number) const { return Vector3(x_ / number, y_ / number, z_ / number); }
};

class Colour { // Make child of Vector3?
	private:
		Vector3 rgba_ = { 0, 0, 0 };

	public:
		static Vector3 HexToRGB(string hex) { return Vector3(0, 0, 0); }
		static string RGBToHex(Vector3 rgb) { return ""; }

		Colour(string hex) {
			setHex(hex);
		}

		Colour(int r, int g, int b, int a = 255) {
			rgba_.setX(r);
			rgba_.setY(g);
			rgba_.setZ(b);
		}

		void setHex(string hex) {
			rgba_ = HexToRGB(hex);
		}

		string getHex() { return ""; }
		Vector3 getRGB() { return rgba_; }
		Vector3 getRGBA() { return rgba_; }
		Vector3 getRGBf() { return rgba_ / 255.0; }
		Vector3 getRGBAf() { return rgba_ / 255.0; }

		bool operator==(const Colour& other) const { return rgba_ == other.rgba_; }
};

// Temp
class Vector2 : Vector3 {
public:
	Vector2(double x, double y) : Vector3(x, y, 0) {}
};