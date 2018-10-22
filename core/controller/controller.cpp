#include "controller.h"

Controller::Controller(ControllerType type, const char* name) {
	Log::inf(LOG_CTRL, "=== Creating Controller: %s ===", name);

	int len = strlen(name) + 7;
	char n[len];
	memset(n, 0, len);
	snprintf(n, len, "%s_Ctrl", name);

	this->name = Str(n);
	this->type = type;
}

Controller::~Controller() {
	Log::inf(LOG_CTRL, "=== Deleting Controller: %s ===", this->name);

	if (this->ch != NULL) {
		delete this->ch;
	}
}

CharObj* Controller::getObject() {
	return this->obj;
}

Character* Controller::getCharacter() {
	return this->ch;
}

void Controller::moveDir(DirectionEnum dir) {
	if (this->ch == NULL) {
		Log::war(LOG_CTRL, "Trying To Move On NULL Character: %s", this->name);
		return;
	}

	this->ch->moveDir(dir);
}

void Controller::stopMove() {
	if (this->ch == NULL) {
		Log::war(LOG_CTRL, "Trying To Stop Move On NULL Character: %s", this->name);
		return;
	}

	this->ch->stopMove();
}



void Controller::setCharacter(Character* ch, bool deleteOld) {
	if (this->ch == ch) {
		return;
	}
	
	if (this->ch != NULL && deleteOld) {
		delete this->ch;
	}

	this->ch = ch;
	if (ch != NULL) {
		this->obj = ch->getObject();
		this->ch->setCtrl(this);
	}


	Log::inf(LOG_CTRL_PLAYER, "CREATE AI");
}