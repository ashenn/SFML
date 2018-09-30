#include "event.h"

EventMgr::EventMgr() {
	STATIC_CLASS_CONSTRUCT(EventMgr)
}

EventMgr::~EventMgr() {
}


void EventMgr::init(RenderWindow* window) {
	if (this->isInit) {
		return;
	}

	this->window = window;
	this->isInit = true;
}

void EventMgr::handle() {
	if (this->window == NULL) {
		Log::war(LOG_EVENT, "Can't Handle Event, EventMgr::window Is  NULL !!!");
		return;
	}

	Event event;
	Project* pro = Project::get();

	while (this->window->pollEvent(event)) {

	    if (event.type == Event::Closed) {
	        Log::inf(LOG_EVENT, "=== Event: Close");
	        pro->changeStatus(PRO_CLOSE);
	        break;
	    }

	}
}