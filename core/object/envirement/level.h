#ifndef LEVEL_H
#define LEVEL_H

#include "envirement.h"

class Level : AbstractClass
{
	private:
		Json* conf;
		ListManager* sections;

		Level(const char* name);
		~Level();

		void loadSections();

		static Level* curLevel;
		static void closeLevel();

	public:
		static void close();
		static Level* load(const char* name);
		static Level* getLevel();
		
	
};

#endif