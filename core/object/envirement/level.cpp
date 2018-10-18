#include "level.h"

Level::Level(const char* name) {
	Log::war(LOG_LEVEL, "=== Init Level: %s ===", name);
	this->name = Str(name);

	int len = strlen(name) + 7; 
	char path[len];
	memset(path, 0, len);
	snprintf(path, len, "level/%s", name);

	Log::war(LOG_LEVEL, "-- path: %s", path);
	this->conf = AssetMgr::get()->getJson(path);

	if (this->conf == NULL) {
		len = 27 + strlen(path);
		char msg[len];
		memset(msg, 0, len);
		snprintf(msg, len, "Fail To Load Level Conf: %s", path);

		throw(new Exception(LOG_LEVEL , msg));
	}

	this->loadSections();
}

Level::~Level() {
	deleteList(this->sections);
}

void Level::loadSections() {
	Node* n = NULL;
	this->sections = initListMgr();
	
	ListManager* sections = (ListManager*) jsonGetValue(this->conf, "sections", NULL);

	Log::war(LOG_LEVEL, "-- Loading Sections: %d", sections->nodeCount);

	while ((n = listIterate(sections, n)) != NULL) {
		Json* secJ = (Json*) n->value;
		Log::war(LOG_LEVEL, "++++++ Sections: %s", secJ->key);

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