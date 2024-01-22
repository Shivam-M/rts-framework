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

	public:
		Moveable* background_ = new Moveable({ 0.0, 0.0 }, { 0.20, 0.175 }, Colour(40, 40, 40, 150), Colour(0, 0, 0, 0));
		Text* title = new Text({ 0.01, 0.05 }, title_font, Colour(255, 255, 255, 255), "London", 0.5f);
		Text* subtitle = new Text({ 0.01, 0.085 }, subtitle_font, Colour(255, 255, 255, 255), "Owned by: Mercia", 0.5f);
		Text* subtitle2 = new Text({ 0.01, 0.11 }, subtitle_font, Colour(255, 255, 255, 255), "Value: 2.32", 0.5f);
		Text* subtitle3 = new Text({ 0.01, 0.135 }, subtitle_font, Colour(255, 255, 255, 255), "Terrain: Hills", 0.5f);

		GeneralTooltip() : Panel() {
			background_->addFlag(CURVED);
			add(background_);
			add(reinterpret_cast<Moveable*>(title));
			add(reinterpret_cast<Moveable*>(subtitle));
			add(reinterpret_cast<Moveable*>(subtitle2));
			add(reinterpret_cast<Moveable*>(subtitle3));
			setSize(background_->size.x, background_->size.y);
		}

		void setTitle(string content) {
			title->setContent(content);
		}

		void setSubtitle(string content) {
			subtitle->setContent(content);
		}

};
