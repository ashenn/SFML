#ifndef COLLISION_H
#define COLLISION_H

#include "../../common.h"
using namespace sf;

class Object;

typedef enum ColType {
	COL_IGNORE,
	COL_BLOCK,
	COL_OVERLAP,
} ColType;

typedef enum ColChanel {
	COL_NONE,
	COL_ALL,
	COL_ITEM,
	COL_PLAYER,
	COL_MONSTER,
} ColChanel;

class Collision
{
	Object* obj;
	IntRect pos;

	public:
		Collision(Object* obj, IntRect pos);
		~Collision();

		IntRect getPosition()  const;
		IntRect getWordlPosition()  const;

		bool collides(const Collision* col) const;
		bool operator&&(const Collision& col) const;
		bool operator&&(const Collision* col) const;
};

#include "../object/object.h"

#endif