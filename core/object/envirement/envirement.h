#ifndef ENVIREMENT_H
#define ENVIREMENT_H

#include "../object.h"

class EnvObj : public Object
{
	public:
		EnvObj(const char* name, const char* path, const char* conf, vector* pos, unsigned int z);
		~EnvObj();
};

#endif