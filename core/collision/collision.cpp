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
	this->overlaps = initListMgr();


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

	deleteList(this->overlaps);

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


bool Collision::widthCollides(const Collision* col) const {
	return this->widthCollides(col, {0,0});
}

bool Collision::widthCollides(const Collision* col, vector move) const{
	IntRect pos = this->getWorldPosition();
	IntRect pos2 = col->getWorldPosition();

	pos.left += move.x;
	pos.top += move.y;

	return
		((pos.left <= pos2.left) && (pos.left + pos.width >= pos2.left))
		||
		((pos2.left <= pos.left) && (pos2.left + pos2.width >= pos.left))
    ;
}

bool Collision::heightCollides(const Collision* col) const {
	return this->heightCollides(col, {0,0});
}

bool Collision::heightCollides(const Collision* col, vector move) const{
	IntRect pos = this->getWorldPosition();
	IntRect pos2 = col->getWorldPosition();

	pos.left += move.x;
	pos.top += move.y;

	return
		((pos.top <= pos2.top) && (pos.top + pos.height >= pos2.top))
		||
		((pos2.top <= pos.top) && (pos2.top + pos2.height >= pos.top))
    ;
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

	if (!flags) {
		this->updateColObj(COL_IGNORE);
	}


	bool width = this->widthCollides(col);
	bool height = this->heightCollides(col);

    bool position = width && height;
    bool collides = flags && position;

    if (collides) {
    	if (hit) {
    		res = COL_BLOCK;
    	}
    	else if (overlap) {
    		res = COL_OVERLAP;
    	}
    }

    this->updateColObj(res);

	return res;
}

void Collision::updateColObj(ColType type) const {
    if (this->colObj == NULL) {
    	return;
    }

    IntRect* clip = new IntRect(0, 0, 50, 50);
    if (type != COL_IGNORE) {
    	clip->left = 50;
    }
    else{
    	clip->left = 0;
    }

    this->colObj->setClip(clip, true);
}

bool Collision::callHit(Collision* col2) {
	// Log::war(LOG_COL, "=== Call Hit: '%s' ===", this->name);
	bool b = true;

	if (!this->obj->isEnabled() || !col2->getObject()->isEnabled()) {
		return false;
	}

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
	if (!this->obj->isEnabled() || !col2->getObject()->isEnabled()) {
		return false;
	}
	
	if (this->onOverlap != NULL /*&& this->addOverlap(col2)*/) {
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

	res.top = (this->pos.top + this->obj->getPosition().y) - obj->getOrigin().y;
	res.left = (this->pos.left + this->obj->getPosition().x) - obj->getOrigin().x;

	return res;
}


bool Collision::isEnabled() {
	return this->enabled;
}

void Collision::toggle(bool b) {
	this->enabled = b;

	if (this->colObj != NULL) {
		this->colObj->toggle(b);
	}
}

bool Collision::isLeft(Collision* col2, vector move) {
	IntRect pos = this->getWorldPosition();
	IntRect pos2 = col2->getWorldPosition();
	
	pos.left += move.x;
	pos.top += move.y;

	return pos.left < (pos2.left + (pos2.width / 2));
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

void Collision::setPos(IntRect pos) {
	this->pos.left = pos.left;
	this->pos.top = pos.top;
	this->pos.width = pos.width;
	this->pos.height = pos.height;

	if (this->colObj == NULL) {
		return;
	}

	vector posV;
	posV.x = pos.left;
	posV.y = pos.top;

	this->colObj->setPosition(posV);

	vector scale;
	scale.x = (float) (pos.width / 50.0f);
	scale.y = (float) (pos.height / 50.0f);
	this->colObj->setScale(scale);
}

bool Collision::addOverlap(Collision* col) {
	Node* n = addNodeUniqValue(this->overlaps, col->getName(), col, false);
	if (n != NULL) {
		Log::err(LOG_COL, "ADD OVERLAP: LIST: %p | COL: %p", this->overlaps, col);
	}
	return n != NULL;
}

vector Collision::getHitMove(Collision* col2) {
	return this->getHitMove(col2, {0,0});
}

vector Collision::getHitMove(Collision* col2, vector move) {
	IntRect pos = this->getWorldPosition();
	IntRect pos2 = col2->getWorldPosition();


	Log::err(LOG_COL, "Move: %lf | %lf", move.x, move.y);
	Log::err(LOG_COL, "POS: X: %d | Y: %d | W: %d | H: %d", pos.left, pos.top, pos.width, pos.height);
	Log::err(LOG_COL, "POS2: X: %d | Y: %d | W: %d | H: %d", pos2.left, pos2.top, pos2.width, pos2.height);

	pos.left += move.x;
	pos.top += move.y;

	int up = abs((pos.top + pos.height) - pos2.top );	

	int down = abs(pos.top - (pos2.top + pos2.height));	

	int left = abs((pos.left + pos.width) - pos2.left);	
	int right = abs(pos.left - (pos2.left + pos2.width));

	Log::err(LOG_COL, "Up: %d | Down: %d | Left: %d | Right: %d", up, down, left, right);


	vector res = {0,0};
	vector test = {0,0};

	if (this->heightCollides(col2, move)) {
		if (up <= down) {
			res.y = (pos2.top - pos.height) -1;
		}
		else{
			res.y = (pos2.top + pos2.height) + 1;
		}

		Log::err(LOG_COL, "Top Target: %lf", res.y);
		
		test.x = 0;
		test.y = pos.top - res.y;
		if (!this->collides(col2, test)) {
			res.y = res.y - (pos.top);
			Log::err(LOG_COL, "1-Change Top: %lf", res.y);
			return res;
		}
	}
	else {
		res.y = move.y;
	}

	if (this->widthCollides(col2, move)) {
		if (left <= right) {
			res.x = (pos2.left - pos.width) -1;
		}
		else{
			res.x = (pos2.left + pos2.width) + 1;
		}

		test.y = 0;
		test.x = res.x - (pos.left - move.x);
		if (!this->collides(col2, test)) {
			
			Log::err(LOG_COL, "Top Target: %lf", res.y);
			if (res.y < res.x) {
				res.x = 0;
				res.y = res.y - (pos.top);
				Log::err(LOG_COL, "2-Change Top: %lf", res.y);
				return res;
			}
			else{
				res.y = 0;
				res.x = res.x - (pos.left);

				Log::err(LOG_COL, "Change Left: %lf", res.x);
				return res;
			}
		}
	}
	else {
		res.x = move.x;
	}

	return res;
}