#ifndef ANIMATION_H
#define ANIMATION_H

#include "../object/object.h"

typedef struct AnimDistance {
	int rest;
	int perFrame;
	int dispatch;
	int curDispatch;
} AnimDistance;

class Animator;

class Animation : public AbstractClass
{
	public:
		Animation(Object* obj);
		Animation(Object* obj, float time, float delay);
		~Animation();

		Object* obj = NULL;

		float delay = 0;
		bool done = false;
		bool loop = false;
		bool breakAnim = false;
		bool deleteOnDone = false;
		bool deleteObject = false;

		int frames = 0;
		int initialFrames = 0;

		virtual void fnc() = 0;
		void setId(unsigned int id);

		//void (Animator::*fnc)(Animation* anim);
		void (*stepFnc)(Animation* anim);
		void (*callback)(Animation* anim);
};


class MoveAnim : public Animation
{
	public:
		MoveAnim(Object* obj, float time, float delay);
		~MoveAnim();

		float duration = 0;
		bool breakOnReach = false;

		IntRect targetPos;
		AnimDistance move[2];

		void fnc();
	
};


class Animator : public AbstractStaticClass
{
	private:
		Animator();
		~Animator();

		ListManager* moves = NULL;
		ListManager* sprites = NULL;

		void animateMove();
		void animateSprite();
		void addMoveObject(Object* obj, Animation* anim);


	public:
		STATIC_CLASS_BODY(Animator)

		void animMoveTo(Animation* anim);
		Animation* moveTo(Object* obj, int x, int y, float time, float delay);

		void animate();
		void addSprite(Animation* animParam);
		void spriteRemoveObject(Object* obj);
};

#endif