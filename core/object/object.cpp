#include "object.h"
#include "../render/render.h"
#include "../asset/asset.h"
#include "../animation/animation.h"
#include "../collision/collisionMgr.h"

ListManager* Object::objectList = initListMgr();

void delChildObj(Node* n) {
	if (n->value == NULL) {
		return;
	}

	delete (Object*) n->value;
}

Object::Object(const char* name, vector* pos, Texture* text, IntRect* clip, unsigned short z, bool visible) {
	if (name == NULL || !strlen(name)) {
		Log::war(LOG_OBJ, "Creating Object With Empty Name");
	}

	Log::inf(LOG_OBJ, "=== Creating Object: %s ===", name);

	int len = strlen(name) + 5;
	this->name = StrE(len);
	snprintf(this->name, len, "%s_Obj", name);


	this->z = z;
	this->texture = text;
	this->visible = visible;
	this->movement = new Movement(this);

	this->childs = initListMgr();
	this->collisions = initListMgr();

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

	if (pos != NULL) {
		this->setPosition(*pos);
	}
	else {
		vector pos2 = {0, 0};
		this->setPosition(pos2);
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
	
	this->removeParent();
	deleteList(this->childs);

	delete this->movement;
	deleteList(this->collisions);
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

	Log::inf(LOG_OBJ, "-- Setting Texture: %p", text);
	
	this->removeTexture();
	this->texture = text;


	if (this->texture != NULL) {
		Log::inf(LOG_OBJ, "-- Applying New Texture");
		this->sprite = new Sprite(*this->texture, *this->clip);
		// this->sprite->setOrigin({ this->sprite->getLocalBounds().width / 2, 0 });
		
		this->setPosition(this->pos);
	}

	this->unlock("Set texture", b);
}

sf::Vector2f Object::getOrigin() {
	return this->sprite->getOrigin();
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
	if (this->parent == NULL) {
		return this->pos;
	}
	vector pos = this->parent->getPosition();
	
	vector res;
	res.x = (pos.x) + this->pos.x;
	res.y = (pos.y) + this->pos.y;

	return res;
}

vector Object::getRelativePosition() {
	bool b = this->lock("Get Relative Posision");
	vector res;

	const Vector2f pos = this->sprite->getPosition();
	res.x = (double) pos.x;
	res.y = (double) pos.y;

	if (this->parent != NULL) {
		const Vector2f pos2 = this->sprite->getPosition();
		res.x -= pos2.x;
		res.y -= pos2.y;
	}

	this->unlock("Get Relative Posision", b);
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

	vector move = this->canMove(m);

	this->pos.x += move.x;
	this->pos.y += move.y;
	this->sprite->move(move.x, move.y);

	if (move != m) {
		this->movement->setVelocity(move);
	}
	
	this->unlock("Move", b);
	return this->getRelativePosition();
}

vector Object::canMove(vector m) {
	Collision** cols = CollisionMgr::get()->searchCollision(this, COL_BLOCK, m);

	if (cols == NULL) {
		return m;
	}

	//Log::dbg(LOG_OBJ, "Can Move Call Hit");
	bool applyCol = cols[0]->callHit(cols[1]);
	if (!applyCol) {
		Log::dbg(LOG_OBJ, "Skip Collision");
		return m;
	}

	vector move = {0, 0};
	move.x = m.x;

	if (cols[0]->collides(cols[1], move)) {
		Log::dbg(LOG_OBJ, "UPDATE MOVE X: %d", m.x);
		m.x = 0;
	}

	move.x = 0;
	move.y = m.y;
	if (cols[0]->collides(cols[1], move)) {
		Log::dbg(LOG_OBJ, "UPDATE MOVE Y: %d", m.y);
		m.y = 0;
	}

	free(cols);
	return m;
}

void Object::draw(RenderWindow* window, bool grav) {
	if (!this->enabled || !this->visible || this->sprite == NULL) {
		Log::dbg(LOG_OBJ, "-- Skip Rendre: %s", this->getName());
		return;
	}

	bool b = this->lock("Draw");
	if (this->gravity && grav) {
		vector gravity = {0, 1};
		this->movement->addVelocity(gravity);
	}

	vector move = this->movement->getVelocity();
	
	if (move.x > 0) {
		Log::dbg(LOG_OBJ, "MOVE: %lf | %lf", move.x, move.y);
	}

	move = this->canMove(move);
	this->pos.x += move.x;
	this->pos.y += move.y;

	this->movement->setVelocity(move);
	vector pos = this->getPosition();


	if (move.x > 0) {
		Log::dbg(LOG_OBJ, "POS: %lf | %lf", pos.x, pos.y);
	}

	
	this->sprite->setPosition(pos.x, pos.y);


	window->draw(*this->sprite);

	Node* n = NULL;
	while ((n = listIterate(this->childs, n)) != NULL) {
		if (n->value == NULL) {
			continue;
		}

		Object* child = (Object*) n->value;
		child->draw(window, grav);
	}

	this->unlock("Draw", b);
}

void Object::updateClip() {
	if (this->sprite != NULL) {
		this->sprite->setTextureRect(*this->clip);
	}
}

void Object::removeClip(bool clean=true) {
	if (this->clip != NULL && clean) {
		delete this->clip;
	}
	
	this->clip = NULL;
}

// IntRect Object::getClip() {
// 	return *(this->clip);
// }

void Object::setClip(IntRect* clip, bool clean=true) {
	if (clean) {
		this->removeClip(clean);
	}

	if (clip != NULL) {
		this->clip = clip;
		// this->defaultClip = *clip;
	}
	else {
		this->clip = new IntRect(0, 0, 50, 50);
	}

	this->updateClip();
}

void deleteObject(Node* n) {
	Object* obj = (Object*) n->value;
	if (obj == NULL) {
		return;
	}

	//Log::war(LOG_OBJ, "deleting Object: %s", obj->getName());

	delete obj;
}

void Object::addObject(Object* obj) {
	Log::inf(LOG_OBJ, "Adding Object: %s", obj->getName());
	Node* n = addNodeV(objectList, obj->getName(), obj, false);
	n->del = deleteObject;
}

void Object::removeObject(Object* obj) {	
	Node* n = getNodeByValue(objectList, obj);
	if (n == NULL) {
		//Log::war(LOG_OBJ, "Not Found Object: %s", obj->getName());
		return;
	}
	
	n->del = NULL;
	removeAndFreeNode(objectList, n);
}

void Object::clearObjects() {
	deleteList(objectList);
}

Movement* Object::getMovement() {
	return this->movement;
}

vector Object::getVelocity() {
	return this->movement->getVelocity();
}

void Object::flipH() {
	this->sprite->scale(-1,1);
}

void delCollision(Node* n) {
	if (n->value == NULL) {
		return;
	}

	Collision* col = (Collision*) n->value;
	delete col;
}

Collision* Object::addCollision(const char* name, IntRect pos, ColChanel chan) {
	Node* n = addNodeUniq(this->collisions, name, NULL, 0);
	if (n == NULL) {
		Log::war(LOG_OBJ, "Trying To Add Douplicate Collision: '%s'", name);
		return NULL;
	}

	Log::inf(LOG_OBJ, "Add Col: X: %d | Y: %d | W: %d | H: %d", pos.left, pos.top, pos.width, pos.height);

	Collision* col = new Collision(name, this, pos, chan);
	n->value = col;
	n->del = delCollision;

	Project* pro = Project::get();

	if (!(pro->getFlags() & LOG_COL)) {
		return col;
	}


	AssetMgr* ast = AssetMgr::get();

	vector v;
	v.x = pos.left;
	v.y = pos.top;

	IntRect* clip = new IntRect(0,0,50,50);

	sf::Texture* text = ast->getTexture("debug/collision");
	Object* colObj = new Object(name, NULL, text, clip, 1);
	col->colObj = colObj;

	vector scale;
	scale.x = (float) (pos.width / 50.0f);
	scale.y = (float) (pos.height / 50.0f);

	colObj->scale(scale);

	this->addChild(colObj, v, false);
	
	return col;
}

bool Object::addChildToList(Object* child) {
	Log::inf(LOG_OBJ, "=== Adding Child Object: '%s' To Object: '%s' ===");
	if (child == NULL) {
		Log::war(LOG_OBJ, "Trying To add NULL Child Object To: '%s'", child->getName());
		return false;
	}

	child->setParent(this);

	Node* n = addNodeV(this->childs, child->getName(), child, false);
	if (n == NULL) {
		Log::war(LOG_OBJ, "Fail To Add Child Object: '%s' To Object: '%s'");
		return false;
	}

	n->del = deleteObject;
	return true;
}

bool Object::addChild(Object* child, bool keepWorldPosition) {
	if (!this->addChildToList(child)) {
		return false;
	}

	vector pos;
	if (keepWorldPosition) {
		pos = child->getPosition();
		pos.x -= this->pos.x;
		pos.y -= this->pos.y;
	}
	else {
		if (child->getParent() == NULL) {
			pos.x = 0;
			pos.y = 0;
		}
		else {
			pos = child->getRelativePosition();
		}

		pos.x += this->pos.x;
		pos.y += this->pos.y;
	}

	child->setPosition(pos);

	return true;
}

bool Object::addChild(Object* child, vector pos, bool worldPosition) {
	if (!this->addChildToList(child)) {
		return false;
	}

	// if (!worldPosition) {
	// 	Log::war(LOG_OBJ, "Test: %d + %d", (int) pos.x, (int) this->pos.x);

	// 	pos.x -= this->pos.x;
	// 	Log::war(LOG_OBJ, "Test: %d", (int) pos.x);
	// 	pos.y -= this->pos.y;
	// }

	Vector2f origin = this->getOrigin();
	pos.x -= origin.x;
	pos.y -= origin.y;

	child->setPosition(pos);
	return true;
}

Object* Object::getParent() {
	return this->parent;
}

bool Object::removeChild(Object* child, bool delChild) {
	Log::inf(LOG_OBJ, "=== Deleting Child Object: '%s' From Object: '%s' ===", child->getName(), this->getName());

	Object* parent = child->getParent();
	if (parent == NULL) {
		Log::war(LOG_OBJ, "Trying To Remove Root Object: '%s' From Not Related Object: '%s'");
		return false;
	}

	if (parent != this) {
		Log::war(LOG_OBJ, "Trying To Remove Child Object: '%s' From Not Related Object: '%s'");
		Log::war(LOG_OBJ, "Real Parent Is: '%s'", parent->getName());
		return false;
	}

	Node* n = getNodeByValue(this->childs, child);
	if (n == NULL) {
		Log::inf(LOG_OBJ, "Fail To Find Child: '%s'", child->getName());
		return false;
	}

	if (!delChild) {
		n->del = NULL;
		child->setParent(NULL);
	}

	removeAndFreeNode(this->childs, n);
	return true;
}

void Object::removeParent() {
	if (this->parent == NULL) {
		return;
	}

	this->parent->removeChild(this, false);
	this->parent = NULL;
}

void Object::setParent(Object* parent) {
	this->removeParent();
	this->parent = parent;
}

ListManager* Object::getCollisions() {
	return this->collisions;
}

bool Object::isEnabled() {
	return this->enabled;
}

void Object::toggle(bool b) {
	this->enabled = b;
}

void Object::scale(vector s) {
	if (this->sprite == NULL) {
		return;
	}

	this->sprite->scale(sf::Vector2f(s.x, s.y));
}

void Object::setScale(vector s) {
	if (this->sprite == NULL) {
		return;
	}

	this->sprite->setScale(sf::Vector2f(s.x, s.y));
}

void Object::stopMove() {
	Animator::get()->removeMove(this);
}

void Object::setMaxSpeed(unsigned int x, unsigned int y) {
	this->movement->maxVelocity.x = (double) x;
	this->movement->maxVelocity.y = (double) y;
}