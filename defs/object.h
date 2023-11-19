#pragma once

class Object {
	private:
		int identifier_ = -1;
		string name_ = "Generic Object";
		Colour colour_ = Colour(255, 255, 255);

	public:
		void setID(int identifier) { identifier_ = identifier; }
		int getID() { return identifier_; }

		string getName() { return name_; }
		void setName(string name) { name_ = name; }

		Colour getColour() { return colour_; }
		void setColour(Colour colour) { colour_ = colour; }

		void evaluate();
};