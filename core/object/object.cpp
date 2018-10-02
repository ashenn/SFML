#include "object.h"
#include "../render/render.h"

ListManager* Object::objectList = initListMgr();

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


	this->setClip(clip, true);
	
	Log::dbg(LOG_OBJ, "-- Clip:");
	Log::dbg(LOG_OBJ, "++++ X: %d", this->clip->left);
	Log::dbg(LOG_OBJ, "++++ Y: %d", this->clip->top);
	Log::dbg(LOG_OBJ, "++++ W: %d", this->clip->width);
	Log::dbg(LOG_OBJ, "++++ H: %d", this->clip->height);

	if (text != NULL) {
		this->setTexture(text);
	}
	
	Object::addObject(this);
	Log::dbg(LOG_OBJ, "=== Ready Object #%d: %s ===", this->id, this->name);
}

Object::~Object() {
	Log::inf(LOG_OBJ, "=== Deleting Object #%d: %s ===", this->id, this->name);

	this->removeTexture();

	if (this->clip != NULL) {
		delete this->clip;
	}
	
	removeObject(this);
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
		this->sprite = new Sprite(*this->texture, *this->clip);
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

void Object::updateClip() {
	if (this->sprite != NULL) {
		this->sprite->setTextureRect(*this->clip);
	}

	Log::err(LOG_SPRITE, "-- New Clip: X: %d | Y: %d | W: %d | H: %d", this->clip->left, this->clip->top, this->clip->width, this->clip->height);
}

void Object::setClip(IntRect* clip, bool clean=true) {
	if (this->clip != NULL && clean) {
		delete this->clip;
		this->clip = NULL;
	}

	if (clip != NULL) {
		this->clip = clip;
	}
	else {
		this->clip = new IntRect(0, 0, 50, 50);
	}

	this->updateClip();
}

void deleteObject(Node* n) {
	Object* obj = (Object*) n->value;

	Log::war(LOG_OBJ, "deleting Object: %s", obj->getName());
	delete obj;
}

void Object::addObject(Object* obj) {
	Log::war(LOG_OBJ, "Adding Object: %s", obj->getName());
	Node* n = addNodeV(objectList, obj->getName(), obj, false);
	n->del = deleteObject;
}

void Object::removeObject(Object* obj) {
	Log::war(LOG_OBJ, "Removing Object: %s", obj->getName());
	
	Node* n = getNodeByValue(objectList, obj);
	if (n == NULL) {
		return;
	}
	
	n->del = NULL;
	Log::war(LOG_OBJ, "Free Node: %s", n->name);
	removeAndFreeNode(objectList, n);
}

void Object::clearObjects() {
	Log::war(LOG_OBJ, "Clearing Objects List");
	deleteList(objectList);
}