#include "keyEvent.h"

HoldAbstract::HoldAbstract () {
	this->cond = new std::condition_variable();
}

HoldAbstract::~HoldAbstract () {
	delete this->cond;
}

void HoldAbstract::done () {
	this->started = 0;
	this->killed = false;
}

void holdThread(HoldAbstract* hold) {
	std::unique_lock<std::mutex> lk(hold->mutex);

	if (hold->max) {
		float time = hold->max * 1000;
		hold->cond->wait_for(lk, std::chrono::milliseconds((int) time));
	}
	else{
		hold->cond->wait(lk);
	}

	if (hold->killed) {
		return;
	}

	hold->call();
}

Keyboard::Key KeyEvtAbstract::getKey(){
	return this->key;
}

sf::Event KeyEvtAbstract::getEvent(){
	return this->evt;
}


void deleteEvent(Node* n) {
	if (n->value == NULL) {
		return;
	}

	Evt* evt = (Evt*) n->value;
	delete evt;
}