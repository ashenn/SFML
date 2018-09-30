#ifndef EVENT_MGR_H
#define EVENT_MGR_H

#include "../render/render.h"
#include "../project/project.h"

class EventMgr : public AbstractStaticClass
{
	private:
		EventMgr();
		~EventMgr();
		RenderWindow* window = NULL;
	
	public:
		STATIC_CLASS_BODY(EventMgr)

		void handle();
		void init(RenderWindow* window);
};

#endif