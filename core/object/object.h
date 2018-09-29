#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics.hpp>
#include "../../base/math.h"
#include "../abstractClass.h"

using namespace sf;

class Object : AbstractClass
{
	private:
		int lifetime = -1;
		unsigned short z = 0;

		bool visible = true;
		bool enabled = false;

		IntRect clip;
		Object* parent = NULL;

		Sprite* sprite = NULL;
		Texture* texture = NULL;

	protected:

	public:
		Object(const char* name, Texture* text, IntRect* clip, unsigned short z, bool visible);
		
		Object(const char* name) : Object(name, NULL, NULL, 0, false){};
		Object(const char* name, Texture* text, IntRect* clip) : Object(name, text, clip, 0, false){};
		Object(const char* name, Texture* text, IntRect* clip, unsigned short z) : Object(name, text, clip, z, false){};

		~Object();

		Sprite* getSprite();
		void removeTexture();
		void setTexture(Texture* text);

		/*
		void move(vector move);
		void setPos(vector pos);
		*/
};

#endif