#include "playerCtrl.h"
#include "../../view/view.h"

ListManager* PlayerCtrl::playerList = initListMgr();

void delPlayer(Node* n) {
	if (n->value == NULL) {
		return;
	}

	PlayerCtrl* player = (PlayerCtrl*) n->value;
	delete player;
}

PlayerCtrl::PlayerCtrl(int id, const char* name, const char* jsonKey, vector* pos, int z) : 
Controller(CTRL_PLAYER, name) {
	Node* n = getNode(playerList, id);

	if (n != NULL && n->value != NULL) {
		int len = 40;
		char msg[len];
		memset(msg, 0, len);
		snprintf(msg, len, "Trying To Spawn Player: #%d Twice !!!", id);

		Log::war(LOG_CTRL_PLAYER, "%s", msg);
		throw(new Exception(LOG_CTRL_PLAYER, msg));
	}
	else if(n != NULL) {
		removeAndFreeNode(playerList, n);
	}

	if (jsonKey != NULL) {
		Character* ch = new Character(CHAR_PLAYER,name, jsonKey, pos, z);
		this->setCharacter(ch, true);
	}

	this->id = id;

	char ctrlName[150];
	memset(ctrlName, 0, 150);
	snprintf(ctrlName, 150, "Player-%d", id);

	n = addNodeV(playerList, name, this, false);
	n->id = id;
	n->del = delPlayer;


	this->callables = initListMgr();
	CallableFncEvt<PlayerCtrl>* ctrl = new CallableFncEvt<PlayerCtrl>(&PlayerCtrl::moveEvt);

	n = addNodeV(this->callables, "move", ctrl, false);
	n->del = deleteCallable;
	
	ctrl = new CallableFncEvt<PlayerCtrl>(&PlayerCtrl::stopEvt);
	n = addNodeV(this->callables, "stop", ctrl, false);
	n->del = deleteCallable;

	ctrl = new CallableFncEvt<PlayerCtrl>(&PlayerCtrl::downEvt);
	n = addNodeV(this->callables, "down", ctrl, false);
	n->del = deleteCallable;

	ctrl = new CallableFncEvt<PlayerCtrl>(&PlayerCtrl::jumpEvt);
	n = addNodeV(this->callables, "jump", ctrl, false);
	n->del = deleteCallable;

	this->loadControls();
}

PlayerCtrl::~PlayerCtrl() {
	
}

void PlayerCtrl::setCharacter(Character* ch, bool deleteOld) {
	Controller::setCharacter(ch, deleteOld);
	if (this->ch == NULL) {
		return;
	}

	Log::inf(LOG_CTRL_PLAYER, "CREATE PLAYER");
	this->ch = ch;
	new ViewMgr(this->ch, VIEW_CAMERA, sf::FloatRect(-150,0,800,600));
}

void PlayerCtrl::moveEvt(KeyEvt<PlayerCtrl>* evt) {
	if (evt->getKey() == Keyboard::Key::Left) {
		this->moveDir(DIR_LEFT);
	}
	else if(evt->getKey() == Keyboard::Key::Right) {
		this->moveDir(DIR_RIGHT);
	}
}

void PlayerCtrl::stopEvt(KeyEvt<PlayerCtrl>* evt) {
	if (evt->getKey() == sf::Keyboard::Key::Down) {
		this->ch->Stand();
	}
	else {
		this->stopMove();
	}
}

unsigned short PlayerCtrl::getId() {
	return this->id;
}

void PlayerCtrl::loadControls() {
	Log::inf(LOG_CTRL_PLAYER, "=== LOADING CONTROLS ===");
	
	Json* ctrls = AssetMgr::get()->getConf("control/control");
	if (ctrls == NULL) {
		Log::err(LOG_CTRL_PLAYER, "Fail To Find Controls Conf: '%s'", ctrls);
		return;
	}
	
	Json* ctrlMode = jsonGetData(ctrls, "SideScroll");
	if (ctrlMode == NULL) {
		Log::err(LOG_CTRL_PLAYER, "Fail To Find Controls Mode: '%s'", "SideScroll");
		return;
	}

	int len = 10;
	char playerName[len];
	memset(playerName, 0, len);
	snprintf(playerName, len, "player-%d", this->id);

	ListManager* playerCtrl = (ListManager*) jsonGetValue(ctrlMode, playerName, NULL);
	if (playerCtrl == NULL) {
		Log::err(LOG_CTRL_PLAYER, "Fail To Find Controls For Player: '%s'", playerName);
		return;
	}

	Node* n = NULL;
	while ((n = listIterate(playerCtrl, n)) != NULL) {
		this->loadControl((Json*) n->value, playerName);
	}
}

void PlayerCtrl::loadControl(Json* ctrl, const char* playerName) {
	ControlMgr* mgr = ControlMgr::get();

	Log::inf(LOG_CTRL_PLAYER, "-- Load Key: '%s'", ctrl->key);
	sf::Keyboard::Key key = mgr->getKey(ctrl->key);

	if (key == -1) {
		Log::err(LOG_CTRL_PLAYER, "Fail To Find Key: '%s'", ctrl->key);
		return;
	}

	int len = strlen(playerName) + strlen(ctrl->key) + 6;
	char evtN[len];
	memset(evtN, 0, len);
	snprintf(evtN, len, "%s_Evt_%s", playerName, ctrl->key);

	EventMgr::get()->bindKeyEvent(evtN, key, this, ctrl);
}

void PlayerCtrl::downEvt(KeyEvt<PlayerCtrl>* evt) {
	this->ch->Crouch();
}

void PlayerCtrl::jumpEvt(KeyEvt<PlayerCtrl>* evt) {
	vector time = evt->getOnHoldTime();
	this->ch->Jump(time.x >= time.y);
}

bool PlayerCtrl::hitWall(Collision* col, Collision* col2, IntRect pos, IntRect pos2) {
	this->ch->hitWall(col, col2, pos, pos2);
	return true;
}

bool PlayerCtrl::hitMonster(Collision* col, Collision* col2, IntRect pos, IntRect pos2) {
	bool under = col->isOver(col2, this->obj->getMovement()->getVelocity());
	if (under) {
		this->ch->landed();

		CharObj* mon = (CharObj*) col2->getObject();
		this->ch->makeDamage(mon->getCharacter());
		this->ch->Jump(true);
	}

	return true;
}

void PlayerCtrl::kill() {
	if (this->checkpoint != NULL) {
		vector pos = this->checkpoint->getPosition();
		this->obj->getMovement()->setVelocity({0,0});
		this->obj->setPosition(pos);
	}
}

bool PlayerCtrl::overlapCheckPoint(Object* checkObj) {
	if (this->checkpoint == checkObj) {
		return false;
	}

	this->checkpoint = checkObj;

	return true;
}

bool PlayerCtrl::overlap(Collision* col, Collision* col2) {
	switch (col2->getFlag()) {
		case (int) 1 << COL_CHECKPOINT:
			return this->overlapCheckPoint(col2->getObject());
			break;

		default:
			return false;
			break;
	}

}

bool PlayerCtrl::hit(Collision* col, Collision* col2) {
	IntRect pos = col->getWorldPosition();
	pos.top += this->obj->getMovement()->getVelocity().y;

	IntRect pos2 = col2->getWorldPosition();
	// Log::war(LOG_CHAR, "=== Obj: %s", col2->getObject()->getName());
	// Log::war(LOG_CHAR, "=== Target: %d", pos2.top);

	// Log::war(LOG_CHAR, "=== Top: %d", pos.top);
	// Log::war(LOG_CHAR, "=== Height: %d", pos.top + pos.height);

	switch (col2->getFlag()) {
		case (int) 1 << COL_WALL:
		case (int) 1 << COL_PLATFORM:
			return this->hitWall(col, col2, pos, pos2);
			break;

		case (int) 1 << COL_MONSTER:
			return this->hitMonster(col, col2, pos, pos2);
			break;

		default:
			return true;
			break;
	}
}