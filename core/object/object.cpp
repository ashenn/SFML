#include "object.h"
#include "../render/render.h"

Object::Object(const char* name, vector* pos, Texture* text, IntRect* clip, unsigned short z, bool visible) {
	if (name == NULL || !strlen(name)) {
		Log::war(LOG_OBJ, "Creating Object With Empty Name");
	}

	Log::inf(LOG_OBJ, "=== Creating Object: %s ===", name);
	this->name = Str(name);



	this->z = z;
	this->texture = text;
	this->visible = visible;

	if (pos != NULL) {
		this->pos = *pos;
	}
	else {
		this->pos.x = 0;
		this->pos.y = 0;
	}

	Log::dbg(LOG_OBJ, "-- Pos:");
	Log::dbg(LOG_OBJ, "++++ X: %d", this->pos.x);
	Log::dbg(LOG_OBJ, "++++ Y: %d", this->pos.y);


	if (clip != NULL) {
		this->clip = *clip;
	}
	else {
		this->clip.top = 0;
		this->clip.left = 0;
		this->clip.width = 50;
		this->clip.height = 50;
	}
	
	Log::dbg(LOG_OBJ, "-- Clip:");
	Log::dbg(LOG_OBJ, "++++ X: %d", this->clip.left);
	Log::dbg(LOG_OBJ, "++++ Y: %d", this->clip.top);
	Log::dbg(LOG_OBJ, "++++ W: %d", this->clip.width);
	Log::dbg(LOG_OBJ, "++++ H: %d", this->clip.height);

	if (text != NULL) {
		this->setTexture(text);
	}
	
	Log::dbg(LOG_OBJ, "=== Ready Object #%d: %s ===", this->id, this->name);
}

Object::~Object() {
	Log::inf(LOG_OBJ, "=== Deleting Object #%d: %s ===", this->id, this->name);

	this->removeTexture();
}

void Object::removeTexture() {
	bool b = this->lock("Remove texture");

	if (this->sprite != NULL) {
		Log::dbg(LOG_OBJ, "-- Deleting Sprite");
		delete this->sprite;
	}

	this->unlock("Remove texture", b);
}

void Object::setTexture(Texture* text) {
	bool b = this->lock("Set texture");

	Log::war(LOG_OBJ, "-- Setting Texture: %p", text);
	
	this->removeTexture();
	this->texture = text;


	if (this->texture != NULL) {
		Log::war(LOG_OBJ, "-- Applying New Texture");
		this->sprite = new Sprite(*this->texture, this->clip);
		this->setPosition(this->pos);
	}

	this->unlock("Set texture", b);
}

Sprite* Object::getSprite() {
	return this->sprite;
}

void Object::addToView() {
	Log::inf(LOG_OBJ, "Adding To View Object #%d %s", this->id, this->name);
	
	if (this->node != NULL) {
		Log::war(LOG_OBJ, "Trying To add To View Twice Object #%d %s", this->id, this->name);
		return;
	}

	this->node = Render::get()->addObject(this);
}

vector Object::getPosition() {
	bool b = this->lock("Get Posision");
	vector res;

	const Vector2f  pos = this->sprite->getPosition();
	res.x = (double) pos.x;
	res.y = (double) pos.y;

	this->unlock("Get Posision", b);
	return res;
}

void Object::setPosition(vector pos) {
	bool b = this->lock("Set Posision");

	this->pos = pos;
	if (this->sprite != NULL) {
		this->sprite->setPosition(pos.x, pos.y);
	}

	this->unlock("Set Posision", b);
}

vector Object::move(vector m) {
	bool b = this->lock("Move");

	this->sprite->move(m.x, m.y);
	
	this->unlock("Move", b);
	return this->getPosition();
}

void Object::draw(RenderWindow* window) {
	if (!this->enabled || !this->visible || this->sprite == NULL) {
		Log::war(LOG_OBJ, "-- Skip Rendre: %s", this->getName());
		return;
	}

	bool b = this->lock("Draw");
	
	// Log::war(LOG_OBJ, "-- Call Draw: %p", this->sprite);
	window->draw(*this->sprite);

	this->unlock("Draw", b);
}