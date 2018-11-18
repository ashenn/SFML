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

	bool canAttack;
	bool attackPosition;
	unsigned short attack;
	unsigned short maxAttack;

	bool hasDoubleJump;
	bool canDoubleJump;
	bool doubleJumping;

	unsigned int life;
	unsigned int lifeMax;

	unsigned int moveSpeed;
	unsigned int maxMoveSpeedX;
	unsigned int maxMoveSpeedY;

	bool canFallOfEdge;
	unsigned int stepHeight;
};

class Controller;
class Character : public AbstractClass
{
	private:
		CharStats stats;
		CharacterType type;

		bool alive = true;
		CharObj* obj = NULL;
		Controller* ctrl = NULL;

		ViewMgr* view = NULL;
		Json* loadConfig(const char* jsonKey);

		DirectionEnum dir[2];

		ListManager* attacks = NULL;

		void initCallableFncs();
		void initCollision(Json* js);

		void setAttackCol();
		void loadAttacks(Json* attacks);

	protected:
		void initAnimFncs();
		virtual void initStats(Json* data);

	public:
		bool isAlive();
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
		bool Idle2Sword();
		bool Sword2Idle();
		bool Idle2Attack1();
		bool Attack2Idle();

		bool Attack2();
		bool Attack3();
		bool canAttack();

		bool inAir();

		void update(bool gravity);

		void Stand();
		void Crouch();
		void Jump(bool full);

		void landed();

		void attack();
		bool attack_end();

		bool hit(Collision* col, Collision* col2);
		bool hitWall(Collision* col, Collision* col2, IntRect pos, IntRect pos2);
		bool hitMonster(Collision* col, Collision* col2, IntRect pos, IntRect pos2);
		
		bool overlap(Collision* col, Collision* col2);
		bool attackOverlap(Collision* col, Collision* col2);

		void kill();
		bool canMove();
		void takeDamage(unsigned int dmg);
		void makeDamage(Character* target);

		bool canFallOfEdge();
		unsigned int getStepHeight();

		Controller* getCtrl();
		void setCtrl(Controller* ctrl);
};

#include "../object/sprite/character/charObject.h"
#include "../view/view.h"

#endif
