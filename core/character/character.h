#ifndef CHARACTER_H
#define CHARACTER_H

#include "../../base/math.h"
#include "../../base/json.h"
#include "../abstractClass.h"
#include "../movement/movement.h"

class CharObj;
class ViewMgr;

typedef enum CharacterType
{
	CHAR_PLAYER,
	CHAR_MONSTER,
	CHAR_BOSS
} CharacterType;

struct CharStats
{
	bool crouch;
	bool moving;

	bool inAir;
	bool doubleJump;
	unsigned int jump;
	unsigned int jumpMax;

	bool hasDoubleJump;
	bool canDoubleJump;

	unsigned int life;
	unsigned int lifeMax;

	unsigned int moveSpeed;
	unsigned int maxMoveSpeedX;
	unsigned int maxMoveSpeedY;
};

class Character : public AbstractClass
{
	private:
		CharStats stats;
		CharacterType type;

		CharObj* obj = NULL;
		ViewMgr* view = NULL;
		Json* loadConfig(const char* jsonKey);

		void initCallableFncs();
		void initCollision(Json* js);

	protected:
		void initAnimFncs();
		virtual void initStats();

	public:
		void removeView();
		void setView(ViewMgr* v);

		Character(CharacterType type, const char* name, const char* jsonKey, vector* pos, int z);
		~Character();

		CharObj* getObject();
		void moveDir(DirectionEnum dir);

		void loadObject(Json* data, const char* name, vector* pos, int z);
		void stopMove();

		bool isOnLand();
		bool isRunning();
		bool isStopped();
		bool isJumping();
		bool isFalling();
		bool isCrouching();

		void update(bool gravity);

		void Crouch();
		void Jump(bool full);

		void landed();


		bool hit(Collision* col, Collision* col2);


		bool canMove();
};

#include "../object/sprite/character/charObject.h"
#include "../view/view.h"

#endif
