#ifndef ANIMATION_H
#define ANIMATION_H

#include "../object/object.h"

// Animation Move Distance (One Per Axis)
typedef struct AnimDistance {
	int perFrame;		// Pixels Per Frames
	int rest;			// Rest Of Per Frames Calculation
	int dispatch;		// Use To delay movement
	int curDispatch;	// Current Dispatch Index
} AnimDistance;

class Animation;

class AnimCallbackAbstract : public AbstractClass {
	public:
		virtual void trigger() = 0;
};

template<class T>
class AnimCallback : public AnimCallbackAbstract {
	public:
		T* target;
		void (T::*fnc)();

		void trigger() {
			T* t= this->target;
			(t->*(this->fnc))();
		}
};


// Base Of Animation
class Animation : public AbstractClass
{
	protected:

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

		int frames = 0;			// Anim Frames Left
		int initialFrames = 0;	// Total Anim Frames

		virtual void fnc() = 0;	// Animation Function
		void setId(unsigned int id);

		AnimCallbackAbstract* callBack = NULL;

		template<class T>
		void triggerCallBack(T* obj) {
			if (this->callBack == NULL) {
				return;
			}

			this->callBack->trigger();
		}

		void removeCallBack() {
			if (this->callBack != NULL) {
				delete this->callBack;
			}
		}

		template<class T>
		void setCallBack(T* targ, void (T::*fnc)()) {
			this->removeCallBack();

			AnimCallback<T>* cl = new AnimCallback<T>();
			cl->fnc = fnc;
			cl->target = targ;

			this->callBack = cl;
		}
		//void (Animator::*fnc)(Animation* anim);
		// void (*stepFnc)(Animation* anim);
		// void (*callback)(Animation* anim);
};

// Movement Animation
class MoveAnim : public Animation
{
	public:
		MoveAnim(Object* obj, float time, float delay);
		~MoveAnim();


		//float duration = 0;

		// Break Animation If Position Reached Before Time
		bool breakOnReach = false;

		//IntRect targetPos;
		AnimDistance move[2];

		void fnc();
};

// Animation Manager Singleton
class Animator : public AbstractStaticClass
{
	private:
		Animator();
		~Animator();

		// List Of Obect Movemet Animations
		ListManager* moves = NULL;

		// List Of Obect Sprite Animations
		ListManager* sprites = NULL;

		// Run Move Animation
		void animateMove();

		// Run Sprite Animation
		void animateSprite();

		// Add Object To Move List
		void addMoveObject(Object* obj, Animation* anim);


	public:
		// SingletonDeclaration
		STATIC_CLASS_BODY(Animator)

		//void animMoveTo(Animation* anim);
		
		// Move Object To Position
		MoveAnim* moveTo(Object* obj, int x, int y, float time, float delay);

		// Run One Frame Animation
		void animate();

		// Add Object To Sprite List
		void addSprite(Animation* animParam);

		// Remove Object From Move List
		void removeMove(Object* obj);

		// Remove Object From Sprite List
		void removeSprite(Object* obj);
};

#endif