#include "render.h"

#include "../time/timeMgr.h"
#include "../animation/animation.h"

Render::Render() {
	this->name = Str("Render");
	this->objectList = initListMgr();
}

Render::~Render() {
	deleteList(this->objectList);
}

ListManager* Render::getObjectList() {
	return this->objectList;
}

Node* Render::addObject(Object* obj) {
	if (obj == NULL) {
		Log::war(LOG_RENDER, "Trying To add NULL Object To View");
		return NULL;
	}
	

	Node* n = addNodeV(this->objectList, obj->getName(), obj, true);
	if (n == NULL) {
		Log::err(LOG_RENDER, "Fail To add Object To View !!");
		return NULL;
	}

	return n;
}

void Render::init(RenderWindow* window) {
	if (isInit) {
		return;
	}

	Log::inf(LOG_RENDER, "=== Init Render ===");
	this->window = window;

	isInit = true;
}

void Render::render() {
	Node* n = NULL;

	bool b = this->lock("Render Lock");
	Log::inf(LOG_RENDER, "=== Rendering ===");

	Log::dbg(LOG_RENDER, "-- cleaning window: %p", window);
	this->window->clear();


	Log::dbg(LOG_RENDER, "-- Start Loop");
	while((n = listIterate(this->objectList, n)) != NULL) {
		Object* obj = (Object*) n->value;

		Log::dbg(LOG_RENDER, "-- Rendering: %s", obj->getName());
		this->window->draw(*obj->getSprite());
	}
	

	Log::dbg(LOG_RENDER, "-- Display window Content");
	window->display();

	Log::dbg(LOG_RENDER, "=== Rendering Done ===");
	this->unlock("Render UnLock", b);
}

void* renderThread(void* param) {
	Log::inf(LOG_RENDER, "=== Start Render Thread ===");
	
	Render* rend = Render::get();
	Project* pro = Project::get();
	Animator* anim = Animator::get();

	TimeMgr* time = TimeMgr::get();
	unsigned int eleapsed = 0;

	Log::inf(LOG_RENDER, "=== Start Render Loop ===");
	pro->signal();
	rend->lock("Start Render");

	while(pro->getStatus() < PRO_CLOSE) {
		rend->unlock("Render Loop", true);

		time->update();

		anim->animate();
		rend->render();
		

		eleapsed = time->getElapsedTime();
		double wait = FRAME - eleapsed;

		if (wait > 0) {
			usleep(wait);
		}

		rend->lock("Render Loop");
	}

	rend->unlock("Render End", true);
	Log::inf(LOG_RENDER, "=== Render Thread Done ===");

	return NULL;
}

void Render::close() {
	deleteList(this->objectList);
	this->window->close();
}

RenderWindow* Render::getWindow() {
	return this->window;
}
