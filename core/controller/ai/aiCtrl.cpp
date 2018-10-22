#include "aiCtrl.h"

ListManager* AiCtrl::aiList = initListMgr();

AiCtrl::AiCtrl(const char* name, const char* jsonKey, vector* pos, int z) :
Controller(CTRL_AI, name) {
	Log::inf(LOG_AI, "=== Spawn AI: %s / %s ===", name, jsonKey);

	static unsigned int id = 0;
	this->id = id++;


	this->dir[0] = DIR_RIGHT;
	this->dir[1] = DIR_DOWN;

	Log::dbg(LOG_AI, "-- id: %d", this->id);

	free(this->name);
	int len = strlen(name) + (this->id / 10) +5;
	this->name = StrE(len);
	snprintf(this->name, len, "AI-%d_%s", this->id, name);


	if (jsonKey != NULL) {
		Log::dbg(LOG_AI, "-- Init Character");
		Character* ch = new Character(CHAR_MONSTER, name, jsonKey, pos, z);
		this->setCharacter(ch, true);
	}

	this->callables = initListMgr();
	this->ch->moveDir(DIR_RIGHT);
}

AiCtrl::~AiCtrl() {
	
}

bool AiCtrl::hitWall(Collision* col, Collision* col2, IntRect pos, IntRect pos2) {
	this->ch->hitWall(col, col2, pos, pos2);
	if (col->isUnder(col2)) {
		Log::dbg(LOG_AI, "-- AI HIT WALL");

		if (col->isRight(col2)) {
			Log::dbg(LOG_AI, "-- Move Right");
			this->dir[0] = DIR_RIGHT;
		}
		else {
			Log::dbg(LOG_AI, "-- Move Left");
			this->dir[0] = DIR_LEFT;
		}

		vector vel = this->obj->getMovement()->getVelocity();
		Log::dbg(LOG_AI, "Vel: %lf", vel.x);
		Log::dbg(LOG_AI, "UPDATE: %d", this->ch->getDirX());
	}

	return true;
}

bool AiCtrl::hit(Collision* col, Collision* col2) {
	IntRect pos = col->getWorldPosition();
	pos.top += this->obj->getMovement()->getVelocity().y;

	IntRect pos2 = col2->getWorldPosition();
	Log::dbg(LOG_CHAR, "=== Target: %d", pos2.top);

	Log::dbg(LOG_CHAR, "=== Top: %d", pos.top);
	Log::dbg(LOG_CHAR, "=== Height: %d", pos.top + pos.height);

	switch (col2->getFlag()) {
		case (int) 1 << COL_WALL:
		case (int) 1 << COL_PLAYER:
		case (int) 1 << COL_PLATFORM:
			return this->hitWall(col, col2, pos, pos2);
			break;

		default:
			return true;
			break;
	}

}

void AiCtrl::update() {	
	this->ch->moveDir(this->dir[0]);
}

bool AiCtrl::overlap(Collision* col, Collision* col2) {
	return true;
}

void AiCtrl::kill() {
	Log::dbg(LOG_CHAR, "Killing: %s", this->name);
	if (this->ch != NULL) {
		this->setCharacter(NULL, true);
	}
}