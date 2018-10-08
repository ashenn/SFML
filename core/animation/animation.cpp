#include "animation.h"


AnimDistance animDistanceByFrame(int dist, float time) {
	Log::inf(LOG_ANIM, "==== Calculation Animation Distance Per Frame ====");
	Log::dbg(LOG_ANIM, "-- dist: %dpx\n--time: %fs", dist, time);

	int rest;
	int frames;
	int perFrames;
	AnimDistance animDist;

	frames = FPS * time;
	if (frames < 1) {
		frames = 1;
	}

	Log::dbg(LOG_ANIM, "-- Total Frames: %d", frames);

	rest = dist % frames;
	perFrames = dist / frames;

	Log::dbg(LOG_ANIM, "-- Step per frame: %d\n--rest: %d", perFrames, rest);

	animDist.rest = rest % frames;
	animDist.perFrame = perFrames + (rest / frames);

	Log::dbg(LOG_ANIM, "-- dist per frame: %d\n--rest: %d", animDist.perFrame, animDist.rest);


	animDist.curDispatch = 0;
	if (rest) {
		Log::dbg(LOG_ANIM, "-- Calc Dispatch: %d / %d", frames, rest);
		animDist.dispatch = abs(frames / rest);
	}
	else {
		animDist.dispatch = 0;
	}


	Log::dbg(LOG_ANIM, "-- Dispatch: %d", animDist.dispatch);
	Log::dbg(LOG_ANIM, "==== Calculation DONE ====");

	return animDist;
}


Animator::Animator() {
	this->moves = initListMgr();
	this->sprites = initListMgr();
}

Animator::~Animator() {
	deleteList(moves);
	deleteList(sprites);
}


Animation::Animation(Object* obj, float time, float delay) {
	static int i = 0;
	this->obj = obj;

	int len = strlen(obj->getName()) + 15;

	char name[len];
	memset(name, 0, len);
	snprintf(name, len, "%s_SpriteAnim-%d", obj->getName(), i++);
	this->name = Str(name);
	// this->stepFnc = NULL;
	// this->callback = NULL;

	this->done = false;
	this->loop = false;
	this->breakAnim = false;
	this->deleteOnDone = false;
	this->deleteObject = false;

	this->delay = delay * FPS;
	this->initialFrames = this->frames = time * FPS;
}

Animation::Animation(Object* obj) : Animation(obj, 0.0f, 0.0f) {}

Animation::~Animation() {

	this->removeCallBack();
}

void Animation::setId(unsigned int id) {
	this->id = id;

	char name[150];
	memset(name, 0, 150);
	snprintf(name, 150, "Move Anim #%d => %s", this->id, obj->getName());

	if (this->name != NULL) {
		free(this->name);
		this->name = NULL;
	}

	this->name = Str(name);
}


MoveAnim::MoveAnim(Object* obj, float time, float delay) : Animation(obj, time, delay) {}

MoveAnim::~MoveAnim() {}

MoveAnim* Animator::moveTo(Object* obj, int x, int y, float time, float delay) {
	bool b = obj->lock("MoveTo-0");
	Log::inf(LOG_ANIM, "==== Animation: Moving: %s to %d | %d (%fs) ====", obj->getName(), x, y, time);

	vector targetPos;
	targetPos.x = x;
	targetPos.y = y;

	Log::dbg(LOG_ANIM, "-- Move To: %d | %d", x, y);
	vector vec = getVector(obj->getPosition(), targetPos);
	Log::dbg(LOG_ANIM, "-- Move Vector: %lf | %lf", vec.x, vec.y);

	MoveAnim* anim = new MoveAnim(obj, time, delay);

	Log::dbg(LOG_ANIM, "-- Time: %f", time);
	Log::dbg(LOG_ANIM, "-- Frames: %d", anim->frames);

	anim->move[0] = animDistanceByFrame(vec.x, time);
	Log::dbg(LOG_ANIM, "-- xTotDist: %d", anim->move[0].perFrame + anim->move[0].rest);

	anim->move[1] = animDistanceByFrame(vec.y, time);
	Log::dbg(LOG_ANIM, "-- yTotDist: %d", anim->move[1].perFrame + anim->move[1].rest);


    Log::dbg(LOG_ANIM, "==== ADDING OBJ TO ANIMATOR !!!!");

    this->addMoveObject(obj, (Animation*) anim);

    Log::inf(LOG_ANIM, "==== Animation Added ====");

	obj->unlock("MoveTo-1", b);
    return anim;
}

void deleteAnim(Node* n) {
	if (n->value == NULL) {
		return;
	}


	Animation* anim = (Animation*) n->value;

	Log::inf(LOG_ANIM, "Deleting Animation: %s", anim->getName());
	delete anim;
}

void Animator::addMoveObject(Object* obj, Animation* anim) {
	Log::inf(LOG_ANIM, "==== Adding Anim Object: %s ====", obj->getName());

	Log::dbg(LOG_ANIM, "FINDING NODE");

	this->removeMove(obj);
	//getNodeByName(this->moves, obj->getName());
	
	Node* paramNode = addNodeV(this->moves, obj->getName(), anim, false);
	
	anim->setId(paramNode->id);
	paramNode->del = deleteAnim;

	Log::dbg(LOG_ANIM, "==== Animation: Added %s #%d ====", obj->getName(), paramNode->id);
}


void Animator::animateMove() {
	Log::inf(LOG_ANIM, "=== Animating Movements ===");

	Node* n = NULL;
	while ((n = listIterate(this->moves, n)) != NULL) {
		Log::inf(LOG_ANIM, "--- Move: %s", n->name);

		Animation* anim = (Animation*) n->value;
		if (anim == NULL) {
			Node* tmp = n->prev;

			Log::dbg(LOG_ANIM, "-- Removing NULL Anim");
			removeAndFreeNode(this->moves, n);
			Log::dbg(LOG_ANIM, "-- Anim NULL Removed");

			n = tmp;
			continue;
		}

		if (anim->breakAnim) {
			Node* tmp = n->prev;
			Log::inf(LOG_ANIM, "-- Removing Anim");
			removeAndFreeNode(this->moves, n);
			Log::inf(LOG_ANIM, "-- Anim Removed");

			n = tmp;
			continue;
		}

		Log::inf(LOG_ANIM, "ANIMATING: %s", anim->obj->getName());
		if (anim->delay > 0) {
			Log::dbg(LOG_ANIM, "-- delayed: %d", anim->delay);
			anim->delay--;
			continue;
		}


		Object* obj = anim->obj;
		Log::dbg(LOG_ANIM, "-- Calling Anim Function: %s", anim->obj->getName());

		bool b = obj->lock("ANIMMATE-0");

		anim->fnc();

		obj->unlock("ANIMMATE-1", b);


		anim->frames--;
		Log::dbg(LOG_ANIM, "-- Frames Left: %d", anim->frames);

		if (anim->frames > 0 && !anim->breakAnim) {
			continue;
		}



		Log::dbg(LOG_ANIM, "-- Animation Ended");
		bool b1 = obj->lock("ANIMMATE-4");
		anim->triggerCallBack(obj);
		obj->unlock("ANIMMATE-5", b1);


		if (anim->loop) {
			Log::inf(LOG_ANIM, "-- Looping Animation");
			anim->frames = anim->initialFrames;
			continue;
		}
		else{
			Node* tmp = n->prev;
			Log::inf(LOG_ANIM, "-- Removing Anim");
			removeAndFreeNode(this->moves, n);
			Log::inf(LOG_ANIM, "-- Anim Removed");

			n = tmp;
		}
	}
}

void Animator::animate() {
	if (this->moves->nodeCount) {
		Log::inf(LOG_ANIM, "Objects Moves To Animate: #%d", this->moves->nodeCount);
		this->animateMove();
	}

	if (this->sprites->nodeCount) {
		Log::inf(LOG_ANIM, "Objects Sprites To Animate: #%d", this->sprites->nodeCount);
		this->animateSprite();
	}
}

void MoveAnim::fnc() {
	Log::inf(LOG_ANIM, "==== ANIM Move: %s ====", this->obj->getName());

	int xRest = this->move[0].rest;
	int xMove = this->move[0].perFrame;
	int xDispatch = this->move[0].dispatch;
	int xCurDispatch = this->move[0].curDispatch;


	int yRest = this->move[1].rest;
	int yMove = this->move[1].perFrame;
	int yDispatch = this->move[1].dispatch;
	int yCurDispatch = this->move[1].curDispatch;


	if (xDispatch && xCurDispatch < xDispatch) {
		this->move[0].curDispatch++;
		Log::inf(LOG_ANIM, "-- Dispatching X: %d / %d", this->move[0].curDispatch, this->move[0].dispatch);
	}
	else{
		Log::inf(LOG_ANIM, "-- Apply X Move");
		this->move[0].curDispatch = 0;
		if (xRest != 0) {
			if (xRest > 0) {
				xMove++;
				xRest--;
			}
			else{
				xMove--;
				xRest--;
			}

			this->move[0].rest = xRest;
		}
	}


	if (yDispatch && yCurDispatch < yDispatch) {
		this->move[1].curDispatch++;
		Log::dbg(LOG_ANIM, "-- Dispatching Y: %d / %d", this->move[1].curDispatch, this->move[1].dispatch);
	}
	else {
		if (yRest != 0) {
			Log::dbg(LOG_ANIM, "-- Apply Y Move");
			this->move[1].curDispatch = 0;
			if (yRest > 0) {
				yMove++;
				yRest--;
			}
			else{
				yMove--;
				yRest--;
			}

			this->move[1].rest = yRest;
		}
	}


	vector m;
	m.x = xMove;
	m.y = yMove;

	Object* obj = this->obj;
	Log::dbg(LOG_ANIM, "Moving Object: X: %d | Y: %d", xMove, yMove);

	vector pos = obj->getPosition();
	Log::dbg(LOG_ANIM, "Current Position: X: %lf | Y: %lf", pos.x, pos.y);
	
	pos = obj->move(m);
	Log::dbg(LOG_ANIM, "New Position: X: %lf | Y: %lf", pos.x, pos.y);


	if (this->breakOnReach) {
		Log::dbg(LOG_ANIM, "-- Breaking Anim");
		bool xDone = !this->move[0].rest && !this->move[0].perFrame;
		bool yDone = !this->move[1].rest && !this->move[1].perFrame;

		if (xDone && yDone) {
			Log::dbg(LOG_ANIM, "ANIMATION DONE");
			this->breakAnim = true;
			Log::dbg(LOG_ANIM, "Frames Left: %d", this->frames);
		}
	}

	if (breakAnim) {
		this->breakAnim = true;
	}

	Log::inf(LOG_ANIM, "==== ANIM Move DONE ====");
}

void Animator::removeMove(Object* obj) {
	if (obj == NULL) {
		return;
	}

	Log::inf(LOG_ANIM, "Removing Anim For Obj: %s", obj->getName());

	Node* n = NULL;
	while ((n = listIterate(this->moves, n)) != NULL) {
		Animation* anim = (Animation*) n->value;

		if (anim->obj == obj) {
			Log::inf(LOG_ANIM, "Remove Move Anim: %s | %p", anim->getName(), anim);
			Animation* anim = (Animation*) n->value;
			if (anim != NULL) {
				anim->breakAnim = true;
			}
		}
	}

//	deleteNodeByValue(this->sprites, obj);
}

void Animator::removeSprite(Object* obj) {
	if (obj == NULL) {
		return;
	}

	Node* n = NULL;
	while ((n = listIterate(this->sprites, n)) != NULL) {
		Animation* anim = (Animation*) n->value;

		if (anim->obj == obj) {
			removeAndFreeNode(this->sprites, n);
		}
	}

//	deleteNodeByValue(this->sprites, obj);
}


void Animator::addSprite(Animation* animParam){
	Node* n = addNodeV(this->sprites, animParam->obj->getName(), animParam, 0);
	n->del = deleteAnim;
}

void Animator::animateSprite() {
	Node* n = NULL;
	//Log::war(LOG_ANIM, "Launch Sprite");
	while ((n = listIterate(this->sprites, n)) != NULL) {
		Animation* anim = (Animation*) n->value;
		Object* obj = (Object*) anim->obj;

		bool b = obj->lock("ANIMMATE-SPRITE-0");

		//Log::war(LOG_ANIM, "Call Sprite Fnc");
		anim->fnc();
		//Log::err(LOG_ANIM, "END Sprite Fnc");

		if (anim->breakAnim || (!anim->loop && anim->done)) {
			if (anim->deleteOnDone) {
				Node* tmp = n->prev;
				removeAndFreeNode(this->sprites, n);
				n = tmp;
			}
		}

		obj->unlock("ANIMMATE-SPRITE-1", b);
	}
	//Log::err(LOG_ANIM, "END Launch Sprite");
}