#include "render.h"

#include "../time/timeMgr.h"
#include "../animation/animation.h"

Render::Render() {
	this->name = Str("Render");
	this->layers = initListMgr();
	this->objectList = initListMgr();
}

Render::~Render() {
	deleteList(this->layers);
	deleteList(this->objectList);
	this->window->close();
}

ListManager* Render::getObjectList() {
	return this->objectList;
}

Node* Render::addObject(Object* obj) {
	if (obj == NULL) {
		Log::war(LOG_RENDER, "Trying To add NULL Object To View");
		return NULL;
	}
	

	Node* n = addNodeV(this->objectList, obj->getName(), obj, false);
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
	static int i = 0;
	Node* layerN = NULL;


	bool b = this->lock("Render Lock");
	Log::inf(LOG_RENDER, "=== Rendering ===");
	sf::Color c(17,32,36,255);
	this->window->clear(c);

	Log::dbg(LOG_RENDER, "-- Start Loop");
	while((layerN = listIterate(this->layers, layerN)) != NULL) {
		ListManager* layer = (ListManager*) layerN->value;
		Log::dbg(LOG_RENDER, "## Rendering: %s ##", layerN->name);
		

		Node* objN = NULL;
		while((objN = listIterate(layer, objN)) != NULL) {
			Object* obj = (Object*) objN->value;
			Log::dbg(LOG_RENDER, "---- %s", obj->getName());

			obj->draw(this->window, i == 10);
		}
	}

	// Node* objN = NULL;
	// while((objN = listIterate(this->objectList, objN)) != NULL) {
	// 	Object* obj = (Object*) objN->value;
	// 	Log::dbg(LOG_RENDER, "---- %s", obj->getName());

	// 	obj->draw(this->window, i == 10);
	// }
	

	Log::dbg(LOG_RENDER, "-- Display window Content");
	window->display();

	if (i && (i == 10)) {
		i = 0;
	}
	else{
		i++;
	}

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

RenderWindow* Render::getWindow() {
	return this->window;
}

void Render::setView(sf::View* view) {
	if (this->window == NULL) {
		Log::war(LOG_RENDER, "Trying to Set View On NULL Window !!!");
		return;
	}

	this->window->setView(*view);
}

const sf::View& Render::getView() {
	return this->window->getView();
}

void deleteLayer(Node* n) {
	if (n->value == NULL) {
		return;
	}

	deleteList((ListManager*) n->value);
}

ListManager* Render::getLayer(int z) {
	Log::inf(LOG_RENDER, "=== Getting Layer: %d ===", z);
	Node* n = getNode(this->layers, z);
	if (n != NULL) {
		return (ListManager*) n->value;
	}

	Log::dbg(LOG_RENDER, "-- Init Layer: %d", z);
	
	int len = (z / 10) + 8;
	char name[len];
	memset(name, 0, len);
	snprintf(name, len, "layer-%d", z);

	n = addNodeV(this->layers, name, initListMgr(), false);

	if (n == NULL) {
		Log::err(LOG_RENDER, "Fail To Create Layer: %s", name);
		return NULL;
	}

	Log::dbg(LOG_RENDER, "-- Layer Created");
	n->id = z;
	n->del = deleteLayer;

	Log::dbg(LOG_RENDER, "-- Sorting Layers");
	sortListById(this->layers);

	return (ListManager*) n->value;
}

void Render::removeFromLayer(Object* obj) {
	if (obj->layer == NULL) {
		return;
	}

	Log::inf(LOG_RENDER, "=== Remove '%s' From Layer ===", obj->getName());

	deleteNodeByValue(obj->layer, obj);
	obj->layer = NULL;
}

void Render::addToLayer(Object* obj) {
	Log::inf(LOG_RENDER, "=== Adding '%s' To Layer %u ===", obj->getName(), obj->getZ());
	this->removeFromLayer(obj);
	
	Log::dbg(LOG_RENDER, "-- Getting Layer: %u", obj->getZ());
	ListManager* layer = this->getLayer((int) obj->getZ());
	if (layer == NULL) {
		Log::err(LOG_RENDER, "Fail To Add Object: %s To Layer: %u", obj->getName(), obj->getZ());
	}

	Log::dbg(LOG_RENDER, "-- Adding Object");
	addNodeUniqValue(layer, obj->getName(), obj, false);

	obj->layer = layer;
}