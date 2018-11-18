#ifndef SPRITE_OBJET_H
#define SPRITE_OBJET_H

#include "../object.h"
#include "../../asset/asset.h"
#include "../../animation/sprite/spriteAnim.h"

#include "../../event/eventMgr.h"

// Animated Object
class SpriteObj : public Object
{
	private:
		void loadConfig(const char* path);	// Load Animation Config In Asset Folder
		void loadSheet(const Json* json);	// Load Sprite Image
		void loadAnims(const Json* json);	// Init Animations

		void addAnim(int i, const Json* anim);	// Add Anim To List
		void loadAnim(const char* name, const Json* animJson, int* data);	// Loading Animation Data
		void loadAnimLinks(const char* name, const Json* linkJson, SpriteAnimData* anim); 	// Loading Animation Links
		void loadAnimCollisions(const char* name, const Json* colsJson, SpriteAnimData* anim); 	// Loading Animation Links


		SpriteAnim* anim = NULL;
		ListManager* animLinkFncs = NULL;	// Animation Links Funtions

		template<typename T>
		void clearAnimLinks(T* obj);

	protected:
		IntRect* curClip;
		unsigned int clipIndex = 0;		

		ListManager* animList = NULL;	// List Of Object Animations

		unsigned short cell_x = 0;		// Sprite Cell Size
		unsigned short cell_y = 0;		// Sprite Cell Size

		unsigned short spriteRows = 0;		// Number Of Row In Sprite Sheet
		unsigned short spriteColumns = 0;	// Number Of Row In Sprite Sheet

		void initAnimLinkFnc();

		bool moving = false;

	public:
		template<typename T>
		AnimLinkFnc<T>* getAnimLinkFnc(const char* name, T* obj);

		vector getCellSize();
		ListManager* getAnimList();

		void clearAnimLinks();

		template<typename T>
		void addAnimLinkFnc(const char* name, T* target, bool (T::*fnc)());

		~SpriteObj();
		SpriteObj(const char* name, vector* pos, unsigned short z, const char* path, bool visible);
		SpriteObj(const char* name, vector* pos, unsigned short z, const char* path) : SpriteObj(name, pos, z, path, true) {}

		
		SpriteAnim* getCurrentAnim();

		bool isMoving();	// Anim Sprite Test
		bool isStopped();	// Anim Sprite Test

		void startMove(KeyEvt<SpriteObj>* evt);
		void stopMove(KeyEvt<SpriteObj>* evt);
		void animate(const char* name, unsigned int clipIndex);

		virtual void draw(RenderWindow* window, bool grav);
};

#include "spriteObject.tpp"

#endif