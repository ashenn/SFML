#ifndef TEST_OBJECT_H
#define TEST_OBJECT_H

#include "../object.h"

class TextObj : public Object
{

	char* text = NULL;
	sf::Text* textObj = NULL;

	unsigned int size = 10;
	sf::Font* font = NULL;
	sf::Color color = sf::Color::White;

	public:
		virtual void draw(RenderWindow* window, bool grav);
		TextObj(const char* name, vector* pos, const char* text, const char* font, unsigned int fontSize, unsigned short z, bool visible);
		TextObj(const char* name, vector* pos, const char* text, const char* font, unsigned int fontSize, unsigned short z, bool visible, sf::Color color);
		~TextObj();
	
};

#endif