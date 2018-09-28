#include "abstractClass.h"
#include <sys/time.h>

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

void AbstractClass::wait() {
	pthread_cond_wait(&this->cond, &this->mutex);
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
}

void AbstractClass::signal() {
	pthread_cond_signal(&this->cond);
}


bool AbstractClass::lock(const char* tag) {
	pid_t curTH = getpid();

	if (curTH == this->pid) {
		return false;
	}

	this->pid = curTH;


	if (this->lockTag != NULL) {
		free(this->lockTag);
	}

	this->lockTag = Str(tag);

	if (this->unlockTag != NULL) {
		free(this->unlockTag);
		this->unlockTag = NULL;
	}

	pthread_mutex_lock(&this->mutex);
	return true;
}

void AbstractClass::unlock(const char* tag, bool b) {
	if (!b) {
		return;
	}

	if (this->pid == getpid())
	{
		this->pid = -1;
		pthread_mutex_unlock(&this->mutex);

		if (this->unlockTag != NULL) {
			free(this->unlockTag);
		}

		this->unlockTag = Str(tag);
	}
	else if(this->pid >= 0){
		Log::err(LOG_MAIN, "Trying To Unlock Thread But Mutex Was Locked By An Other: %s FROM %s", this->name, tag);
		Log::err(LOG_MAIN, "-- Locked By %s", this->lockTag);
		Log::err(LOG_MAIN, "-- UnLocked By %s", this->unlockTag);
	}
	else {
		Log::err(LOG_MAIN, "Trying To Unlock Thread None Locked Mutex: %s | FROM %s", this->name, tag);
		Log::err(LOG_MAIN, "-- Locked By %s", this->lockTag);
		Log::err(LOG_MAIN, "-- UnLocked By %s", this->unlockTag);
	}
}
