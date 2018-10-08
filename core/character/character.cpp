#include "character.h"

Character::Character(CharacterType type, const char* name, const char* jsonKey, vector* pos, int z) {
	Log::inf(LOG_CHAR, "=== Creating Character: %s ===", name);

	this->type = type;

	int len = strlen(name) + 6;
	this->name = StrE(len);
	snprintf(this->name, len, "%s_Char", name);
	
	Json* data = this->loadConfig(jsonKey);
	this->loadObject(data, name, pos, z);

	this->initAnimFncs();
}

Character::~Character() {
	Log::inf(LOG_CHAR, "Deleting Character: %s", this->getName());

	this->removeView();	

	if (this->obj != NULL) {
		delete this->obj;
	}
	Log::inf(LOG_CHAR, "Deleting Character: %s Done", this->getName());
}

Json* Character::loadConfig(const char* jsonKey) {
	char jsonPath[150];
	memset(jsonPath, 0, 150);

	switch (this->type) {
		case CHAR_PLAYER:
			snprintf(jsonPath, 150, "character/player");
			break;

		case CHAR_MONSTER:
			snprintf(jsonPath, 150, "character/monster");
			break;

		case CHAR_BOSS:
			snprintf(jsonPath, 150, "character/boss");
			break;
	}

	Log::inf(LOG_CHAR, "-- Path: %s", jsonPath);

	Json* json = AssetMgr::get()->getJson(jsonPath);

	if (json == NULL) {
		char msg[500];
		memset(msg, 0, 500);
		snprintf(msg, 500, "Fail To Find Character Json: %s", jsonPath);

		Log::err(LOG_CHAR, "%s", msg);

		throw(new Exception(0, msg));
	}

	Log::inf(LOG_CHAR, "-- Search: %s", jsonKey);
	Json* data = jsonGetData(json, jsonKey);

	if (data == NULL) {
		char msg[500];
		memset(msg, 0, 500);
		snprintf(msg, 500, "Fail To Find %s Data In %s", jsonKey, jsonPath);

		Log::err(LOG_CHAR, "%s", msg);
		throw(new Exception(0, msg));
	}

	return data;
}

void Character::loadObject(Json* data, const char* name, vector* pos, int z) {
	char* sheet = (char*) jsonGetValue(data, "sheet", NULL);
	Log::inf(LOG_CHAR, "-- Sheet: %s", sheet);
	
	this->obj = new CharObj(name, pos, z, sheet, this);
	free(sheet);
}

CharObj* Character::getObject() {
	return this->obj;
}

void Character::moveDir(DirectionEnum dir) {
	Movement* m = this->obj->getMovement();
	vector vel = m->getVelocity();
	Direction d = m->getDirection();

	bool flip = true;
	switch (dir) {
		case DIR_LEFT:
			vel.x = -50;
			break;

		case DIR_RIGHT:
			vel.x = 50;
			break;

		default:
			flip = false;
			break;
	}

	if (flip && d.x != dir) {
		this->obj->flipH();
	}
	
	m->setVelocity(vel);
}

void Character::stopMove() {
	Movement* m = this->obj->getMovement();
	vector vel = m->getVelocity();
	vel.x = 0;
	
	m->setVelocity(vel);
}

void Character::initAnimFncs() {
	this->obj->addAnimLinkFnc("Idl2Run", this, &Character::isRunning);
	this->obj->addAnimLinkFnc("Run2Idle", this, &Character::isStopped);
}

bool Character::isRunning() {
	vector vel = this->obj->getVelocity();
	return vel.x && ! vel.y;
}

bool Character::isStopped() {
	vector vel = this->obj->getVelocity();
	return !vel.x && ! vel.y;
}



void Character::removeView() {
	if (this->view != NULL) {
		Log::inf(LOG_CHAR, "-- Deleting view: %p", this->view);
		delete this->view;
	}

	this->view = NULL;
}


void Character::setView(ViewMgr* v) {
	this->removeView();
	
	Log::inf(LOG_CHAR, "-- Setting view: %p", v);
	this->view = v;
}

void Character::update() {
	if (this->view != NULL) {
		Log::inf(LOG_CHAR, "-- Char Call View Update");
		this->view->update();
	}
}