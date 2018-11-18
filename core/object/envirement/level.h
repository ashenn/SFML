#ifndef LEVEL_H
#define LEVEL_H

#include "envirement.h"

class Level : AbstractClass
{
	private:
		Json* conf;
		vector start;
		vector playerStart;
		ListManager* sections;

		Level(const char* name);
		~Level();

		void loadStart();
		void loadEnd();
		void loadSections();

		static Level* curLevel;
		static void closeLevel();

	public:
		static void close();
		static Level* load(const char* name);
		static Level* getLevel();
		
	
};

#endif