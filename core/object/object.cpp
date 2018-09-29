#include "object.h"

Object::Object(const char* name, Texture* text, IntRect* clip, unsigned short z, bool visible) {
	if (name == NULL || !strlen(name)) {
		Log::war(LOG_OBJ, "Creating Object With Empty Name");
	}

	Log::inf(LOG_OBJ, "=== Creating Object: %s ===", name);
	this->name = Str(name);

	
	Log::dbg(LOG_OBJ, "-- clip:");
	Log::dbg(LOG_OBJ, "++++ X: %d", clip->left);
	Log::dbg(LOG_OBJ, "++++ Y: %d", clip->top);
	Log::dbg(LOG_OBJ, "++++ W: %d", clip->width);
	Log::dbg(LOG_OBJ, "++++ H: %d", clip->height);


	this->z = z;
	this->clip = *clip;
	this->texture = text;
	this->visible = visible;

	if (text != NULL) {
		this->setTexture(text);
	}
	
	Log::dbg(LOG_OBJ, "=== Ready Object #%d: %s ===", this->id, this->name);
}

Object::~Object() {
	Log::inf(LOG_OBJ, "=== Deleting Object #%d: %s ===", this->id, this->name);

	this->removeTexture();
	if (this->sprite != NULL) {
	}
}

void Object::removeTexture() {
	if (this->sprite != NULL) {
		Log::dbg(LOG_OBJ, "-- Deleting Old Texture");
		delete this->sprite;
	}
}

void Object::setTexture(Texture* text) {
	bool b = this->lock("Set texture");

	Log::dbg(LOG_OBJ, "-- Setting Texture: %p", text);
	this->removeTexture();

	this->texture = text;
	if (this->texture != NULL) {
		Log::dbg(LOG_OBJ, "-- Applying New Texture");
		this->sprite = new Sprite(*this->texture, this->clip);
	}

	this->unlock("Set texture", b);
}

Sprite* Object::getSprite() {
	return this->sprite;
}