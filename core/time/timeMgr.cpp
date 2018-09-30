#include "timeMgr.h"

#include <chrono>
using namespace std::chrono;

void TimeMgr::start() {
	this->update1 = 0;
	this->update2 = 0;

	this->startAt = this->getTime();
}

void TimeMgr::update() {
	this->update2 = this->update1;
	this->updateAt = this->update1 = this->getTime();
}

double TimeMgr::getTime() {
	return duration_cast<microseconds>(
	    system_clock::now().time_since_epoch()
	).count();
}

double TimeMgr::getElapsedTime() const {
	if (this->update1 == 0 || this->update2 == 0) {
		return 0;
	}

	return this->update1 - this->update2;
}

double TimeMgr::getStartedTime() const {
	if (this->startAt == 0 || this->updateAt == 0) {
		return 0;
	}

	return this->updateAt - this->startAt;
}
