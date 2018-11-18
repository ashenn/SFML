#include "charObject.h"
#include "../../../collision/collisionMgr.h"
#include "../../../controller/controller.h"

CharObj::CharObj(const char* name, vector* pos, unsigned short z, const char* path, Character* ch) : 
SpriteObj(name, pos, z, path) {
	this->ch = ch;
	this->gravity = true;
}

CharObj::~CharObj() {

}

void CharObj::draw(RenderWindow* window, bool grav) {
	SpriteObj::draw(window, grav);

	if (this->ch == NULL) {
		return;
	}

	this->ch->update(grav);
}

Character* CharObj::getCharacter() {
	return this->ch;
}

vector CharObj::canMove(vector m) {
	m = Object::canMove(m);

	if (this->ch == NULL || this->ch->getCtrl() == NULL || this->ch->canFallOfEdge()) {
		return m;
	}

	vector move = m;
	move.y += this->ch->getStepHeight();

	ListManager* collisions = CollisionMgr::get()->searchCollision(this, COL_BLOCK, move);
	if (collisions != NULL || this->ch->getCtrl()->willFall()) {
		Log::dbg(LOG_CHAR, "Can Move");
		deleteList(collisions);
		return m;
	}


	Log::dbg(LOG_CHAR, "Can't Fall");
	return {0,0};
	move = {0,0};
	move.y += this->ch->getStepHeight();
	
	collisions = CollisionMgr::get()->searchCollision(this, COL_BLOCK, move);
	if (collisions != NULL) {
		deleteList(collisions);
	}

	Collision* col = this->getCollision("hitbox");
	IntRect pos = col->getWorldPosition();
	Log::dbg(LOG_CHAR, "############## %s ##############", this->name);
	Log::dbg(LOG_CHAR, "-- Height: %u | %d", this->ch->getStepHeight(), pos.top + pos.height + this->ch->getStepHeight());
	Log::dbg(LOG_CHAR, "-- Pos: %d | %d | %d | %d", pos.left + ((int) move.x), pos.top + ((int) move.y), pos.left + ((int) move.x) + pos.width, pos.top + ((int) move.y) + pos.height);

	Log::dbg(LOG_CHAR, "Accepted");
	return m;
}