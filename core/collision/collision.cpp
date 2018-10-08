#include "collision.h"

Collision::Collision(Object* obj, IntRect pos) {
	this->obj = obj;
	this->pos = pos;
}

Collision::~Collision() {}

bool Collision::collides(const Collision* col) const {
	IntRect pos = this->getWordlPosition();
	IntRect pos2 = col->getWordlPosition();

	Log::war(LOG_MAIN, "POS: X: %d | Y: %d | W: %d | H: %d", pos.left, pos.top, pos.width, pos.height);
	Log::war(LOG_MAIN, "POS2: X: %d | Y: %d | W: %d | H: %d", pos2.left, pos2.top, pos2.width, pos2.height);

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

    bool collides = (width1 || width2) && (height1 || height2);
    return collides;
}

bool Collision::operator&&(const Collision* col) const {
	return this->collides(col);
}

bool Collision::operator&&(const Collision& col) const {
	return this->collides(&col);
}


IntRect Collision::getPosition() const {
	return this->pos;
}

IntRect Collision::getWordlPosition() const {
	IntRect res;
	res.width = this->pos.width;
	res.height = this->pos.height;

	res.left = this->pos.left + this->obj->getPosition().x;
	res.top = this->pos.top + this->obj->getPosition().y;

	return res;
}
