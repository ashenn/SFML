#include "character.h"
#include "../collision/collision.h"

Character::Character(CharacterType type, const char* name, const char* jsonKey, vector* pos, int z) {
	Log::inf(LOG_CHAR, "=== Creating Character: %s ===", name);

	this->type = type;

	int len = strlen(name) + 6;
	this->name = StrE(len);
	snprintf(this->name, len, "%s_Char", name);

	this->initCallableFncs();

	Json* data = this->loadConfig(jsonKey);
	this->loadObject(data, name, pos, z);

	this->initStats();
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

void Character::initCallableFncs() {
	this->callables = initListMgr();
	CallableFncCol<Character>* ctrl = new CallableFncCol<Character>(&Character::hit);

	Node* n = addNodeV(this->callables, "hit", ctrl, false);
	n->del = deleteCallable;
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

		throw(new Exception(LOG_CHAR, msg));
	}

	Log::inf(LOG_CHAR, "-- Search: %s", jsonKey);
	Json* data = jsonGetData(json, jsonKey);

	if (data == NULL) {
		char msg[500];
		memset(msg, 0, 500);
		snprintf(msg, 500, "Fail To Find %s Data In %s", jsonKey, jsonPath);

		Log::err(LOG_CHAR, "%s", msg);
		throw(new Exception(LOG_CHAR, msg));
	}

	return data;
}

void Character::loadObject(Json* data, const char* name, vector* pos, int z) {
	char* sheet = (char*) jsonGetValue(data, "sheet", NULL);
	Log::inf(LOG_CHAR, "-- Sheet: %s", sheet);

	this->obj = new CharObj(name, pos, z, sheet, this);

	ListManager* cols = (ListManager*) jsonGetValue(data, "collisions", NULL);
	Node* colN = NULL;

	while ((colN = listIterate(cols, colN)) != NULL) {
		Json* js = (Json*) colN->value;
		this->initCollision(js);
	}


	free(sheet);
}

void Character::initCollision(Json* js) {
	Log::inf(LOG_CHAR, "Adding Collision: %s", js->key);
	IntRect pos(0,0,0,0);

	Log::inf(LOG_CHAR, "Adding Collision: %s", js->key);

	char* colChanelName = (char*) jsonGetValue(js, "type", NULL);
	ColChanel chan = colChanelValue(colChanelName);

	Log::inf(LOG_CHAR, "COL Channel: %s", colChanelName);
	Log::inf(LOG_CHAR, "COL Channel Val: %d", chan);
	free(colChanelName);


	Json* posList = jsonGetData(js, "pos");
	jsonGetValue(posList, "top", &(pos.top));
	jsonGetValue(posList, "left", &(pos.left));
	jsonGetValue(posList, "width", &(pos.width));
	jsonGetValue(posList, "height", &(pos.height));

	Log::inf(LOG_CHAR, "COL Position:\n-- Top: %d\n-- Left: %d\n-- Width: %d\n-- Height: %d", pos.top, pos.left, pos.width, pos.height);

	Collision* col = this->obj->addCollision(js->key, pos, chan);

	char* callbackN = (char*) jsonGetValue(js, "callback", NULL);
	if (callbackN == NULL){
		return;
	}

	Log::war(LOG_CHAR, "CALLBACK COL: %s", callbackN);
	CallableFncCol<Character>* fncCol = (CallableFncCol<Character>*) this->getFnc(callbackN);

	if (fncCol == NULL) {
		Log::war(LOG_CHAR, "Fail To Find Callable Function: '%s' In Object %s", callbackN, this->getName());
		free(callbackN);
		return;
	}

	Log::war(LOG_CHAR, "Init Hit COL: %s", callbackN);
	col->setHit(this, fncCol->fnc);
	free(callbackN);
}

CharObj* Character::getObject() {
	return this->obj;
}

void Character::moveDir(DirectionEnum dir) {
	if (!this->canMove()) {
		return;
	}

	Movement* m = this->obj->getMovement();
	vector vel = m->getVelocity();
	Direction d = m->getDirection();

	bool flip = true;
	unsigned int speed = this->stats.moveSpeed;

	if (flip && d.x != dir) {
		this->obj->flipH();
		vel.x = 0;
	}

	switch (dir) {
		case DIR_LEFT:
			vel.x = -((double) speed);
			break;

		case DIR_RIGHT:
			vel.x = ((double) speed);
			break;

		default:
			flip = false;
			break;
	}

	this->stats.moving = true;
	Log::war(LOG_CHAR, "Adding Vel: %lf", vel.x);
	//m->setVelocity(vel);
	m->setVelocity(vel);

	Log::war(LOG_CHAR, "Res Vel: %lf", m->getVelocity().x);
}

void Character::stopMove() {
	//Movement* m = this->obj->getMovement();
	/*vector vel = m->getVelocity();
	vel.x = 0;*/

	this->stats.moving = 0;

	//m->setVelocity(vel);
}

void Character::initAnimFncs() {
	this->obj->addAnimLinkFnc("Fall2Land", this, &Character::isOnLand);
	this->obj->addAnimLinkFnc("Idle2Run", this, &Character::isRunning);
	this->obj->addAnimLinkFnc("Run2Idle", this, &Character::isStopped);
	this->obj->addAnimLinkFnc("Idle2Jump", this, &Character::isJumping);
	this->obj->addAnimLinkFnc("Idle2Fall", this, &Character::isFalling);
	this->obj->addAnimLinkFnc("Idle2Down", this, &Character::isCrouching);
}

bool Character::isOnLand() {
	return !this->stats.inAir;
}

bool Character::isRunning() {
	// Log::war(LOG_CHAR, "Is Run");

	vector vel = this->obj->getVelocity();
	return vel.x && !this->stats.inAir;
}

bool Character::isStopped() {
	vector vel = this->obj->getVelocity();
	return !vel.x && !this->stats.inAir;
}

bool Character::isJumping() {
	return this->stats.inAir && this->obj->getMovement()->getVelocity().y < 0;
}

bool Character::isFalling() {
	return this->stats.inAir && this->obj->getMovement()->getVelocity().y > 0;
}

bool Character::isCrouching() {
	return !this->stats.inAir && this->stats.crouch && this->obj->getMovement()->getVelocity().y == 0;
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

void Character::update(bool gravity) {
	if (this->view != NULL) {
		Log::inf(LOG_CHAR, "-- Char Call View Update");
		this->view->update();
	}

	vector vel = this->obj->getMovement()->getVelocity();

	if (vel.y && !this->stats.inAir) {
		this->stats.inAir =  true;
	}
	else if(!this->stats.inAir && gravity && !this->stats.moving) {
		vector vel = this->obj->getMovement()->getVelocity();
		vector frict = {0, 0};

		if (vel.x > 0){
			frict.x = -1;
		}
		else if(vel.x < 0){
			frict.x = 1;
		}

		Log::war(LOG_CHAR, "Velocity: %lf", vel.x);
		Log::war(LOG_CHAR, "Friction: %lf", frict.x);
		this->obj->getMovement()->addVelocity(frict);

		vel = this->obj->getMovement()->getVelocity();
		Log::war(LOG_CHAR, "New Velocity: %lf", vel.x);
	}
}

void Character::Crouch() {
	if (this->stats.inAir) {
		return;
	}

	//this->stopMove();
	this->stats.crouch = true;
}

void Character::Jump(bool full) {
	if (this->stats.inAir && this->stats.hasDoubleJump) {
		Log::war(LOG_CHAR, "Can't Jump");
		return;
	}
	else if(this->stats.inAir) {
		Log::war(LOG_CHAR, "Double Jump");
		Log::war(LOG_CHAR, "%d / %d", this->stats.inAir, this->stats.hasDoubleJump);
		this->stats.hasDoubleJump = true;
		Log::war(LOG_CHAR, "%d / %d", this->stats.inAir, this->stats.hasDoubleJump);
	}
	else {
		Log::war(LOG_CHAR, "Jump");
	}

	this->stats.inAir = true;
	this->stats.crouch = false;

	Movement* m = this->obj->getMovement();

	vector vel = m->getVelocity();

	vel.y = 0;
	m->setVelocity(vel);

	vel.x = 0;
	if (full) {
		vel.y = -((double) (this->stats.jumpMax));
	}
	else{
		vel.y = -((double) (this->stats.jump));
	}

	m->addVelocity(vel);
}

void Character::initStats() {
	this->stats.jump = 3;
	this->stats.jumpMax = 5;

	this->stats.life = 50;
	this->stats.lifeMax = 50;

	this->stats.inAir = false;
	this->stats.moving = false;
	this->stats.crouch = false;

	this->stats.moveSpeed = 5;
	this->stats.maxMoveSpeedX = 10;
	this->stats.maxMoveSpeedY = 5;

	this->stats.canDoubleJump = true;
	this->stats.hasDoubleJump = false;

	this->obj->setMaxSpeed(this->stats.maxMoveSpeedX, this->stats.maxMoveSpeedY);
}

void Character::landed() {
	Log::dbg(LOG_CHAR, "LANDED !!!");
	this->stats.inAir = false;
	this->stats.canDoubleJump = true;
	this->stats.hasDoubleJump = false;
}

bool Character::hit(Collision* col, Collision* col2) {
	if (!this->stats.inAir) {
		return true;
	}

	if (col2->getFlag() != (int) 1 << COL_WALL && col2->getFlag() != (int) 1 << COL_PLATFORM) {
		return true;
	}

	IntRect pos = col->getWorldPosition();
	pos.top += this->obj->getMovement()->getVelocity().y;

	IntRect pos2 = col2->getWorldPosition();
	Log::dbg(LOG_CHAR, "=== Target: %d", pos2.top);

	Log::dbg(LOG_CHAR, "=== Top: %d", pos.top);
	Log::dbg(LOG_CHAR, "=== Height: %d", pos.top + pos.height);

	bool under = (pos2.top >= pos.top) && (pos2.top <= pos.top + pos.height);

	if (under) {
		this->landed();
	}
	Log::dbg(LOG_CHAR, "=== HIT UNDER: %d", under);

	return true;
}

bool Character::canMove() {
	return !this->stats.crouch;
}
