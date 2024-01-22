#include "panel.h"
#include "text.h"
#include "../tools/fonts.h"

class GeneralTooltip : public Panel {
	private:
		// Font* title_font = Fonts::getFont(CONSOLAS_BOLD, 18);
		// Font* subtitle_font = Fonts::getFont(CONSOLAS_BOLD, 12);

		string fn = "data/fonts/Cinzel-Regular.ttf";

		Font* title_font = Fonts::getFont(fn, 32, true);
		Font* subtitle_font = Fonts::getFont(fn, 20, true);

		Moveable* background_ = new Moveable({ 0.0, 0.0 }, { 0.20, 0.175 }, Colour(40, 40, 40, 150), Colour(40, 40, 40, 150));
		Text* title_ = new Text({0.01, 0.05}, title_font, Colour(255, 255, 255, 255), "London", 0.5f);
		Text* subtitle_ = new Text({ 0.01, 0.085 }, subtitle_font, Colour(255, 255, 255, 255), "Owned by: Mercia", 0.5f);
		

	public:

		Text* subtitle2_ = new Text({ 0.01, 0.11 }, subtitle_font, Colour(255, 255, 255, 255), "Value: 2.32", 0.5f);
		Text* subtitle3_ = new Text({ 0.01, 0.135 }, subtitle_font, Colour(255, 255, 255, 255), "Terrain: Hills", 0.5f);

		GeneralTooltip() : Panel() {

			background_->addFlag(CURVED);
			background_->addFlag(UNEDITABLE);
			title_->addFlag(UNEDITABLE);
			subtitle_->addFlag(UNEDITABLE);
			subtitle2_->addFlag(UNEDITABLE);
			subtitle3_->addFlag(UNEDITABLE);

			add(background_);
			add((Moveable*)title_);
			add((Moveable*)subtitle_);
			add((Moveable*)subtitle2_);
			add((Moveable*)subtitle3_);
			setSize(background_->size.x, background_->size.y);
		}

		void setTitle(string content) {
			title_->setContent(content);
		}

		void setSubtitle(string content) {
			subtitle_->setContent(content);
		}

};