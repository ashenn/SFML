#include "asset.h"

AssetMgr::AssetMgr() {
	STATIC_CLASS_CONSTRUCT(AssetMgr)

	this->jsons = initListMgr();
	this->confs = initListMgr();
	this->textures = initListMgr();
}

AssetMgr::~AssetMgr() {
	deleteList(this->jsons);
	deleteList(this->confs);
	deleteList(this->textures);
}

void* AssetMgr::isCached(const char* name, ListManager* cont) {
	Log::dbg(LOG_ASSET, "-- Search Cache: %s", name);
	Node* n = getNodeByName(cont, name);

	if (n != NULL) {
		Log::dbg(LOG_ASSET, "-- Cache Found: %d", n->id);
		return n->value;
	}

	return NULL;
}

Node* AssetMgr::cache(const char* name, ListManager* cont, void* data) {
	Log::dbg(LOG_ASSET, "-- Caching: %s", name);
	Node* n =  addNodeV(cont, name, data, 0);

	Log::dbg(LOG_ASSET, "-- Cache Node: %p", n);
	Log::dbg(LOG_ASSET, "-- id: %d", n->id);
	Log::dbg(LOG_ASSET, "-- name: %s", n->name);

	return n;
}

Json* AssetMgr::getJson(const char* path) {
	Json* json = (Json*) this->isCached(path, this->jsons);

	if (json == NULL){
		char jsonPath[350];
		memset(jsonPath, 0, 350);
		snprintf(jsonPath, 350, "asset/%s.json", path);

		Log::dbg(LOG_ASSET, "-- Fething File: %s", jsonPath);
		json = loadJsonFile(jsonPath);

		if (json == NULL){
			Log::err(LOG_ASSET, "-- Fail to get JSON: %s", jsonPath);
			Log::dbg(LOG_ASSET, "==== GETTING JSON FAILD ====");
			return NULL;
		}

		this->cache(path, this->jsons, json);
	}
	else{
		Log::dbg(LOG_ASSET, "-- isCached");
	}

	return json;
}

void deleteTexture(Node* n) {
	Texture* text = (Texture*) n->value;
	delete text;
}

Texture* AssetMgr::getTexture(const char* path) {
	Log::inf(LOG_ASSET, "=== GETTING IMG: %s ===", path);
	Texture* text = (Texture*) this->isCached(path, this->textures);
	

	if (text != NULL) {
		Log::dbg(LOG_ASSET, "-- isCached");
		return text;
	}

	char imgPath[100];
	snprintf(imgPath, 100, "asset/%s.png", path);
	validatePath(imgPath);

	Log::dbg(LOG_ASSET, "-- Loading File: %s", imgPath);

	text = new Texture();
	if (!text->loadFromFile(imgPath)) {
		Log::err(LOG_ASSET, "Fail To Load Texture: %s", imgPath);
		return NULL;
	}

	Node* n = this->cache(path, this->textures, text);
	n->del = deleteTexture;
	return text;
}