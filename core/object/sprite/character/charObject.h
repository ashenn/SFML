#ifndef CHAR_OBJ_H
#define CHAR_OBJ_H

#include "../spriteObject.h"
#include "../../../character/character.h"

class CharObj : public SpriteObj
{
	protected:
		Character* ch = NULL;

	public:
		CharObj(const char* name, vector* pos, unsigned short z, const char* path, Character* ch);
		~CharObj();

		virtual void draw(RenderWindow* window);	
};

#endif