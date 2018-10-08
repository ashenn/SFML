#ifndef EVENT_H
#define EVENT_H

#include "../render/render.h"
#include "../project/project.h"

#include <SFML/Graphics.hpp>

class Evt : public AbstractClass
{
	public:
	    void* arg;
		bool enabled;
	    bool (*fnc)(Event* evt);
	    void (*remove)(Event* evt);
};


#endif