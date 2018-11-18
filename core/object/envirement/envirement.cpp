#include "envirement.h"
#include "../../asset/asset.h"
#include "../../collision/collision.h"
#include "../../controller/ai/aiCtrl.h"

EnvObj::EnvObj(const char* name, const char* path, const char* conf, vector* pos, unsigned int z, IntRect* resize) :
EnvObj(name, path, conf, pos, z) {
	Log::err(LOG_CHAR, "RESIZE Height: %d | Width: %d!!!!", resize->height, resize->width);
	if (!resize->width) {
		resize->width = this->clip->width;
	}

	if (!resize->height) {
		resize->height = this->clip->height;
	}


	Log::dbg(LOG_CHAR, "RESIZE Height: %d | Width: %d!!!!", resize->height, resize->width);
	this->setClip(resize, true);
}

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
	Log::dbg(LOG_CHAR, "TEST Height: %d | Width: %d!!!!", objPos->height, objPos->width);
	Log::dbg(LOG_CHAR, "DEFAULT Height: %d | Width: %d!!!!", this->clip->height, this->clip->width);

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

	//jsonPrint(conf, 0);
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

	this->monsters = initListMgr();
	ListManager* monsters = (ListManager*) jsonGetValue(conf, "monsters", NULL);
	this->spawnMonsters(monsters);

	free(sprite);
}

EnvSection::~EnvSection() {
	free(this->sprite);
	deleteList(this->objects);
	deleteList(this->monsters);
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

		EnvObj* obj = NULL;
		Json* resizeJ = jsonGetData(objJ, "resize");

		if (resizeJ != NULL && resizeJ->type != JSON_NULL) {
			Log::err(LOG_CHAR, "RESIZE FOUND !!!!");
			IntRect* resizeR = new IntRect(0,0,0,0);

			Json* resize = jsonGetData(resizeJ, "width");
			if (resize != NULL && resize->type != JSON_NULL) {
				resizeR->width = resize->integer;
				Log::err(LOG_CHAR, "RESIZE Width: %s %d!!!!", getJsonTypeName(resize->type), resizeR->width);
			}

			resize = jsonGetData(resizeJ, "height");
			if (resize != NULL && resize->type != JSON_NULL) {
				resizeR->height = resize->integer;
				Log::err(LOG_CHAR, "RESIZE Height: %d!!!!", resizeR->height);
			}

			Log::err(LOG_CHAR, "RESIZE Height: %d | Width: %d!!!!", resizeR->height, resizeR->width);
			obj = new EnvObj(name, this->sprite, sprite, &pos, data[2], resizeR);
		}
		else{
			obj = new EnvObj(name, this->sprite, sprite, &pos, data[2]);
		}

		Node* objN = addNodeV(this->objects, name, obj, false);
		objN->del = deleteSectionObjects;

		free(sprite);
	}
}

void deleteSpawn(Node* n) {
	if (n->value == NULL) {
		return;
	}

	Log::dbg(LOG_ENV_OBJ, "DELETING MONSTER");
	AiCtrl* mon = (AiCtrl*) n->value;
	Log::dbg(LOG_ENV_OBJ, "%s", mon->getName());

	delete mon;
}

void EnvSection::spawnMonsters(ListManager* monsters) {
	Log::inf(LOG_ENV_OBJ, "=== Spawning Monsters: %d ===", monsters->nodeCount);

	Node* n = NULL;
	while ((n = listIterate(monsters, n)) != NULL) {
		Json* monJ = (Json*) n->value;

		Json* posJ = jsonGetData(monJ, "pos");

		vector pos;
		int data[3];
		jsonGetValue(posJ, "x", &(data[0]));
		jsonGetValue(posJ, "y", &(data[1]));
		jsonGetValue(posJ, "z", &(data[2]));

		pos.x = data[0];
		pos.y = data[1];

		char* key = (char*) jsonGetValue(monJ, "key", NULL);

		int len = strlen(key) + (n->id / 10) + 3;
		char name[len];
		snprintf(name, len, "%s_%d", key, n->id);

		Log::dbg(LOG_ENV_OBJ, "%s x: %lf Y: %lf Z: %d", name, pos.x, pos.y, data[2]);

		AiCtrl* mon = new AiCtrl(name, key, &pos, data[2], this);
		Node* monN = addNodeV(this->monsters, name, mon, false);
		monN->del = deleteAbstract;

		free(key);
	}
}

void EnvSection::removeAi(AiCtrl* ai) {
	Node* n = getNodeByValue(this->monsters, ai);
	if (n == NULL) {
		return;
	}

	n->del = NULL;
	removeAndFreeNode(this->monsters, n);
}