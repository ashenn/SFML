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

	bool doubleJump;
	bool hasDoubleJump;
	bool canDoubleJump;

	unsigned int life;
	unsigned int lifeMax;

	unsigned int moveSpeed;
	unsigned int maxMoveSpeed;
};

class Character : public AbstractClass
{
	private:
		CharStats stats;
		CharacterType type;

		CharObj* obj = NULL;
		ViewMgr* view = NULL;
		Json* loadConfig(const char* jsonKey);

	protected:
		void initAnimFncs();

	public:
		void removeView();
		void setView(ViewMgr* v);

		Character(CharacterType type, const char* name, const char* jsonKey, vector* pos, int z);
		~Character();

		CharObj* getObject();
		void moveDir(DirectionEnum dir);

		void loadObject(Json* data, const char* name, vector* pos, int z);
		void stopMove();

		bool isRunning();
		bool isStopped();

		void update();
};

#include "../object/sprite/character/charObject.h"
#include "../view/view.h"

#endif