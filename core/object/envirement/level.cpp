#include "level.h"
#include "../../controller/player/playerCtrl.h"
#include "../../object/text/textObject.h"

Level::Level(const char* name) {
	Log::inf(LOG_LEVEL, "=== Init Level: %s ===", name);
	this->name = Str(name);

	int len = strlen(name) + 7; 
	char path[len];
	memset(path, 0, len);
	snprintf(path, len, "level/%s", name);

	Log::dbg(LOG_LEVEL, "-- path: %s", path);
	this->conf = AssetMgr::get()->getJson(path);

	if (this->conf == NULL) {
		len = 27 + strlen(path);
		char msg[len];
		memset(msg, 0, len);
		snprintf(msg, len, "Fail To Load Level Conf: %s", path);

		throw(new Exception(LOG_LEVEL , msg));
	}

	this->loadSections();
	this->loadEnd();
	this->loadStart();
}

Level::~Level() {
	deleteList(this->sections);
}

void Level::loadStart() {
	Json* startJ = jsonGetData(this->conf, "start");

	if (startJ == NULL || startJ->type == JSON_NULL) {
		this->start = {0, 0};
		this->playerStart = {0, -15};
	}
	else {
		int data[3];
		jsonGetValue(startJ, "x", &(data[0]));
		jsonGetValue(startJ, "y", &(data[1]));
		jsonGetValue(startJ, "z", &(data[2]));

		this->start.x = data[0];
		this->start.y = data[1];
		this->playerStart = this->start;


		vector pos = {-100, -150};

		EnvObj* st = new EnvObj("Start", "land/sprite", "start", &this->start, data[2]);
		TextObj* text = new TextObj("finish", &pos, "Reach The End Of The Map !!", "pfBold", 24, 10, true);

		Log::dbg(LOG_CTRL_PLAYER, "-- Adding To View");
		st->addChild(text, pos, false);
		
		this->playerStart.y -= 25;
		new PlayerCtrl(1, "Player", "adventurer", &this->start, 5);
	}
}

void Level::loadEnd() {
	Json* startJ = jsonGetData(this->conf, "end");

	if (startJ == NULL || startJ->type == JSON_NULL) {
		this->start = {0, 0};
	}
	else {
		int data[3];
		jsonGetValue(startJ, "x", &(data[0]));
		jsonGetValue(startJ, "y", &(data[1]));
		jsonGetValue(startJ, "z", &(data[2]));

		this->start.x = data[0];
		this->start.y = data[1];


		new EnvObj("Start", "land/sprite", "end", &this->start, data[2]);
	}
}

void Level::loadSections() {
	Node* n = NULL;
	this->sections = initListMgr();
	
	ListManager* sections = (ListManager*) jsonGetValue(this->conf, "sections", NULL);

	Log::dbg(LOG_LEVEL, "-- Loading Sections: %d", sections->nodeCount);

	while ((n = listIterate(sections, n)) != NULL) {
		Json* secJ = (Json*) n->value;
		Log::dbg(LOG_LEVEL, "++++++ Sections: %s", secJ->key);

		EnvSection* envObj = new EnvSection(secJ);

		Node* secN = addNodeV(this->sections, envObj->getName(), envObj, false);
		secN->del = deleteAbstract;
	}
}

Level* Level::curLevel = NULL;

Level* Level::getLevel() {
	return curLevel;
}

void Level::closeLevel() {
	if (curLevel != NULL) {
		delete curLevel;
		curLevel = NULL;
	}
}

void Level::close() {
	closeLevel();
}

Level* Level::load(const char* name) {
	closeLevel();
	curLevel = new Level(name);

	return curLevel;
}