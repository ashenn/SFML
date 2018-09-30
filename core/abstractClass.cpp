#include "abstractClass.h"
#include <sys/time.h>
#include <string>
#include <sstream>

AbstractClass::~AbstractClass() {
	if (this->name != NULL) {
		free(name);
	}

	if (this->lockTag != NULL) {
		free(lockTag);
	}

	if (this->unlockTag != NULL) {
		free(unlockTag);
	}
}

unsigned int AbstractClass::getId() {
	return this->id;
}

const char* AbstractClass::getName() {
	return this->name;
}

void AbstractClass::setLockTag(const char* tag) {
	if (this->lockTag != NULL) {
		free(this->lockTag);
		this->lockTag = NULL;
	}

	this->lockTag = Str(tag);
}

void AbstractClass::setUnlockTag(const char* tag) {
	if (this->unlockTag != NULL) {
		free(this->unlockTag);
		this->unlockTag = NULL;
	}

	this->unlockTag = Str(tag);
}

void AbstractClass::wait() {
	pthread_cond_wait(&this->cond, &this->mutex);

	this->pid = this->getThreadId();	
	this->setLockTag("WAIT");
}

void AbstractClass::wait_time(float delay) {
	struct timespec waitTime;
    struct timeval now;
	int seconds = (int)delay;
    float microsec =  (delay - ((float) seconds)) * 1000.0f;

	gettimeofday(&now,NULL);

    waitTime.tv_sec = now.tv_sec + (int) delay;
    waitTime.tv_nsec = (now.tv_usec + microsec) *1000UL;

	pthread_cond_timedwait(&this->cond, &this->mutex, &waitTime);
	this->setLockTag("TIMED WAIT");
}

void AbstractClass::signal() {
	pthread_cond_signal(&this->cond);
}


bool AbstractClass::lock(const char* tag) {
	double curTH = this->getThreadId();	

	if (curTH == this->pid) {
		Log::err(LOG_MAIN, "Trying To lock Thread From Same Thread: '%s' Locked  FROM '%s'", this->name, tag);
		Log::err(LOG_MAIN, "Pid: %d", (int) this->pid);
		return false;
	}


	pthread_mutex_lock(&this->mutex);
	this->pid = curTH;

	this->setLockTag(tag);
	this->setUnlockTag(NULL);

	return true;
}

void AbstractClass::unlock(const char* tag, bool b) {
	if (!b) {
		return;
	}

	double curTH = this->getThreadId();
	if (this->pid == curTH)
	{
		this->pid = -1;

		this->setLockTag(NULL);
		this->setUnlockTag(tag);

		pthread_mutex_unlock(&this->mutex);
	}
	else if(this->pid >= 0){
		Log::err(LOG_MAIN, "Trying To Unlock Thread But Mutex Was Locked By An Other: '%s' FROM '%s'", this->name, tag);
		Log::err(LOG_MAIN, "-- Locked By '%s'", this->lockTag);
		Log::err(LOG_MAIN, "-- UnLocked By '%s'", this->unlockTag);
	}
	else {
		Log::err(LOG_MAIN, "Trying To Unlock Thread None Locked Mutex: '%s' | FROM '%s'", this->name, tag);
		Log::err(LOG_MAIN, "-- Locked By '%s'", this->lockTag);
		Log::err(LOG_MAIN, "-- UnLocked By '%s'", this->unlockTag);
	}
}

double AbstractClass::getThreadId() {
	std::stringstream ss;
	ss << std::this_thread::get_id();
	
	return str2double(ss.str().c_str());
}