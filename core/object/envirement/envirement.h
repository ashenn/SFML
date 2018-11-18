#ifndef ENVIREMENT_H
#define ENVIREMENT_H

#include "../object.h"

class EnvObj : public Object
{
	public:
		EnvObj(const char* name, const char* path, const char* conf, vector* pos, unsigned int z);
		EnvObj(const char* name, const char* path, const char* conf, vector* pos, unsigned int z, IntRect* resize);
		~EnvObj();
};


class AiCtrl;
class EnvSection : public AbstractClass
{
	private:
		vector pos;

		char* sprite;
		ListManager* objects;
		ListManager* monsters;

		void loadObjects(ListManager* objects);

	public:
		EnvSection(Json* conf);
		~EnvSection();
	
		void spawnMonsters(ListManager* monsters);
		void removeAi(AiCtrl* ai);
};

#include "../../controller/ai/aiCtrl.h"

#endif