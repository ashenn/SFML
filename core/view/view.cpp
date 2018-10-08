#include "view.h"

ViewMgr::ViewMgr(Character* ch, ViewType type, sf::FloatRect pos, ViewBoundType boundType) {
	Log::inf(LOG_VIEW, "=== Init ViewMgr ===");

	this->ch = ch;
	this->type = type;
	this->boundType = boundType;

	this->bound = {0, 0};
	this->boundLimit = {0, 0};

	this->view = new sf::View(pos);
	this->ch->setView(this);
	Render::get()->setView(this->view);
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
		

		this->view->setCenter(plyPos.x + this->bound.x, plyPos.y + this->bound.y);
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