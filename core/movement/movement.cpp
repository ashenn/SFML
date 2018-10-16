#include "movement.h"
#include "../animation/animation.h"

Movement::Movement(Object* obj) {
	this->obj = obj;
	this->velocity.x = 0;
	this->velocity.y = 0;

	this->dir.x = DIR_RIGHT;
	this->dir.y = DIR_DOWN;
}

Movement::~Movement() {}


void Movement::_setVelocityX(double x) {
	Log::inf(LOG_MOVE, "==== Setting Velocity X: %d ====", (int) x);
	vector* oVel = &this->velocity;

	oVel->x = x;
	if (oVel->x < 0) {
		this->dir.x = DIR_LEFT;
		if (oVel->x < -this->maxVelocity.x) {
			oVel->x = -this->maxVelocity.x;
		}
	}
	else if (oVel->x > 0) {
		this->dir.x = DIR_RIGHT;
		if (oVel->x > this->maxVelocity.x) {
			oVel->x = this->maxVelocity.x;
		}
	}

	Log::inf(LOG_MOVE, "-- Done X: %d ====", (int) x);
}

void Movement::_setVelocityY(double y) {
	Log::inf(LOG_MOVE, "==== Setting Velocity Y: %d ====", (int) y);
	vector* oVel = &this->velocity;

	oVel->y = y;
	if (oVel->y < 0) {
		this->dir.y = DIR_UP;
		if (oVel->x < -this->maxVelocity.x) {
			oVel->x = -this->maxVelocity.x;
		}
	}
	else if (oVel->y > 0) {
		this->dir.y = DIR_DOWN;
		if (oVel->y > this->maxVelocity.y) {
			oVel->y = this->maxVelocity.y;
		}
	}

	Log::inf(LOG_MOVE, "-- Done Y: %d ====", (int) y);
}

void Movement::setVelocityX(double x) {
	bool b = obj->lock("Set Velocity X-0");

	this->_setVelocityX(x);
	this->velocityUpdated();

	obj->unlock("Set Velocity X-1", b);
}

void Movement::setVelocityY(double y) {
	bool b = this->obj->lock("Set Velocity Y-0");

	this->_setVelocityX(y);
	this->velocityUpdated();

	this->obj->unlock("Set Velocity Y-1", b);
}

void Movement::setVelocity(vector vel) {
	Log::inf(LOG_MOVE, "==== Setting Velocity ====");
	Object* obj = this->obj;

	//Log::err(LOG_ANIM, "Lock Set Velocity");
	Log::dbg(LOG_MOVE, "-- Obj: %s", obj->getName());
	Log::dbg(LOG_MOVE, "-- vel: X: %d | Y: %d", (int) vel.x, (int) vel.y);

	bool b = obj->lock("Set Velocity-0");

	this->_setVelocityX(vel.x);
	this->_setVelocityY(vel.y);


	Log::inf(LOG_MOVE, "-- Direction: X: %d | %d", this->dir.x, this->dir.y);

	Log::inf(LOG_MOVE, "-- UpdatingVelocity");

	this->velocityUpdated();

	obj->unlock("Set Velocity-1", b);
	Log::inf(LOG_MOVE, "-- Set Velocity Done");
}

void Movement::velocityUpdated() {
	//Animator::get()->removeMove(this->obj);
	applyVelocity();
}

void Movement::addVelocity(vector vel) {
	Log::inf(LOG_MOVE, "==== Adding Velocity ====");
	Log::dbg(LOG_MOVE, "-- Obj: %s", this->obj->getName());
	Log::dbg(LOG_MOVE, "-- vel: X: %d | Y: %d", (int) vel.x, (int) vel.y);

	vector resV;
	vector* oVel = &this->velocity;

	resV.x = oVel->x + vel.x;
	resV.y = oVel->y + vel.y;

	this->setVelocity(resV);
}

void Movement::applyVelocity() {
	return;
	Log::inf(LOG_MOVE, "==== Applying Velocity ====");
	vector* vel = &this->velocity;
	if (!vel->x && !vel->y) {
		Animator::get()->removeMove(this->obj);
		return;
	}

	Object* obj = this->obj;
	vector pos = obj->getRelativePosition();

	Log::inf(LOG_MOVE, "Move To: X: %d + %d | Y: %d  + %d", (int) pos.x, (int) vel->x, (int) pos.y, (int) vel->y);
	Log::inf(LOG_MOVE, "Move To: X: %d | Y: %d", (int) (pos.x + vel->x), (int) (pos.y + vel->y));



	MoveAnim* anim = Animator::get()->moveTo(obj, (pos.x + vel->x), (pos.y + vel->y), 0.5f, 0);
	anim->breakOnReach = true;

	if (vel->x || vel->y) {
		anim->setCallBack(this, &Movement::refreshVelocity);
		//anim->callback = refreshVelocity;
	}

	Log::inf(LOG_MOVE, "==== Applying Velocity DONE ====");
}


void Movement::refreshVelocity() {
	Log::inf(LOG_MOVE, "==== Refresh Velocity ====");
	this->applyVelocity();
}

vector Movement::getVelocity() {
	return this->velocity;
}

Direction Movement::getDirection() {
	return this->dir;
}