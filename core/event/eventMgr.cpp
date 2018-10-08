#include "eventMgr.h"

EventMgr::EventMgr() {
	STATIC_CLASS_CONSTRUCT(EventMgr)
}

EventMgr::~EventMgr() {
	if (this->keyEvts != NULL) {
		deleteList(this->keyEvts);
	}
	
	if (this->inputEvts != NULL) {
		deleteList(this->inputEvts);
	}
}


void EventMgr::init(RenderWindow* window) {
	if (this->isInit) {
		return;
	}

	this->window = window;
	this->isInit = true;

	this->keyEvts = initListMgr();
	this->inputEvts = initListMgr();
}


void EventMgr::handleKey(Event* event) {
	Log::inf(LOG_EVENT_KEY, "=== Handeling Key Event ===");

	Node* n = NULL;
	while ((n = listIterate(this->keyEvts, n)) != NULL) {
		KeyEvtAbstract* evt = (KeyEvtAbstract*) n->value;

		if (evt->getKey() == event->key.code) {
			Log::inf(LOG_EVENT_KEY, "-- Event Key Found: %s", evt->getName());

			if (event->type == Event::KeyPressed) {
				evt->callPressed(*event);
			}
			else {
				evt->callReleased(*event);
			}			
		}
	}
}

void EventMgr::handle() {
	if (this->window == NULL) {
		Log::war(LOG_EVENT, "Can't Handle Event, EventMgr::window Is  NULL !!!");
		return;
	}

	Event event;
	Project* pro = Project::get();

	while (pro->getStatus() != PRO_CLOSE && this->window->pollEvent(event)) {
		switch (event.type) {
		    case Event::Closed:
		        Log::inf(LOG_EVENT, "=== Event: Close");
		        pro->changeStatus(PRO_CLOSE);
		        break;

		    case Event::KeyPressed:
		    case Event::KeyReleased:
		        Log::inf(LOG_EVENT, "=== KeyEvent");
		        this->handleKey(&event);
		        break;

		      default:
		      	break;
		}

	}
}