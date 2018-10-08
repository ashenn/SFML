#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "../../base/math.h"
#include "../abstractClass.h"

class Object;

#define DIRECTION_TYPES(TYPE) \
	TYPE(DIR_NULL) \
	TYPE(DIR_UP) \
	TYPE(DIR_RIGHT) \
	TYPE(DIR_DOWN) \
	TYPE(DIR_LEFT)

#define GEN_DIRECTION_TYPE_ENUM(ENUM) ENUM,
#define GEN_DIRECTION_TYPE_STRING(STRING) #STRING,

typedef enum DirectionEnum {
	DIRECTION_TYPES(GEN_DIRECTION_TYPE_ENUM)
} DirectionEnum;


typedef struct Direction
{
	DirectionEnum x;
	DirectionEnum y;
} Direction;


class Movement
{
	private:
		Object* obj;
		Direction dir;
		vector velocity;

		void _setVelocityY(double y);
		void _setVelocityX(double y);

		void velocityUpdated();
		void applyVelocity();

	public:
		Movement(Object* obj);
		~Movement();

		vector getVelocity();
		Direction getDirection();
		void setVelocityY(double y);
		void setVelocityX(double y);
		void setVelocity(vector vel);

		void addVelocity(vector vel);
		void refreshVelocity();
};

#include "../object/object.h"

#endif