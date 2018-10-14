#ifndef PLAYER_CTRL_H
#define PLAYER_CTRL_H

#include "../controller.h"
#include "../../control/control.h"

class PlayerCtrl : public Controller
{
	protected:
		unsigned short id = 0;

		static ListManager* playerList;

		void loadControls();
		void loadControl(Json* ctrl, const char* playerName);

	public:
		~PlayerCtrl();
		PlayerCtrl(int id, const char* name, const char* jsonKey, vector* pos, int z);

		unsigned short getId();
		void setCharacter(Character* ch, bool deleteOld);

		void moveEvt(KeyEvt<PlayerCtrl>* evt);
		void stopEvt(KeyEvt<PlayerCtrl>* evt);
		void downEvt(KeyEvt<PlayerCtrl>* evt);
		void jumpEvt(KeyEvt<PlayerCtrl>* evt);
};

#endif