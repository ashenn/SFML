#include "charObject.h"

CharObj::CharObj(const char* name, vector* pos, unsigned short z, const char* path, Character* ch) : 
SpriteObj(name, pos, z, path) {
	this->ch = ch;
}

CharObj::~CharObj() {

}

void CharObj::draw(RenderWindow* window) {
	Object::draw(window);

	if (this->ch == NULL) {
		return;
	}

	this->ch->update();
}