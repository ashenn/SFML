#ifndef PLAYER_CTRL_H
#define PLAYER_CTRL_H

#include "../controller.h"
#include "../../control/control.h"


class PlayerCtrl : public Controller
{
	protected:
		unsigned short id = 0;

		Object* checkpoint = NULL;
		static ListManager* playerList;

		void loadControls();
		void loadControl(Json* ctrl, const char* playerName);

	public:
		~PlayerCtrl();
		PlayerCtrl(int id, const char* name, const char* jsonKey, vector* pos, int z);

		unsigned short getId();
		virtual void setCharacter(Character* ch, bool deleteOld);

		virtual bool hit(Collision* col, Collision* col2);
		virtual bool hitWall(Collision* col, Collision* col2, IntRect pos, IntRect pos2);
		
		bool hitMonster(Collision* col, Collision* col2, IntRect pos, IntRect pos2);


		bool overlapCheckPoint(Object* checkObj);
		virtual bool overlap(Collision* col, Collision* col2);

		void kill();

		void moveEvt(KeyEvt<PlayerCtrl>* evt);
		void stopEvt(KeyEvt<PlayerCtrl>* evt);
		void downEvt(KeyEvt<PlayerCtrl>* evt);
		void jumpEvt(KeyEvt<PlayerCtrl>* evt);
};

#endif