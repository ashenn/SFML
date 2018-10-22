#ifndef AI_CTRL_H
#define AI_CTRL_H

#include "../controller.h"

class AiCtrl : public Controller
{
	private:
		DirectionEnum dir[2];
		static ListManager* aiList;

	public:
		~AiCtrl();
		AiCtrl(const char* name, const char* jsonKey, vector* pos, int z);

		virtual void kill();
		virtual void update();
		
		virtual bool hit(Collision* col, Collision* col2);
		virtual bool hitWall(Collision* col, Collision* col2, IntRect pos, IntRect pos2);

		virtual bool overlap(Collision* col, Collision* col2);
};

#endif