#include "collision.h"
#include "collisionMgr.h"

static const char* COL_TYPE_STRINGS[] = {
    COL_TYPES(GEN_COL_TYPE_STRING)
};

const char* colTypeName(ColType v) {
	return COL_TYPE_STRINGS[v];
}

ColType colTypeValue(const char* name) {
	for (int i = 0; COL_TYPE_STRINGS[i]; ++i) {
		const char* iName = COL_TYPE_STRINGS[i];

		if (!strcmp(name, iName)) {
			return (ColType) i;
		}
	}

	return COL_IGNORE;
}

static const char* COL_CHANEL_STRINGS[] = {
    COL_CHANELS(GEN_COL_CHANEL_STRING)
};

const char* colChanelName(ColChanel v) {
	return COL_CHANEL_STRINGS[v];
}

ColChanel colChanelValue(const char* name) {
	for (int i = 0; COL_CHANEL_STRINGS[i]; ++i) {
		const char* iName = COL_CHANEL_STRINGS[i];

		if (!strcmp(name, iName)) {
			return (ColChanel) i;
		}
	}

	return COL_NONE;
}


Collision::Collision(const char* name, Object* obj, IntRect pos) {
	Log::inf(LOG_COL, "=== Adding Collision '%s' To '%s'", name, obj->getName());

	const char* objN = obj->getName();

	int len = strlen(name) + strlen(objN) + 5;
	this->name = StrE(len);
	snprintf(this->name, len, "%s_%s_col", name, objN);


	Vector2f origin = obj->getOrigin();
	pos.left -= origin.x;
	pos.top -= origin.y;

	this->obj = obj;
	this->pos = pos;
	this->flag = COL_NONE;

	CollisionMgr::get()->addObject(obj);
}

Collision::Collision(const char* name, Object* obj, IntRect pos, ColChanel chanel) : Collision(name, obj, pos) {
	Log::inf(LOG_COL, "==== INIT COL ===");
	Log::dbg(LOG_COL, "Channel: %s", colChanelName(chanel));

	this->flag = 1 << (int) chanel;
	Log::dbg(LOG_COL, "Channel: %u", this->flag);

	ListManager* channels = CollisionMgr::get()->getChannels();

	Node* chanN = NULL;
	while ((chanN = listIterate(channels, chanN)) != NULL) {
		ListManager* chanConf = (ListManager*) chanN->value;

		if (colChanelValue(chanN->name) != chanel) {
			continue;
		}

		Log::inf(LOG_COL, "%s", chanN->name);

		Node* valN = NULL;
		while ((valN = listIterate(chanConf, valN)) != NULL) {
			ColType* type = (ColType*) valN->value;

			/*Log::dbg(LOG_COL, "%s : %d", valN->name, (int) *type);*/
			//Log::dbg(LOG_COL, "Type : %s", colTypeName(*type));

			if (*type == COL_BLOCK) {
                /*Log::dbg(LOG_COL, "Add Flag Hit: %s", valN->name);
                Log::dbg(LOG_COL, "Add Flag Hit: %d", (int) colChanelValue(valN->name));
                Log::dbg(LOG_COL, "Add Flag Hit: %u", (1 << (int) colChanelValue(valN->name)));*/

				this->hitFlags = this->hitFlags | (1 << (int) colChanelValue(valN->name));
			}
			else if (*type == COL_OVERLAP) {
				this->overlapFlags = this->overlapFlags | (1 << (int) colChanelValue(valN->name));
			}
		}
	}

	Log::dbg(LOG_COL, "Hit: %u", this->hitFlags);
	Log::dbg(LOG_COL, "Overlap: %u", this->overlapFlags);
}


Collision::~Collision() {
	Log::inf(LOG_COL, "=== Deleting Collision: %s ===", this->name);

	if (this->onHit != NULL) {
		Log::dbg(LOG_COL, "-- delete on Hit");
		delete this->onHit;
	}

	if (this->onOverlap != NULL) {
		Log::dbg(LOG_COL, "-- delete on Overlap");
		delete this->onOverlap;
	}
}

const unsigned int Collision::getFlag() const {
	return this->flag;
}

ColType Collision::collides(Collision* col, vector move) {
	this->pos.top += move.y;
	this->pos.left += move.x;

	ColType res = this->collides(col);

	this->pos.top -= move.y;
	this->pos.left -= move.x;

	return res;
}


ColType Collision::collides(const Collision* col) const {
	ColType res = COL_IGNORE;
	IntRect pos = this->getWorldPosition();
	IntRect pos2 = col->getWorldPosition();

	Log::inf(LOG_COL, "POS: X: %d | Y: %d | W: %d | H: %d", pos.left, pos.top, pos.width, pos.height);
	Log::inf(LOG_COL, "POS2: X: %d | Y: %d | W: %d | H: %d", pos2.left, pos2.top, pos2.width, pos2.height);


	// Log::war(LOG_COL, "Col Flag: %d", col->getFlag());

	// Log::war(LOG_COL, "Col Hit: %d", this->hitFlags);
	// Log::war(LOG_COL, "Col Overlap: %d", this->overlapFlags);

	// Log::war(LOG_COL, "Blocks: %d", this->hitFlags & col->getFlag());
	// Log::war(LOG_COL, "Overlaps: %d", this->overlapFlags & col->getFlag());

	bool hit = this->hitFlags & col->getFlag();
	bool overlap = this->overlapFlags & col->getFlag();

	bool flags = hit || overlap;

	bool width1 =
		(pos2.left + pos2.width >= pos.left)
		&&
		(pos2.left + pos2.width <= (pos.left + pos.width))
    ;

	bool width2 =
		(pos.left + pos.width >= pos2.left)
		&&
		(pos.left + pos.width <= pos2.left + pos2.width)
    ;

    bool height1 =
    	(pos2.top + pos2.height >= pos.top)
    	&&
		(pos2.top + pos2.height <= pos.top + pos.height)
    ;

    bool height2 =
    	(pos.top + pos.height >= pos2.top)
    	&&
		(pos.top + pos.height <= pos2.top + pos2.height)
    ;

    bool position = (width1 || width2) && (height1 || height2);
    bool collides = flags && position;

    if (position && ! collides) {
    	//Log::war(LOG_COL, "BAD Flags !!!");
    }

    if (collides) {
    	if (hit) {
    		res = COL_BLOCK;
    	}
    	else if (overlap) {
    		res = COL_OVERLAP;
    	}
    }

    if (this->colObj == NULL) {
    	return res;
    }

    IntRect* clip = new IntRect(0, 0, 50, 50);
    if (collides) {
    	clip->left = 50;
    }
    else{
    	clip->left = 0;
    }

    this->colObj->setClip(clip, true);

	return res;
}

bool Collision::callHit(Collision* col2) {
	// Log::war(LOG_COL, "=== Call Hit: '%s' ===", this->name);
	bool b = true;
	if (this->onHit != NULL) {
		b = this->onHit->call(this, col2);
	}

	// if (b) {
	// 	Log::inf(LOG_COL, "=== Call STOP: '%s' ===", this->name);
	// 	if (col2->getPosition().top > this->pos.top) {
	// 		Log::inf(LOG_COL, "-- STOP FALL");
	// 		this->obj->getMovement()->setVelocityY(0);
	// 	}
	// 	else{
	// 		Log::inf(LOG_COL, "-- STOP MOVE");
	// 		this->obj->getMovement()->setVelocityX(0);
	// 	}
	// }

	return b;
}

bool Collision::callOverlap(Collision* col2) {
	Log::inf(LOG_COL, "=== Call Overlap: '%s' ===", this->name);
	if (this->onOverlap != NULL) {
		return this->onOverlap->call(this, col2);
	}

	return true;
}

bool Collision::operator&&(const Collision* col) const {
	return this->collides(col) != COL_IGNORE;
}

bool Collision::operator&&(const Collision& col) const {
	return this->collides(&col) != COL_IGNORE;
}


IntRect Collision::getPosition() const {
	return this->pos;
}

IntRect Collision::getWorldPosition() const {
	IntRect res;
	res.width = this->pos.width;
	res.height = this->pos.height;

	res.top = this->pos.top + this->obj->getPosition().y;
	res.left = this->pos.left + this->obj->getPosition().x;

	return res;
}


bool Collision::isEnabled() {
	return this->enabled;
}

void Collision::toggle(bool b) {
	this->enabled = b;
}

bool Collision::isLeft(Collision* col2, vector move) {
	IntRect pos = this->getWorldPosition();
	IntRect pos2 = col2->getWorldPosition();
	
	pos.left += move.x;
	pos.top += move.y;

	return (pos2.left >= pos.left) && (pos2.left <= pos.left + pos.width);
}

bool Collision::isLeft(Collision* col2) {
	return this->isLeft(col2, {0,0});
}

bool Collision::isRight(Collision* col2, vector move) {
	return !this->isLeft(col2, move);
}

bool Collision::isRight(Collision* col2) {
	return this->isRight(col2, {0,0});
}


bool Collision::isUnder(Collision* col2, vector move) {
	IntRect pos = this->getWorldPosition();
	IntRect pos2 = col2->getWorldPosition();

	pos.left += move.x;
	pos.top += move.y;
	
	return (pos.top >= pos2.top) && (pos.top <= pos2.top + pos2.height);
}

bool Collision::isUnder(Collision* col2) {
	return this->isUnder(col2, {0, 0});
}

bool Collision::isOver(Collision* col2, vector move) {
	return !this->isUnder(col2, move);
}

bool Collision::isOver(Collision* col2) {
	return this->isOver(col2, {0,0});
}

Object* Collision::getObject()  const {
	return this->obj;
}