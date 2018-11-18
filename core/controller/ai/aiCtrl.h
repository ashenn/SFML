#ifndef AI_CTRL_H
#define AI_CTRL_H

#include "../controller.h"
#include "../../object/envirement/envirement.h"

class AiCtrl : public Controller
{
	private:
		DirectionEnum dir[2];
		//static ListManager* aiList;
		EnvSection* section = NULL;

	public:
		~AiCtrl();
		AiCtrl(const char* name, const char* jsonKey, vector* pos, int z, EnvSection* section);
		AiCtrl(const char* name, const char* jsonKey, vector* pos, int z) : AiCtrl(name, jsonKey, pos, z, NULL){};

		virtual void kill();
		virtual void update();
		
		virtual bool hit(Collision* col, Collision* col2);
		virtual bool hitWall(Collision* col, Collision* col2, IntRect pos, IntRect pos2);
		virtual bool hitPlayer(Collision* col, Collision* col2, IntRect pos, IntRect pos2);

		
		virtual bool willFall();
		virtual bool overlap(Collision* col, Collision* col2);
};

#endif