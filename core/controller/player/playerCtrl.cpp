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
		throw(new Exception(0, msg));
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
}

PlayerCtrl::~PlayerCtrl() {
	
}

void PlayerCtrl::setCharacter(Character* ch, bool deleteOld) {
	if (this->ch != NULL && deleteOld) {
		delete this->ch;
	}

	Log::err(LOG_CTRL_PLAYER, "CREATE PLAYER");
	this->ch = ch;
	new ViewMgr(this->ch, VIEW_CAMERA, sf::FloatRect(-150,0,800,600));

	if (ch != NULL) {
		this->obj = ch->getObject();
	}
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
	this->stopMove();
}

unsigned short PlayerCtrl::getId() {
	return this->id;
}