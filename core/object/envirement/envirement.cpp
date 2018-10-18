#include "envirement.h"
#include "../../asset/asset.h"
#include "../../collision/collision.h"

EnvObj::EnvObj(const char* name, const char* path, const char* conf, vector* pos, unsigned int z)
: Object(name, pos, z) {
	AssetMgr* ast = AssetMgr::get();
	Json* json = ast->getJson(path);

	char msg[500];
	memset(msg, 0, 500);
	if (json == NULL) {
		snprintf(msg, 500, "Fail To Find Envirement Conf: '%s'", path);
		throw(new Exception(LOG_ENV_OBJ, msg));
	}

	Json* confJ = jsonGetData(json, conf);
	if (confJ == NULL) {
		snprintf(msg, 500, "Fail To Find Key: '%s' In Envirement Conf: '%s'", conf, path);
		throw(new Exception(LOG_ENV_OBJ, msg));
	}

	Json* sizeJ = jsonGetData(confJ, "size");

	IntRect* objPos = new IntRect(0,0,0,0);
	jsonGetValue(sizeJ, "0", &(objPos->width));
	jsonGetValue(sizeJ, "1", &(objPos->height));

	this->setClip(objPos, true);

	Log::inf(LOG_ENV_OBJ, "Setting Texture");
	sf::Texture* text = ast->getMultiTexture(path, conf);
	this->setTexture(text);

	Log::inf(LOG_ENV_OBJ, "Loading Collisions");
	Node* n = NULL;
	ListManager* collisions = (ListManager*) jsonGetValue(confJ, "collisions", NULL);
	while ((n = listIterate(collisions, n))) {
		Json* colJ = (Json*) n->value;
		Log::inf(LOG_ENV_OBJ, "-- Col: %s", colJ->key);

		char* colChanN = (char*) jsonGetValue(colJ, "channel", NULL);
		ColChanel colChan = colChanelValue(colChanN);

		Log::inf(LOG_ENV_OBJ, "-- Col Channel: %s | %d", colChanN, colChan);
		free(colChanN);

		Json* posJ = jsonGetData(colJ, "pos");
		IntRect pos(0,0,0,0);
		jsonGetValue(posJ, "top", &(pos.top));
		jsonGetValue(posJ, "left", &(pos.left));
		jsonGetValue(posJ, "width", &(pos.width));
		jsonGetValue(posJ, "height", &(pos.height));

		Log::inf(LOG_ENV_OBJ, "-- Col Position:");
		Log::inf(LOG_ENV_OBJ, "-- Top: %d", pos.top);
		Log::inf(LOG_ENV_OBJ, "-- Left: %d", pos.left);
		Log::inf(LOG_ENV_OBJ, "-- Width: %d", pos.width);
		Log::inf(LOG_ENV_OBJ, "-- Height: %d", pos.height);

		Collision* col = this->addCollision(colJ->key, pos, colChan);

		IntRect colPos = col->getPosition();
		IntRect colWorldPos = col->getWorldPosition();
		Log::inf(LOG_ENV_OBJ, "Col Pos: X: %d | Y: %d | W: %d | H: %d", colPos.left, colPos.top, colPos.width, colPos.height);
		Log::inf(LOG_ENV_OBJ, "Col World Pos: X: %d | Y: %d | W: %d | H: %d", colWorldPos.left, colWorldPos.top, colWorldPos.width, colWorldPos.height);

	}
}

EnvObj::~EnvObj() {

}

EnvSection::EnvSection(Json* conf) {
	this->name = Str(conf->key);
	Log::inf(LOG_ENV_OBJ, "=== LOADING SECTION %s ===", this->name);

	char* sprite = (char*) jsonGetValue(conf, "sprite", NULL);
	int len = strlen(sprite) + 6;

	this->sprite = StrE(len);
	snprintf(this->sprite, len, "land/%s", sprite);

	Log::dbg(LOG_ENV_OBJ, "-- sprite %s", this->sprite);

	jsonPrint(conf, 0);
	Json* posJ = jsonGetData(conf, "pos");
	
	int data[2];
	jsonGetValue(posJ, "x", &(data[0]));
	jsonGetValue(posJ, "y", &(data[1]));
	
	this->pos.x = (double) data[0];
	this->pos.y = (double) data[1];
	Log::dbg(LOG_ENV_OBJ, "-- Pos: X: %lf | Y: %lf", pos.x, pos.y);


	this->objects = initListMgr();
	ListManager* objects = (ListManager*) jsonGetValue(conf, "objects", NULL);
	
	Log::dbg(LOG_ENV_OBJ, "-- Object Count: %d", objects->nodeCount);
	this->loadObjects(objects);

	free(sprite);
}

EnvSection::~EnvSection() {
	free(this->sprite);
	deleteList(this->objects);
}

void deleteSectionObjects(Node* n) {
	if (n->value == NULL) {
		return;
	}

	EnvObj* obj = (EnvObj*) n->value;
	delete obj;
}
void EnvSection::loadObjects(ListManager* objects) {
	Log::inf(LOG_ENV_OBJ, "=== Loading Objects ===");
	Node* n = NULL;

	while ((n = listIterate(objects, n)) != NULL) {
		Log::dbg(LOG_ENV_OBJ, "+++++ Obj: #%d", n->id);

		Json* objJ = (Json*) n->value;
		char* sprite = (char*) jsonGetValue(objJ, "sprite", NULL);
		Log::dbg(LOG_ENV_OBJ, "-- sprite: %s", sprite);

		vector pos = this->pos;
		Json* posJ = jsonGetData(objJ, "pos");

		int data[3];
		jsonGetValue(posJ, "x", &(data[0]));
		jsonGetValue(posJ, "y", &(data[1]));
		jsonGetValue(posJ, "z", &(data[2]));
		
		pos.x += (double) data[0];
		pos.y += (double) data[1];

		Log::dbg(LOG_ENV_OBJ, "-- Pos: X: %lf | Y: %lf | Z: %d", pos.x, pos.y, data[2]);



		int len = 13 + strlen(this->name) + strlen(sprite) + (n->id / 10);
		char name[len];
		memset(name, 0, len);
		snprintf(name, len, "section-%s_#%d_%s", this->name, n->id, sprite);

		Log::dbg(LOG_ENV_OBJ, "-- name: %s", name);
		EnvObj* obj = new EnvObj(name, this->sprite, sprite, &pos, data[2]);

		Node* objN = addNodeV(this->objects, name, obj, false);
		objN->del = deleteSectionObjects;

		free(sprite);
	}
}