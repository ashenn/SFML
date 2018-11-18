#ifndef CHAR_OBJ_H
#define CHAR_OBJ_H

#include "../spriteObject.h"
#include "../../../character/character.h"

class CharObj : public SpriteObj
{
	protected:
		Character* ch = NULL;

	public:
		~CharObj();
		CharObj(const char* name, vector* pos, unsigned short z, const char* path, Character* ch);

		Character* getCharacter();
		virtual vector canMove(vector m);

		virtual void draw(RenderWindow* window, bool grav);		
};

#endif