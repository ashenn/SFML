#ifndef SPRITE_OBJET_H
#define SPRITE_OBJET_H

#include "../object.h"
#include "../../asset/asset.h"
#include "../../animation/sprite/spriteAnim.h"

class SpriteObj : public Object
{
	private:
		void loadConfig(const char* path);
		void loadSheet(const Json* json);
		void loadAnims(const Json* json);

		void addAnim(int i, const Json* anim);
		void loadAnim(const char* name, const Json* animJson, int* data);
		void loadAnimLinks(const char* name, const Json* linkJson, SpriteAnimData* anim);


		ListManager* animLinkFncs = NULL;

		template<typename T>
		void clearAnimLinks(T* obj);


	protected:
		IntRect* curClip;
		unsigned int clipIndex = 0;

		ListManager* animList = NULL;

		unsigned short cell_x = 0;
		unsigned short cell_y = 0;

		unsigned short spriteRows = 0;
		unsigned short spriteColumns = 0;

		template<typename T>
		void addAnimLinkFnc(const char* name, bool (T::*fnc)());

		void initAnimLinkFnc();

	public:
		template<typename T>
		AnimLinkFnc<T>* getAnimLinkFnc(const char* name, T* obj);

		ListManager* getAnimList();

		vector getCellSize();

		~SpriteObj();
		SpriteObj(const char* name, vector* pos, unsigned short z, const char* path, bool visible);
		SpriteObj(const char* name, vector* pos, unsigned short z, const char* path) : SpriteObj(name, pos, z, path, true) {}
	
		bool test();
};

#include "spriteObject.tpp"

#endif