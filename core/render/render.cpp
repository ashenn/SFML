#include "render.h"

Render::Render() {
	this->objectList = initListMgr();
}

Render::~Render() {
	deleteList(this->objectList);
}

ListManager* Render::getObjectList() {
	return this->objectList;
}