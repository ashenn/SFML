#include "spriteObject.h"

SpriteObj::SpriteObj(const char* name, vector* pos, unsigned short z, const char* path, bool visible) : Object (name, pos, NULL, NULL, z, visible) {
	Log::inf(LOG_SPRITE_OBJ, "=== Create Sprite Obj: %s ===", name);

	this->animList = initListMgr();
	this->loadConfig(path);
}

SpriteObj::~SpriteObj() {
	if (this->animList != NULL) {
		deleteList(this->animList);
		this->animList = NULL;
	}
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
	this->loadAnims(json);
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


	int values[5];
	jsonGetValue(sheet, "rows", &values[0]);
	this->spriteRows = values[0];
	Log::dbg(LOG_SPRITE_OBJ, "-- Rows: %d", this->spriteRows);

	jsonGetValue(sheet, "columns", &values[1]);
	this->spriteColumns = values[1];
	Log::dbg(LOG_SPRITE_OBJ, "-- Columns: %d", this->spriteColumns);

	jsonGetValue(sheet, "cell_x", &values[2]);
	this->cell_x = values[2];
	Log::dbg(LOG_SPRITE_OBJ, "-- Cell_x: %d", this->cell_x);

	jsonGetValue(sheet, "cell_y", &values[3]);
	this->cell_y = values[3];
	Log::dbg(LOG_SPRITE_OBJ, "-- Cell_y: %d", this->cell_y);

	this->curClip = new IntRect(0, 0, this->cell_x, this->cell_y);

	this->setTexture(text);
	this->setClip(this->curClip, true);
}

void spriteAnimDataDelete(Node* n) {
	SpriteAnimData* animData = (SpriteAnimData*) n->value;

	SpriteObj* obj = (SpriteObj*) animData->obj;
	vector size = obj->getCellSize();
	obj->setClip(new IntRect(0, 0, size.x, size.y), false);
	
	delete animData;
}

void SpriteObj::loadAnim(const char* name, const Json* animJson, int* data) {
	Log::inf(LOG_SPRITE_OBJ, "======= Loading Anim:  %s =====", name);
	SpriteAnimData* animData = new SpriteAnimData(name);
	
	animData->obj = this;
	animData->fps = data[0];
	animData->row = data[3];
	animData->startIndex = data[2];
	animData->clipCnt = data[1];


	jsonGetValue(animJson, "loop", &animData->loop);
	Log::dbg(LOG_SPRITE_OBJ, "-- Loop: %d", animData->loop);


	animData->wait = (FPS / animData->fps);

	animData->duration = (animData->fps * animData->clipCnt);

	Log::dbg(LOG_SPRITE_OBJ, "-- Calloc Anim Array[%d]:  %s", animData->clipCnt, name);
	animData->clipPos = (IntRect**) calloc(animData->clipCnt+1, sizeof(IntRect*));

    int i = animData->startIndex;
	unsigned short curRow = animData->row;

	for (unsigned int a = 0; a < animData->clipCnt; ++a) {
		if (i >= this->spriteColumns) {
			i = 0;
			curRow++;
		}

		animData->clipPos[a] = new IntRect(i * this->cell_x, curRow * this->cell_y, this->cell_x, this->cell_y);
		IntRect* pos = animData->clipPos[a];
		
		Log::dbg(LOG_SPRITE_OBJ, "-- X: %d | Y: %d  W: %d | H: %d", pos->left, pos->top, pos->width, pos->height);
		i++;
	}

	Node* n = addNodeV(this->animList, name, animData, false);
	animData->animID = n->id;
	n->del = spriteAnimDataDelete;

	Json* linkJson = jsonGetData(animJson, "links");

	this->loadAnimLinks(name, linkJson, animData);
}

void AnimLinkDelete(Node* n) {
	AnimLink* link = (AnimLink*) n->value;

	if (link->name != NULL) {
		free(link->name);
	}

	if (link->target != NULL) {
		free(link->target);
	}
}

void SpriteObj::loadAnimLinks(const char* name, const Json* linkJson, SpriteAnimData* anim) {
	Log::inf(LOG_SPRITE_OBJ, "=== Loading Anim Links For: %s ===", name);

	Node* n = NULL;
	while ((n = listIterate(linkJson->childs, n)) != NULL) {
		Json* linkJson = (Json*) n->value;
		Log::dbg(LOG_SPRITE_OBJ, "-- link: %s", linkJson->key);

		AnimLink* link = (AnimLink*) malloc(sizeof(AnimLink));

		jsonGetValue(linkJson, "waitEnd", &(link->waitEnd));
		link->target = (char*) jsonGetValue(linkJson, "target", NULL);
		link->name = (char*) jsonGetValue(linkJson, "fnc", NULL);
		
		char linkName[200];
		memset(linkName, 0, 200);
		snprintf(linkName, 200, "%s_2_%s", name, link->target);
		

		Log::dbg(LOG_SPRITE_OBJ, "-- Link: %s", linkName);
		Log::dbg(LOG_SPRITE_OBJ, "-- name: %s", link->name);
		Log::dbg(LOG_SPRITE_OBJ, "-- target: %s", link->target);
		Log::dbg(LOG_SPRITE_OBJ, "-- waitEnd: %d", link->waitEnd);


		Log::inf(LOG_SPRITE, "-- Adding Link Fnc To List");
		Node* n = addNodeV(anim->animLinks, linkName, link, true);
		n->del = AnimLinkDelete;
	}
}

void SpriteObj::addAnim(int i, const Json* animJson) {
	Log::inf(LOG_SPRITE_OBJ, "======= Init Anim: #%d => %s =====", i, animJson->key);


	int data[5];
	data[4] = 0;
	char* name = (char*) jsonGetValue(animJson, "name", NULL);

	jsonGetValue(animJson, "fps", &(data[0]));
	jsonGetValue(animJson, "count", &(data[1]));
	jsonGetValue(animJson, "index", &(data[2]));
	jsonGetValue(animJson, "row", &(data[3]));

	Log::dbg(LOG_SPRITE_OBJ, "Adding Anim #%d => %s", i, name);
	Log::dbg(LOG_SPRITE_OBJ, "-- fps: %d", data[0]);
	Log::dbg(LOG_SPRITE_OBJ, "-- count: %d", data[1]);
	Log::dbg(LOG_SPRITE_OBJ, "-- index: %d", data[2]);
	Log::dbg(LOG_SPRITE_OBJ, "-- row: %d", data[3]);


	this->loadAnim(name, animJson, data);

	free(name);
}

void SpriteObj::loadAnims(const Json* json) {
	Log::inf(LOG_SPRITE_OBJ, "-- Getting Anims:");
	
	Json* anims = jsonGetData(json, "anims");
	Log::inf(LOG_SPRITE_OBJ, "-- Anims Count: %d", anims->childs->nodeCount);

	int i = 0;
	Node* n = NULL;
	while ((n = listIterate(anims->childs, n)) != NULL) {
		Json* anim = (Json*) n->value;

		this->addAnim(i++, anim);
	}

	Log::dbg(LOG_SPRITE, "=== Sprite Obj Ready ===");

	if (!this->animList->nodeCount) {
		return;
	}

	SpriteAnimData* curAnim = (SpriteAnimData*) this->animList->first->value;
	SpriteAnim::animate(this, curAnim->getName(), 0);
}


ListManager* SpriteObj::getAnimList() {
	return this->animList;
}

vector SpriteObj::getCellSize() {
	vector size;
	size.x = this->cell_x;
	size.y = this->cell_y;

	return size;
}