#ifndef SPRITE_ANIM
#define SPRITE_ANIM

#include "../animation.h"

// Used To Automatically Call Animtion From Object State
typedef struct AnimLink
{
	char* name;		// Link Name
	bool waitEnd;	// Must Current Animation End Before Calling Link
	char* target;	// Animation Link Function Name (If NULL Link Is Always Called)
	// bool (*fnc)(SpriteAnim*);
} AnimLink;

class AnimLinkFncAbstract {
	public:
		virtual ~AnimLinkFncAbstract(){};
		virtual bool call() = 0;
};

// Animation Link Function
template <class T> 
class AnimLinkFnc : AnimLinkFncAbstract { 
	private:
		T* target;
		// Pointer To Object Function
		bool (T::*fnc)();

	public:
		virtual ~AnimLinkFnc(){}

		AnimLinkFnc(T* targ, bool (T::*fnc)()) {
			this->fnc = fnc;
			this->target = targ;
		}
		
		bool call() {
			T* t = this->target;
			return (t->*(this->fnc))();
		}
};

class AnimColData : public AbstractClass {
	public:
		~AnimColData();
		AnimColData(Json* posJ);
		
		IntRect* pos = NULL;
		bool* enabled = NULL;
};

// Sprite Animation Information 
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
		unsigned int duration;		// total Frames
		unsigned int startIndex;	// Start At Frame Index
		
		IntRect** clipPos = NULL;	// Array Of Animation Rectangles
		unsigned int clipCnt;		// Number Of Animation Rectangles

		ListManager* animLinks = NULL;	// Animation Links
		ListManager* collisions = NULL;	// Animation Links
};

// Predeclaration Of SpriteObj Class
class SpriteObj;


// Sprite Animation
class SpriteAnim : public Animation
{
	private:
		IntRect clip;				// Current Animation Rectangle
		unsigned int wait;
		unsigned int animID;
		unsigned short clipMax;		// Maximum Clip Index
		unsigned short clipIndex;	// Current Clip Index

		SpriteAnimData* anim = NULL;	// Animation Data

		// Iterate Through Anim Links And Call first Valid
		void updateSprite();
		
		// Call Animation Link
		template<typename T>
		bool callAnimLinkFnc(T* obj, AnimLink* link);
		void setCollisions();

		// Apply Animation By Animation Data 
		static SpriteAnim* callAnim(SpriteObj* obj, SpriteAnimData* anim, unsigned int clipIndex);

	public:
		~SpriteAnim();
		SpriteAnim(Object* obj);

		void fnc();
		void clearAnimLinks();
	
		// Search And Call Animation By Animation Name
		static SpriteAnim* animate(SpriteObj* obj, const char* name, unsigned int clipIndex);
};

// Delete Anim Link On List Node Deleted
void deleteAnimLinkFnc(Node* n);

#include "../../object/sprite/spriteObject.h"

#include "spriteAnim.tpp"

#endif