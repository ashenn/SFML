#ifndef ASSET_H
#define ASSET_H

#include "../../base/json.h"
#include "../abstractClass.h"

using namespace sf;

class AssetMgr : public AbstractStaticClass
{
	private:
		AssetMgr();
		~AssetMgr();
		//ListManager* imgs;
		//ListManager* fonts;
		ListManager* jsons;
		ListManager* confs;
		ListManager* textures;

	public:
		STATIC_CLASS_BODY(AssetMgr)
	
		//Texture* getFont(const char* path);
		Json* getJson(const char* path);
		Json* getConf(const char* path);
		Texture* getTexture(const char* path);

		void* isCached(const char* name, ListManager* cont);
		Node* cache(const char* name, ListManager* cont, void* data);
};

#endif