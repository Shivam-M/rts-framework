#include "panel.h"
#include "text.h"
#include "../tools/fonts.h"

class GeneralTooltip : public Panel {
	private:
		Moveable* background_ = new Moveable({ 0.0, 0.0 }, { 0.35, 0.2 }, Colour(255, 0, 0, 100), Colour(255, 0, 0, 100));
		Text* title_ = new Text({0.05, 0.05}, Fonts::getFont(COURIER_NEW, 12), Colour(255, 255, 255, 255), "Sample Text");

	public:
		GeneralTooltip() {
			add(background_);
			add((Moveable*)title_);
		}

		void setTitle(string content) {
			stringstream ss;
			ss << setw(15) << content;
			title_->setContent(ss.str());
		}
};