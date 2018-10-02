#include "spriteAnim.h"

SpriteAnimData::SpriteAnimData(const char* name) {
	this->name = Str(name);
	this->animLinks = initListMgr();
}

SpriteAnimData::~SpriteAnimData() {
	Log::inf(LOG_SPRITE_ANIM, "=== Deleting Sprite Anim: %s ===", this->name);

	if (this->clipPos != NULL) {
		Log::inf(LOG_SPRITE_ANIM, "-- deleting Clip Pos");
		
		this->obj->setClip(NULL, false);
		for(int i=0; this->clipPos[i] != NULL; i++) {
			delete this->clipPos[i];
		}
		
		free(this->clipPos);
		this->clipPos = NULL;
	}

	if (this->animLinks != NULL) {
		Log::inf(LOG_SPRITE_ANIM, "-- deleting Anim Links");
		deleteList(this->animLinks);
		Log::inf(LOG_SPRITE_ANIM, "-- Anim Links DELETED");
		this->animLinks = NULL;
	}
}


ListManager* SpriteAnim::animLinkFncs = initListMgr();

void SpriteAnim::addAnimLinkFnc(const char* name, bool (*fnc)(SpriteAnim*)) {
	AnimLinkFnc* animFnc = (AnimLinkFnc*) malloc(sizeof(AnimLinkFnc));
	animFnc->fnc = fnc;

	addNodeV(animLinkFncs, name, animFnc, true);
}

AnimLinkFnc* SpriteAnim::getAnimLinkFnc(const char* name) {
	Node* n = getNodeByName(animLinkFncs, name);

	if (n == NULL) {
		return NULL;
	}

	return (AnimLinkFnc*) n->value;
}

void SpriteAnim::clearAnimLinkFnc() {
	deleteList(animLinkFncs);
}


SpriteAnim* SpriteAnim::animate(SpriteObj* obj, const char* name, unsigned int clipIndex) {
	Log::inf(LOG_SPRITE_ANIM, "-- Searching Anim: %s For Object: %s", name, obj->getName());
	Node* n = getNodeByName(obj->getAnimList(), name);
	
	if (n == NULL) {
		Log::war(LOG_SPRITE_ANIM, "Fail To Find Animation %s for Object %s !!!", name, obj->getName());
		return NULL;
	}

	Log::dbg(LOG_SPRITE_ANIM, "-- Node Found: %s", n->name);

	if (n->value == NULL) {
		Log::war(LOG_SPRITE_ANIM, "Animation with ID: %s is Not Initialized for Object %s !!!", name, obj->getName());
		return NULL;
	}

	SpriteAnimData* anim = (SpriteAnimData*) n->value;

	Log::dbg(LOG_SPRITE_ANIM, "-- Casted To Anim: #%d => %s", anim->animID, anim->getName());

	return callAnim(obj, anim, clipIndex);
}

SpriteAnim* SpriteAnim::callAnim(SpriteObj* obj, SpriteAnimData* anim, unsigned int clipIndex) {
	Log::inf(LOG_SPRITE_ANIM, "-- Calling Sprite Animation: %s #%d => %s", obj->getName(), anim->animID, anim->getName());
	Log::inf(LOG_SPRITE_ANIM, "-- Removing Old Sprite Anim");

	Animator* animator = Animator::get();
	animator->spriteRemoveObject((Object*) obj);
	
	Log::inf(LOG_SPRITE_ANIM, "-- Calling Sprite Animation: %s #%d => %s", obj->getName(), anim->animID, anim->getName());

	unsigned int animID = anim->animID;
	if (clipIndex >= anim->clipCnt) {
		Log::war(LOG_SPRITE_ANIM, "Undefinded Clip Index #%d / %d for Object %s Animation: #%d !!!", clipIndex, anim->clipCnt, obj->getName(), animID);
		Log::war(LOG_SPRITE_ANIM, "-- Clip Index has been set to 0", clipIndex, anim->clipCnt, obj->getName(), animID);
		clipIndex = 0;
	}

	Log::dbg(LOG_SPRITE_ANIM, "-- New Anim Param");
	SpriteAnim* animParam = new SpriteAnim(obj);


	animParam->wait = 1;
	animParam->anim = anim;
	animParam->done = false;
	animParam->animID = animID;
	animParam->loop = anim->loop;
	animParam->breakAnim = false;
	animParam->clipIndex = clipIndex;
	animParam->clipMax = anim->clipCnt;

	animParam->clip = *(anim->clipPos[clipIndex]);

	Log::dbg(LOG_SPRITE_ANIM, "-- Adding Anim To Animator: %s", anim->getName());
	Log::dbg(LOG_SPRITE_ANIM, "-- Loop: %d", animParam->loop);

	animator->addSprite(animParam);

	return animParam;
}

SpriteAnim::~SpriteAnim() {}

void SpriteAnim::fnc() {
	if (this->clipIndex != this->clipMax) {
		this->done = false;
	}

	this->wait--;
	if (this->wait > 0) {
		Log::inf(LOG_SPRITE_ANIM, "TEST WAITING: %d", this->wait);
		return;
	}

	this->wait = this->anim->wait;
	Log::inf(LOG_SPRITE_ANIM, "NEW WAITING: %d", this->anim->wait);
	if (++this->clipIndex >= this->clipMax) {
		Log::inf(LOG_SPRITE_ANIM, "-- Max Clip Reached: %s => %d", this->anim->getName(), this->clipMax);
		this->done = true;
		this->clipIndex = 0;
	}

	Log::war(LOG_SPRITE_ANIM, "-- Clip Index: %d", this->clipIndex, this->clipMax);
	this->obj->setClip(this->anim->clipPos[this->clipIndex], false);
	// this->obj->clip = &();
}