#ifndef SPRITE_ANIM
#define SPRITE_ANIM

#include "../animation.h"


template <typename T> 
struct AnimLinkFnc { 
	//T* obj;
	bool (T::*fnc)();
}; 

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
		IntRect clip;
		unsigned int wait;
		unsigned int animID;
		unsigned short clipMax;
		unsigned short clipIndex;

		SpriteAnimData* anim = NULL;

		void updateSprite();
		
		template<typename T>
		bool callAnimLinkFnc(T* obj, AnimLink* link);

		static SpriteAnim* callAnim(SpriteObj* obj, SpriteAnimData* anim, unsigned int clipIndex);

	public:
		~SpriteAnim();
		SpriteAnim(Object* obj);

		void fnc();
		//static void clearAnimLinkFnc();
	
		static SpriteAnim* animate(SpriteObj* obj, const char* name, unsigned int clipIndex);
};

void deleteAnimLinkFnc(Node* n);

#include "../../object/sprite/spriteObject.h"

#include "spriteAnim.tpp"

#endif