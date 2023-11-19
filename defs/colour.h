#pragma once

class Vector3 {
	private:
		int x_, y_, z_;

	public:
		Vector3(int x, int y, int z) : x_(x), y_(y), z_(z) {}

		int getX() { return x_; }
		int getY() { return y_; }
		int getZ() { return z_; }

		void setX(int x) { x_ = x; }
		void setY(int y) { y_ = y; }
		void setZ(int z) { z_ = z; }

		void setAll(int x, int y, int z) { x_ = x; y_ = y; z_ = z; }

		bool operator==(const Vector3& other) const { return x_ == other.x_ && y_ == other.y_ && z_ == other.z_; }
};

class Colour {
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

		bool operator==(const Colour& other) const { return rgba_ == other.rgba_; }
};
