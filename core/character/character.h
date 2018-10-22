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

	unsigned int damage;

	bool hasDoubleJump;
	bool canDoubleJump;
	bool doubleJumping;

	unsigned int life;
	unsigned int lifeMax;

	unsigned int moveSpeed;
	unsigned int maxMoveSpeedX;
	unsigned int maxMoveSpeedY;
};

class Controller;
class Character : public AbstractClass
{
	private:
		CharStats stats;
		CharacterType type;

		CharObj* obj = NULL;
		Controller* ctrl = NULL;

		ViewMgr* view = NULL;
		Json* loadConfig(const char* jsonKey);

		DirectionEnum dir[2];

		void initCallableFncs();
		void initCollision(Json* js);

	protected:
		void initAnimFncs();
		virtual void initStats(Json* data);

	public:
		void removeView();
		void setView(ViewMgr* v);

		Character(CharacterType type, const char* name, const char* jsonKey, vector* pos, int z);
		~Character();

		CharObj* getObject();
		const DirectionEnum getDirX();
		const DirectionEnum getDirY();
		void moveDir(DirectionEnum dir);

		void loadObject(Json* data, const char* name, vector* pos, int z);
		void stopMove();

		bool isOnLand();
		bool isRunning();
		bool isStopped();
		bool isJumping();
		bool isFalling();
		bool isCrouching();
		bool isDoubleJump();
		bool isGlinding();
		bool isStanding();
		bool Down2Idle();
		bool Glide2Down();

		void update(bool gravity);

		void Stand();
		void Crouch();
		void Jump(bool full);

		void landed();


		bool hit(Collision* col, Collision* col2);
		bool hitWall(Collision* col, Collision* col2, IntRect pos, IntRect pos2);
		bool hitMonster(Collision* col, Collision* col2, IntRect pos, IntRect pos2);
		
		bool overlap(Collision* col, Collision* col2);

		void kill();
		bool canMove();
		void takeDamage(unsigned int dmg);
		void makeDamage(Character* target);

		void setCtrl(Controller* ctrl);
};

#include "../object/sprite/character/charObject.h"
#include "../view/view.h"

#endif
