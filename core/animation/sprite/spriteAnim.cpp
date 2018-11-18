#include "spriteAnim.h"

SpriteAnimData::SpriteAnimData(const char* name) {
	this->name = Str(name);
	this->animLinks = initListMgr();
}

SpriteAnimData::~SpriteAnimData() {
	Log::inf(LOG_SPRITE_ANIM, "=== Deleting Sprite Anim: %s ===", this->name);

	if (this->clipPos != NULL) {
		Log::dbg(LOG_SPRITE_ANIM, "-- deleting Clip Pos");
		
		this->obj->removeClip(false);
		for(int i=0; this->clipPos[i] != NULL; i++) {
			delete this->clipPos[i];
		}
		
		free(this->clipPos);
		this->clipPos = NULL;
	}

	if (this->animLinks != NULL) {
		Log::dbg(LOG_SPRITE_ANIM, "-- deleting Anim Links");
		deleteList(this->animLinks);
		Log::dbg(LOG_SPRITE_ANIM, "-- Anim Links DELETED");
		this->animLinks = NULL;
	}

	if (this->collisions != NULL) {
		deleteList(this->collisions);
	}
}
// void SpriteAnim::addAnimLinkFnc(const char* name, bool (*fnc)(SpriteAnim*)) {
// 	AnimLinkFnc* animFnc = (AnimLinkFnc*) malloc(sizeof(AnimLinkFnc));
// 	animFnc->fnc = fnc;

// 	addNodeV(animLinkFncs, name, animFnc, true);
// }

// AnimLinkFnc* SpriteAnim::getAnimLinkFnc(const char* name) {
// 	Node* n = getNodeByName(animLinkFncs, name);

// 	if (n == NULL) {
// 		return NULL;
// 	}

// 	return (AnimLinkFnc*) n->value;
// }

// void SpriteAnim::clearAnimLinkFnc() {
// 	deleteList(animLinkFncs);
// }


SpriteAnim* SpriteAnim::animate(SpriteObj* obj, const char* name, unsigned int clipIndex) {
	Log::inf(LOG_SPRITE_ANIM, "-- Searching Anim: %s For Object: %s", name, obj->getName());
	Node* n = getNodeByName(obj->getAnimList(), name);
	
	if (n == NULL) {
		Log::war(LOG_SPRITE_ANIM, "Fail To Find Animation %s for Object %s !!!", name, obj->getName());
		return NULL;
	}

	Log::inf(LOG_SPRITE_ANIM, "-- Node Found: %s", n->name);

	if (n->value == NULL) {
		Log::war(LOG_SPRITE_ANIM, "Animation with ID: %s is Not Initialized for Object %s !!!", name, obj->getName());
		return NULL;
	}

	SpriteAnimData* anim = (SpriteAnimData*) n->value;

	Log::inf(LOG_SPRITE_ANIM, "-- Casted To Anim: #%d => %s", anim->animID, anim->getName());

	return callAnim(obj, anim, clipIndex);
}

SpriteAnim* SpriteAnim::callAnim(SpriteObj* obj, SpriteAnimData* anim, unsigned int clipIndex) {
	Log::inf(LOG_SPRITE_ANIM, "-- Calling Sprite Animation: %s #%d => %s", obj->getName(), anim->animID, anim->getName());
	Log::inf(LOG_SPRITE_ANIM, "-- Removing Old Sprite Anim");

	// Animator* animator = Animator::get();
	// animator->removeSprite((Object*) obj);
	
	Log::inf(LOG_SPRITE_ANIM, "-- Calling Sprite Animation: %s #%d => %s", obj->getName(), anim->animID, anim->getName());

	unsigned int animID = anim->animID;
	if (clipIndex >= anim->clipCnt) {
		Log::dbg(LOG_SPRITE_ANIM, "Undefinded Clip Index #%d / %d for Object %s Animation: #%d !!!", clipIndex, anim->clipCnt, obj->getName(), animID);
		Log::dbg(LOG_SPRITE_ANIM, "-- Clip Index has been set to 0", clipIndex, anim->clipCnt, obj->getName(), animID);
		clipIndex = 0;
	}

	Log::dbg(LOG_SPRITE_ANIM, "-- New Anim Param");
	SpriteAnim* curParam = obj->getCurrentAnim();
	SpriteAnim* animParam = new SpriteAnim(obj);


	if (curParam != NULL) {
		animParam->wait = curParam->anim->wait;
	}
	else {
		animParam->wait = (FPS / 10);
	}

	// animParam->wait = anim->wait;
	animParam->anim = anim;
	animParam->done = false;
	animParam->animID = animID;
	animParam->loop = anim->loop;
	animParam->breakAnim = false;
	animParam->clipIndex = clipIndex;
	animParam->clipMax = anim->clipCnt;

	animParam->clip = *(anim->clipPos[clipIndex]);
	animParam->setCollisions();

	Log::dbg(LOG_SPRITE_ANIM, "-- Adding Anim To Animator: %s", anim->getName());
	Log::dbg(LOG_SPRITE_ANIM, "-- Loop: %d", animParam->loop);

	// animator->addSprite(animParam);

	return animParam;
}

SpriteAnim::SpriteAnim(Object* obj) : Animation(obj) {
	//this->animLinkFncs = initListMgr();
}

SpriteAnim::~SpriteAnim() {
//	deleteList(this->animLinkFncs);
}

void SpriteAnim::fnc() {
	if (this->clipIndex != this->clipMax) {
		this->done = false;
	}

	if (this->wait-- > 0) {
		Log::dbg(LOG_SPRITE_ANIM, "-- Sprite Wait: %d | %s", wait, this->getName());
		return;
	}

	Log::inf(LOG_SPRITE_ANIM, "-- Sprite Launch: %s", this->getName());
	this->wait = this->anim->wait;

	if (++this->clipIndex >= this->clipMax) {
		Log::dbg(LOG_SPRITE_ANIM, "-- Max Clip Reached: %s => %d", this->anim->getName(), this->clipMax);
		this->done = true;

		if (this->loop) {
			this->clipIndex = 0;
		}
		else {
			this->clipIndex = this->clipMax-1;
		}
	}

	Log::dbg(LOG_SPRITE_ANIM, "-- Clip Index: %d / %d", this->clipIndex, this->clipMax);
	this->obj->setClip(this->anim->clipPos[this->clipIndex], false);
	
	IntRect* c = this->anim->clipPos[this->clipIndex];
	Log::dbg(LOG_SPRITE_ANIM, "-- Clip: X: %d | Y: %d | W: %d | H: %d", c->left, c->top, c->width, c->height);
	this->updateSprite();
}

void SpriteAnim::updateSprite() {
	if (this->anim->animLinks == NULL || !this->anim->animLinks->nodeCount) {

		if (this->done) {
			Log::war(LOG_SPRITE_ANIM, "Animation: '%s' Has No Links To An Other Animation.", this->anim->getName());
		}

		return;
	}

	Node* n = NULL;
	while ((n = listIterate(this->anim->animLinks, n)) != NULL) {
		AnimLink* link = (AnimLink*) n->value;
		// Log::war(LOG_SPRITE_ANIM, "Test Link Anim: %s", link->name);

		if (link->waitEnd && !this->done) {
			// Log::war(LOG_SPRITE_ANIM, "Skip: %s", link->name);
			continue;
		}
		
		SpriteObj* sprite = (SpriteObj*) this->anim->obj;
		if(link->name == NULL) {
			Log::inf(LOG_SPRITE_ANIM, "Force Calling Anim: %s", link->target);
			sprite->animate(link->target, 0);
			break;
		}
		else if(this->callAnimLinkFnc(sprite, link)){
			Log::inf(LOG_SPRITE_ANIM, "Calling Anim: %s", link->target);
			Log::inf(LOG_SPRITE_ANIM, "From Link: %s", link->name);

			this->triggerCallBack(this->obj);
			sprite->animate(link->target, 0);
			break;
		}
	}
}


AnimColData::AnimColData(Json* colJ) {
	this->name = Str(colJ->key);


	Json* enJ = jsonGetData(colJ, "enabled");
	
	if (enJ != NULL) {
		this->enabled = (bool*) malloc(sizeof(bool));
		*(this->enabled) = enJ->boolean;
	}

	Json* posJ = jsonGetData(colJ, "pos");
	if (posJ != NULL) {
		this->pos = new IntRect(0,0,0,0);

		jsonGetValue(posJ, "top", &this->pos->top);
		jsonGetValue(posJ, "left", &this->pos->left);
		jsonGetValue(posJ, "width", &this->pos->width);
		jsonGetValue(posJ, "height", &this->pos->height);
	}
}

AnimColData::~AnimColData() {
	if (this->enabled != NULL) {
		free(this->enabled);
	}

	if (this->pos != NULL) {
		delete this->pos;
	}
}

void SpriteAnim::setCollisions() {
	if (this->anim->collisions == NULL) {
		return;
	}

	Log::inf(LOG_SPRITE_ANIM, "==== Changing Collisions For Anim: %s ====", this->getName());

	Node* n = NULL;
	while ((n = listIterate(this->anim->collisions, n)) != NULL) {
		AnimColData* colData = (AnimColData*) n->value;
		Log::inf(LOG_SPRITE_ANIM, "++++ Anim Col: %s ====", colData->getName());

		Collision* col = this->obj->getCollision(colData->getName());
		if (col == NULL) {
			Log::inf(LOG_SPRITE_ANIM, "Fail To Find Collision '%s' For Animation '%s'", colData->getName(), this->anim->getName());
			continue;
		}

		if (colData->enabled != NULL) {
			Log::inf(LOG_SPRITE_ANIM, "Updating Enabled: '%d'", *(colData->enabled));
			col->toggle(*colData->enabled);
		}

		if (colData->pos != NULL) {
			Log::inf(LOG_SPRITE_ANIM, "Updating Pos: %d | %d | %d | %d", colData->pos->left, colData->pos->top, colData->pos->width, colData->pos->height);
			col->setPos(*(colData->pos));
		}
	}
}