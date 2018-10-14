#include "envirement.h"
#include "../../asset/asset.h"
#include "../../collision/collision.h"

EnvObj::EnvObj(const char* name, const char* path, const char* conf, vector* pos, unsigned int z)
: Object(name, pos) {
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