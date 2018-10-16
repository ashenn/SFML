#include "view.h"

ViewMgr::ViewMgr(Character* ch, ViewType type, sf::FloatRect pos, ViewBoundType boundType) {
	Log::inf(LOG_VIEW, "=== Init ViewMgr ===");

	this->ch = ch;
	this->type = type;
	this->boundType = boundType;

	this->rect = pos;
	this->bound = {0, 0};
	this->boundLimit = {0, 0};


	this->view = new sf::View(pos);
	this->ch->setView(this);
	Render::get()->setView(this->view);
	ViewMgr::addView(this);
}

ViewMgr::~ViewMgr() {
	delete this->view;
}

ViewType ViewMgr::getType() {
	return this->type;
}

ViewBoundType ViewMgr::getBoundType() {
	return this->boundType;
}


void ViewMgr::setBound(vector b) {
	this->bound = b;
}

void ViewMgr::setBoundLimit(vector bl) {
	this->boundLimit = bl;
}

void ViewMgr::update() {
	Log::dbg(LOG_VIEW, "=== Update ViewMgr ===");
	vector plyPos = this->ch->getObject()->getPosition();

	if (this->boundType == VIEW_BOUND_FIXED) {
		Log::dbg(LOG_VIEW, "Recenter View");
		Log::dbg(LOG_VIEW, "Obj => X: %lf | Y: %lf", plyPos.x, plyPos.y);
		Log::dbg(LOG_VIEW, "Bound X: %lf | Y: %lf", this->bound.x, this->bound.y);
		

		this->rect.left = plyPos.x + this->bound.x;
		this->rect.top = plyPos.y + this->bound.y;
		this->view->setCenter(this->rect.left, this->rect.top);
		Render::get()->setView(this->view);
		return;
	}

	// const Vector2f pos = this->view->getCenter();

	// vector calcPos;
	// vector newPos;

	// newPos.x = calcPos.x = this->bound.x + pos.x;
	// newPos.y = calcPos.y = this->bound.y + pos.y;

	// bool bChange = false;

	// if (calcPos.x > this->boundLimit) {
	// 	bChange = true;
	// 	newPos.x = calc
	// }
}

Character* ViewMgr::getOwner() {
	return this->ch;
}


sf::FloatRect ViewMgr::getRect() {
	return this->rect;
}

vector ViewMgr::getPos() {
	vector res;
	res.x = this->rect.left;
	res.y = this->rect.top;
	return res;
}

vector ViewMgr::getSize() {
	vector res;
	res.x = this->rect.width;
	res.y = this->rect.height;
	return res;
}



ListManager* ViewMgr::viewList = initListMgr();

void ViewMgr::addView(ViewMgr* v) {
	addNodeUniqValue(viewList, v->getOwner()->getName(), v, 0);
}

void ViewMgr::close() {
	deleteList(viewList);
}

ListManager* ViewMgr::getViews() {
	return viewList;
}