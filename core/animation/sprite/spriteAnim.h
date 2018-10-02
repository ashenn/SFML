#ifndef SPRITE_ANIM
#define SPRITE_ANIM

#include "../animation.h"

typedef struct AnimLinkFnc AnimLinkFnc;

typedef struct AnimLink
{
	char* name;
	bool waitEnd;
	char* target;
	// bool (*fnc)(SpriteAnim*);
} AnimLink;


class SpriteAnimData : public AbstractClass {
	public:
		SpriteAnimData(const char* name);
		~SpriteAnimData();

		bool loop;
		Object* obj;
		unsigned int fps;
		unsigned int row;
		unsigned int wait;
		unsigned int animID;
		unsigned int duration;
		unsigned int startIndex;
		
		IntRect** clipPos = NULL;
		unsigned int clipCnt;

		ListManager* animLinks = NULL;
};


class SpriteObj;
class SpriteAnim : public Animation
{
	private:
		static ListManager* animLinkFncs;

		IntRect clip;
		unsigned int wait;
		unsigned int animID;
		unsigned short clipMax;
		unsigned short clipIndex;

		SpriteAnimData* anim = NULL;

		static SpriteAnim* callAnim(SpriteObj* obj, SpriteAnimData* anim, unsigned int clipIndex);

	public:
		SpriteAnim(Object* obj) : Animation(obj) {}
		~SpriteAnim();
		void fnc();

		static void addAnimLinkFnc(const char* name, bool (*fnc)(SpriteAnim*));
		static AnimLinkFnc* getAnimLinkFnc(const char* name);
		static void clearAnimLinkFnc();
	
		static SpriteAnim* animate(SpriteObj* obj, const char* name, unsigned int clipIndex);
};

struct AnimLinkFnc {
	bool (*fnc)(SpriteAnim*);
};

#include "../../object/sprite/spriteObject.h"
#endif