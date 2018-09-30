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


Animation::Animation(Object* obj, float time, float delay, void (Animator::*fnc)(Animation*)) {
	this->obj = obj;
	this->fnc = fnc;

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


MoveAnim::MoveAnim(Object* obj, float time, float delay, void (Animator::*fnc)(Animation*)) : Animation(obj, time, delay, fnc) {}
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

	MoveAnim* anim = new MoveAnim(obj, time, delay, &Animator::animMoveTo);

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


void Animator::animMoveTo(Animation* param) {

}


void Animator::addMoveObject(Object* obj, Animation* param) {
	Log::inf(LOG_ANIM, "==== Adding Anim Object: %s ====", obj->getName());

	Log::dbg(LOG_ANIM, "FINDING NODE");
	Node* objNode = getNodeByName(this->moves, obj->getName());

	if (objNode == NULL) {
		Log::dbg(LOG_ANIM, "-- Animation: Adding New Object: %s", obj->getName());
		objNode = addNodeV(this->moves, obj->getName(), initListMgr(), 0);

		Log::dbg(LOG_ANIM, "-- nodeID: %d", objNode->id);
	}
	else{
		Log::dbg(LOG_ANIM, "NODE FOUND !!!!");
	}

	Node* paramNode = addNodeV((ListManager*) objNode->value, obj->getName(), param, 1);

	param->id = paramNode->id;

	Log::dbg(LOG_ANIM, "==== Animation: Added %s #%d ====", param->obj->getName(), paramNode->id);
}