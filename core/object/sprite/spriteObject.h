#ifndef SPRITE_OBJET_H
#define SPRITE_OBJET_H

#include "../../asset/asset.h"
#include "../object.h"

class SpriteObj : public Object
{
	private:
		void loadConfig(const char* path);
		void loadSheet(const Json* json);

	protected:
		IntRect curClip;
		unsigned int clipIndex = 0;

		ListManager* animList = NULL;

		unsigned short cell_x = 0;
		unsigned short cell_y = 0;

		unsigned short spriteRows = 0;
		unsigned short spriteColumns = 0;

	public:
		SpriteObj(const char* name, vector* pos, unsigned short z, const char* path, bool visible);
		SpriteObj(const char* name, vector* pos, unsigned short z, const char* path) : SpriteObj(name, pos, z, path, true) {}
		~SpriteObj();
	

};

#endif