#include "spriteObject.h"

SpriteObj::SpriteObj(const char* name, vector* pos, unsigned short z, const char* path, bool visible) : Object (name, pos, NULL, NULL, z, visible) {
	Log::inf(LOG_SPRITE_OBJ, "=== Create Sprite Obj: %s ===", name);

	this->animList = initListMgr();
	this->loadConfig(path);
}

SpriteObj::~SpriteObj() {
	deleteList(this->animList);
}

void SpriteObj::loadConfig(const char* path) {	
	Log::dbg(LOG_SPRITE_OBJ, "-- Loading Config: %s", path);

	AssetMgr* ast = AssetMgr::get();

	int len = strlen(path)+7;

	char jsonPath[len];
	memset(jsonPath, 0, len);
	snprintf(jsonPath, len, "sheet/%s", path);

	Log::dbg(LOG_SPRITE_OBJ, "-- Fetching Anim Json: %p", jsonPath);
	Json* json = ast->getJson(jsonPath);

	this->loadSheet(json);
}

void SpriteObj::loadSheet(const Json* json) {
	Log::dbg(LOG_SPRITE_OBJ, "-- Get Sheet Data");
	Json* sheet = jsonGetData(json, "sheet");

	char* sheetPath = (char*) jsonGetValue(sheet, "path", NULL);
	Log::dbg(LOG_SPRITE_OBJ, "-- Path: %s", sheetPath);

	AssetMgr* ast = AssetMgr::get();

	Texture* text = ast->getTexture(sheetPath);
	if (text == NULL) {
		char exp[300];
		memset(exp, 0, 300);
		snprintf(exp, 300, "Fail To Get Texture: %s", sheetPath);
		throw(Exception(0, exp));
	}

	this->setTexture(text);
}
