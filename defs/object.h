#pragma once

class Object {
	private:
		int identifier_ = -1;
		string name_ = "Generic Object";
		Colour2 colour_ = Colour2(255, 255, 255);

	public:
		void setID(int identifier) { identifier_ = identifier; }
		int getID() { return identifier_; }

		string getName() { return name_; }
		void setName(string name) { name_ = name; }

		Colour2 getColour() { return colour_; }
		void setColour(Colour2 colour) { colour_ = colour; }

		void evaluate();
};