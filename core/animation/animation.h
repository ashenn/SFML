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
		Animation(Object* obj, float time, float delay, void (Animator::*fnc)(Animation*));
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

		void (Animator::*fnc)(Animation* param);
		void (*stepFnc)(Animation* param);
		void (*callback)(Animation* param);
};


class MoveAnim : public Animation
{
	public:
		MoveAnim(Object* obj, float time, float delay, void (Animator::*fnc)(Animation*));
		~MoveAnim();

		float duration = 0;
		bool breakOnReach = false;

		IntRect targetPos;
		AnimDistance move[2];
	
};


class Animator : public AbstractStaticClass
{
	private:
		Animator();
		~Animator();

		ListManager* moves = NULL;
		ListManager* sprites = NULL;

		void addMoveObject(Object* obj, Animation* param);


	public:
		void operator=(Animator const&)  = delete;
		Animation* moveTo(Object* obj, int x, int y, float time, float delay);
		void animMoveTo(Animation* param);

		static Animator* get(bool deleteInst = false) {
			static Animator* instance = NULL;

			if (deleteInst) {
				if (instance != NULL) {
					delete instance;
				}

				return NULL;
			}

			if (instance != NULL) {
				return instance;
			}

			instance = new Animator();

			return instance;
		}
};

#endif