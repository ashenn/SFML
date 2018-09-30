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
		animDist.dispatch = frames / rest;
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
	this->obj = obj;

	this->stepFnc = NULL;
	this->callback = NULL;

	this->done = false;
	this->loop = false;
	this->breakAnim = false;
	this->deleteOnDone = false;
	this->deleteObject = false;

	this->delay = delay * FPS;
	this->initialFrames = this->frames = time * FPS;
}

Animation::~Animation() {}

void Animation::setId(unsigned int id) {
	this->id = id;

	char name[150];
	memset(name, 0, 150);
	snprintf(name, 150, "Move Anim #%d => %s", this->id, obj->getName());

	this->name = Str(name);
}


MoveAnim::MoveAnim(Object* obj, float time, float delay) : Animation(obj, time, delay) {}

MoveAnim::~MoveAnim() {}

Animation* Animator::moveTo(Object* obj, int x, int y, float time, float delay) {
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
    return (Animation*) anim;
}


void Animator::animMoveTo(Animation* anim) {
	Log::inf(LOG_ANIM, "CALL ANIM MOVE TO");
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

	deleteNodeByName(this->moves, obj->getName());
	
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


		Log::inf(LOG_ANIM, "ANIMATING: %s", anim->obj->getName());
		if (anim->delay > 0) {
			Log::dbg(LOG_ANIM, "-- delayed: %d", anim->delay);
			anim->delay--;
			break;
		}


		Object* obj = anim->obj;
		Log::dbg(LOG_ANIM, "-- Calling Anim Function: %s", anim->obj->getName());

		bool b = obj->lock("ANIMMATE-0");

		anim->fnc();

		obj->unlock("ANIMMATE-1", b);


		anim->frames--;
		Log::dbg(LOG_ANIM, "-- Frames Left: %d", anim->frames);

		if (anim->frames > 0 && !anim->breakAnim) {
			break;
		}


		Log::dbg(LOG_ANIM, "-- Animation Ended");

		if (anim->callback != NULL) {
			Log::inf(LOG_ANIM, "-- Calling CallBack: %p", anim);

			bool b = obj->lock("ANIMMATE-4");

			anim->callback(anim);

			obj->unlock("ANIMMATE-5", b);
		}


		if (anim->loop) {
			Log::inf(LOG_ANIM, "-- Looping Animation");
			anim->frames = anim->initialFrames;
			break;
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
		Log::inf(LOG_ANIM, "Objects To Animate: #%d", this->moves->nodeCount);
		this->animateMove();
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
		Log::dbg(LOG_ANIM, "-- Dispatching X: %d / %d", this->move[0].curDispatch, this->move[0].dispatch);
	}
	else{
		Log::dbg(LOG_ANIM, "-- Apply X Move");
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
	Log::dbg(LOG_ANIM, "Current Position: X: %d | Y: %d", pos.x, pos.y);
	
	pos = obj->move(m);
	Log::dbg(LOG_ANIM, "Nox Position: X: %d | Y: %d", pos.x, pos.y);


	if (this->breakOnReach) {
		Log::dbg(LOG_ANIM, "-- Breaking Anim");
		bool xDone = !this->move[0].rest && !this->move[0].perFrame;
		bool yDone = !this->move[1].rest && !this->move[1].perFrame;

		if (xDone && yDone) {
			Log::inf(LOG_ANIM, "ANIMATION DONE");
			this->breakAnim = true;
			Log::inf(LOG_ANIM, "Frames Left: %d", this->frames);
		}
	}

	if (breakAnim) {
		this->breakAnim = true;
	}

	Log::inf(LOG_ANIM, "==== ANIM Move DONE ====");
}