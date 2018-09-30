#include "timeManager.h"

#include <chrono>
using namespace std::chrono;

void TimeManager::start() {
	this->update1 = 0;
	this->update2 = 0;

	this->startAt = this->getTime();
}

void TimeManager::update() {
	this->update2 = this->update1;
	this->updateAt = this->update1 = this->getTime();
}

double TimeManager::getTime() {
	return duration_cast<microseconds>(
	    system_clock::now().time_since_epoch()
	).count();
}

double TimeManager::getElapsedTime() const {
	if (this->update1 == 0 || this->update2 == 0) {
		return 0;
	}

	return this->update1 - this->update2;
}

double TimeManager::getStartedTime() const {
	if (this->startAt == 0 || this->updateAt == 0) {
		return 0;
	}

	return this->updateAt - this->startAt;
}
