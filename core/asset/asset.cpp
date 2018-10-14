#include "asset.h"
#include "multiTexture/multiTexture.h"

AssetMgr::AssetMgr() {
	STATIC_CLASS_CONSTRUCT(AssetMgr)

	this->imgs = initListMgr();
	this->jsons = initListMgr();
	this->confs = initListMgr();
	this->textures = initListMgr();
	this->multiTextures = initListMgr();
}

AssetMgr::~AssetMgr() {
	deleteList(this->imgs);
	deleteList(this->jsons);
	deleteList(this->confs);
	deleteList(this->textures);
	deleteList(this->multiTextures);
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

void delJson(Node* n) {
	if (n->value == NULL) {
		return;
	}

	deleteJson((Json*) n->value);
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

		Node* n = this->cache(path, this->jsons, json);
		n->del = delJson;
	}
	else{
		Log::dbg(LOG_ASSET, "-- isCached");
	}

	return json;
}

void deleteImage(Node* n) {
	Image* img = (Image*) n->value;
	delete img;
}

Image* AssetMgr::getImage(const char* path) {
	Log::inf(LOG_ASSET, "=== GETTING IMG: %s ===", path);
	Image* img = (Image*) this->isCached(path, this->imgs);

	if (img != NULL) {
		Log::dbg(LOG_ASSET, "-- isCached");
		return img;
	}

	int len = strlen(path) + 19;

	char imgPath[len];
	memset(imgPath, 0, len);

	snprintf(imgPath, len, "asset/%s.png", path);
	validatePath(imgPath);

	Log::dbg(LOG_ASSET, "-- Loading File: %s", imgPath);

	img = new Image();
	if (!img->loadFromFile(imgPath)) {
		Log::err(LOG_ASSET, "Fail To Load Image: %s", imgPath);
		return NULL;
	}

	Node* n = this->cache(path, this->imgs, img);
	n->del = deleteImage;

	return img;
}

void deleteTexture(Node* n) {
	Texture* text = (Texture*) n->value;
	delete text;
}

Texture* AssetMgr::getTexture(const char* path) {
	Log::inf(LOG_ASSET, "=== GETTING Texture: %s ===", path);
	Texture* text = (Texture*) this->isCached(path, this->textures);
	

	if (text != NULL) {
		Log::dbg(LOG_ASSET, "-- isCached");
		return text;
	}

	int len = strlen(path) + 19;

	char imgPath[len];
	memset(imgPath, 0, len);

	snprintf(imgPath, len, "asset/%s.png", path);
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

Texture* AssetMgr::getMultiTexture(const char* path, const char* name) {
	Log::inf(LOG_ASSET, "=== GETTING Multi-Texture: %s ===", path);
	Texture* text = (Texture*) this->isCached(path, this->multiTextures);

	if (text != NULL) {
		Log::dbg(LOG_ASSET, "-- isCached");
		return text;
	}

	try {
		text = MultiTexture::load(path, name);
	}
	catch(Exception e) {
		Log::err(LOG_ASSET, "Fail To Load Multi-Texture: '%s'", path);
		return NULL;
	}

	Node* n = this->cache(path, this->multiTextures, text);
	n->del = deleteTexture;

	return text;
}

Texture* AssetMgr::getSheet(const char* n) {
	int len = strlen(n) + 8;
	char name[len];
	memset(name, 0, len);
	snprintf(name, len, "sheet/%s", n);

	return this->getTexture(name);
}

Json* AssetMgr::getConf(const char* path) {
	Json* json = (Json*) this->isCached(path, this->confs);

	if (json != NULL) {
		return json;
	}

	char jsonPath[350];
	memset(jsonPath, 0, 350);
	snprintf(jsonPath, 350, "config/%s.json", path);

	Log::dbg(LOG_ASSET, "-- Fething File: %s", jsonPath);
	json = loadJsonFile(jsonPath);

	if (json != NULL) {
		Node* n = this->cache(jsonPath, this->confs, json);
		n->del = delJson;

		return json;
	}

	if (strstr(path, ".default.json") != NULL) {
		Log::err(LOG_ASSET, "-- Fail to get Config: %s", jsonPath);
		Log::dbg(LOG_ASSET, "==== GETTING JSON FAILD ====");
		return NULL;
	}

	Log::inf(LOG_ASSET, "-- Fail to get Config: %s", jsonPath);
	Log::inf(LOG_ASSET, "-- Getting Default Config: %s", jsonPath);

	json = this->loadDefaultConf(path);
	if (json != NULL) {
		Node* n = this->cache(jsonPath, this->confs, json);
		n->del = delJson;

		return json;
	}

	return NULL;
}


Json* AssetMgr::loadDefaultConf(const char* src) {
	Log::inf(LOG_ASSET, "=== Loading Default Conf: %s", src);

	char srcPath[150];
	memset(srcPath, 0, 150);
	snprintf(srcPath, 150, "%s.default", src);

	Json* json = this->getConf(srcPath);

	char* txt = json2Str(json, false, false);

	char confPath[150];
	memset(confPath, 0, 150);
	snprintf(confPath, 150, "config/%s.json", src);
	
	filePutContent(confPath, txt, false);

	free(txt);

	return loadJsonFile(confPath);
}