#include "multiTexture.h"

static AssetMgr* ast = AssetMgr::get();
sf::Texture* MultiTexture::load(const char* path, const char* name) {
	Log::inf(LOG_MUL_TEXT, "=== Loading Multi Texture: '%s' ===", path);
	char msg[5000];
	memset(msg, 0, 5000);

	Json* spriteConf = ast->getJson(path);
	if (spriteConf == NULL) {
		snprintf(msg, 5000, "Fail To Find Texture Conf: '%s'", path);
		throw(new Exception(LOG_MUL_TEXT, msg));
	}

	Json* conf = jsonGetData(spriteConf, name);
	if (conf == NULL) {
		snprintf(msg, 5000, "Fail To Find Texture '%s' In Conf: '%s'", name, path);
		throw(new Exception(LOG_MUL_TEXT, msg));
	}

	char* sheetPath = (char*) jsonGetValue(conf, "sheet", NULL);
	if (sheetPath == NULL) {
		snprintf(msg, 5000, "No Sheet Path For Texture '%s' In Conf: '%s'", name, path);
		throw(new Exception(LOG_MUL_TEXT, msg));
	}

	Image* sheet = ast->getImage(sheetPath);
	if (sheet == NULL) {
		snprintf(msg, 5000, "Fail To Find Sheet '%s' For MultiText: '%s'", sheetPath, path);
		throw(new Exception(LOG_MUL_TEXT, msg));
	}
	free(sheetPath);

	Json* sizeJ = jsonGetData(conf, "spriteSize");
	int size[2];
	jsonGetValue(sizeJ, "0", &size[0]);
	jsonGetValue(sizeJ, "1", &size[1]);


	Log::inf(LOG_MUL_TEXT, "++++ SIZE: %p", sizeJ);
	Log::inf(LOG_MUL_TEXT, "-- X: %d", size[0]);
	Log::inf(LOG_MUL_TEXT, "-- Y: %d", size[1]);

	bool repeat = false;
	jsonGetValue(conf, "repeat", &repeat);

	Log::inf(LOG_MUL_TEXT, "++++ Creating Texture:");
	sf::Texture* text = new sf::Texture();
	text->create(size[0], size[1]);
	text->setRepeated(true);


	Json* colorJ = jsonGetData(conf, "color");
	int	color[4];
	jsonGetValue(colorJ, "0", &(color[0]));
	jsonGetValue(colorJ, "1", &(color[1]));
	jsonGetValue(colorJ, "2", &(color[2]));
	jsonGetValue(colorJ, "3", &(color[3]));

	Log::inf(LOG_MUL_TEXT, "++++ Setting Background Color:");
	Log::inf(LOG_MUL_TEXT, "-- R: %d", color[0]);
	Log::inf(LOG_MUL_TEXT, "-- G: %d", color[1]);
	Log::inf(LOG_MUL_TEXT, "-- B: %d", color[2]);
	Log::inf(LOG_MUL_TEXT, "-- A: %d", color[3]);

	sf::Color c(color[0], color[1], color[2], color[3]);

	Log::inf(LOG_MUL_TEXT, "++++ Creating Background Color:");
	sf::Image backColor;
	backColor.create(text->getSize().x, text->getSize().y, c);

	Log::inf(LOG_MUL_TEXT, "++++ Applying Background Color:");
	text->update(backColor);

	Log::inf(LOG_MUL_TEXT, "++++ Getting Sprites:");
	ListManager* sprites = (ListManager*) jsonGetValue(conf, "sprites", NULL);
	loadSprites(sprites, text, sheet);

	return text;
}

void MultiTexture::loadSprites(ListManager* sprites, sf::Texture* text, sf::Image* sheet) {
	Log::inf(LOG_MUL_TEXT, "-- Count: %d", sprites->nodeCount);

	Node* n = NULL;
	while ((n = listIterate(sprites, n)) != NULL) {
		Json* sprite = (Json*) n->value;
		Log::inf(LOG_MUL_TEXT, "++++ Sprite: %s", sprite->key);
		
		Json* posJ = jsonGetData(sprite, "pos");
		Json* clipJ = jsonGetData(sprite, "clip");

		int pos[2];
		jsonGetValue(posJ, "0", &pos[0]);
		jsonGetValue(posJ, "1", &pos[1]);

		IntRect clip(0,0,0,0);
		jsonGetValue(clipJ, "top", &(clip.top));
		jsonGetValue(clipJ, "left", &(clip.left));
		jsonGetValue(clipJ, "width", &(clip.width));
		jsonGetValue(clipJ, "height", &(clip.height));

		Log::inf(LOG_MUL_TEXT, "++++ Creating Tmp Img:");
		Log::inf(LOG_MUL_TEXT, "-- Width: %d", clip.width);
		Log::inf(LOG_MUL_TEXT, "-- Height: %d", clip.height);


		sf::Texture img;
		img.loadFromImage(*sheet, clip);

		Log::inf(LOG_MUL_TEXT, "++++ Test Texture:");
		Log::inf(LOG_MUL_TEXT, "-- Width: %d", img.getSize().x);
		Log::inf(LOG_MUL_TEXT, "-- Height: %d", img.getSize().y);

		Log::inf(LOG_MUL_TEXT, "++++ Test Texture:");
		Log::inf(LOG_MUL_TEXT, "-- X: %d", pos[0]);
		Log::inf(LOG_MUL_TEXT, "-- Y: %d", pos[1]);

		Log::inf(LOG_MUL_TEXT, "++++ Requested Size:");
		Log::inf(LOG_MUL_TEXT, "-- Width: %d", img.getSize().x + pos[0]);
		Log::inf(LOG_MUL_TEXT, "-- Height: %d", img.getSize().y + pos[1]);

		text->update(img.copyToImage(), (unsigned int) pos[0], (unsigned int) pos[1]);
	}
}