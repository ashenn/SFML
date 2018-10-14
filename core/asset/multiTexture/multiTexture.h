#ifndef MULTI_TEXTURE_H
#define MULTI_TEXTURE_H

#include "../asset.h"

class MultiTexture
{
	private:
		static void loadSprites(ListManager* sprites, sf::Texture* text, sf::Image* image);

	public:
		static sf::Texture* load(const char* path, const char* name);
};

#endif