#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../character/character.h"
#include "../event/key/keyEvent.h"

typedef enum ControllerType
{
	CTRL_NULL,
	CTRL_AI,
	CTRL_PLAYER,
	CTRL_GOST
} ControllerType;


class Controller : public AbstractClass
{
	protected:
		CharObj* obj = NULL;
		Character* ch = NULL;
		ControllerType type;

	public:
		Controller(ControllerType type, const char* name);
		~Controller();

		virtual void setCharacter(Character* ch, bool deleteOld);
		CharObj* getObject();
		Character* getCharacter();

		void stopMove();
		void moveDir(DirectionEnum dir);

		virtual void update(){}

		virtual bool hit(Collision* col, Collision* col2) = 0;
		virtual bool hitWall(Collision* col, Collision* col2, IntRect pos, IntRect pos2) = 0;


		virtual bool overlap(Collision* col, Collision* col2) = 0;
		virtual void kill() = 0;
		//virtual void move() = 0;
};

#endif