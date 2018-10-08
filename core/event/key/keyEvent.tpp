#include "keyEvent.h"
#include "../../time/timeMgr.h"

#include <chrono>

template <class T>
Hold<T>::Hold(T* obj, KeyEvt<T>* evt, void (T::*fnc)(KeyEvt<T>*), float min, float max) {
	this->evt = evt;

	this->killed = false;
	this->enable = true;
	
	this->fnc = fnc;
	this->min = min;
	this->max = max;
	
	this->time = 0;
	this->started = 0;
}

template <class T>
Hold<T>::~Hold() {
	if (this->delayed != NULL) {
		this->kill();

		delete this->delayed;
	}
}

template <class T>
void Hold<T>::call() {
	double time = (TimeMgr::get()->getTime() - this->started) / 1000000;

	if (time >= this->min) {
		T* obj = (T*) this->evt->target;
		(obj->*(this->fnc))(evt);
	}
}


template <class T>
void Hold<T>::start() {
	if (this->delayed != NULL) {
		if (this->started){
			return;
		}

		delete this->delayed;
		this->delayed = NULL;
	}

	this->started = TimeMgr::get()->getTime();
	this->delayed = new std::thread(&holdThread, (HoldAbstract*) this);
}

template <class T>
void Hold<T>::kill() {
	if (this->delayed == NULL) {
		return;
	}

	this->killed = true;

	if (this->started) {
		this->cond->notify_one();
		this->delayed->join();
	}

	this->done();
}

template <class T>
void Hold<T>::signal() {
	if (this->delayed == NULL) {
		Log::war(LOG_EVENT_KEY, "Trying To Signal Not Lauched Hold Event !!!");
		return;
	}

	this->cond->notify_one();
	this->delayed->join();

	this->done();
}

template <class T>
KeyEvt<T>::KeyEvt(const char* name, sf::Keyboard::Key key, T* obj) {
	this->key = key;
	this->name = Str(name);
	this->target = obj;
}

template <class T>
KeyEvt<T>::~KeyEvt() {
	if (this->onHold != NULL) {
		delete this->onHold;
	}

	if (this->onPress != NULL) {
		delete this->onPress;
	}

	if (this->onRelease != NULL) {
		delete this->onRelease;
	}
}

template<class T>
void KeyEvt<T>::setOnPress(T* obj, void (T::*fnc)(KeyEvt<T>*)) {
	if (this->onPress != NULL) {
		delete this->onPress;
	}

	this->onPress = new Pressed<T>();
	this->onPress->enable = true;
	this->onPress->fnc = fnc;
}

template<class T>
void KeyEvt<T>::setOnRelease(T* obj, void (T::*fnc)(KeyEvt<T>*)) {
	if (this->onRelease != NULL) {
		delete this->onRelease;
	}

	this->onRelease = new Released<T>();
	this->onRelease->enable = true;
	this->onRelease->fnc = fnc;
}

template<class T>
void KeyEvt<T>::setOnHold(T* obj, void (T::*fnc)(KeyEvt<T>*), float min, float max) {
	if (this->onHold != NULL) {
		delete this->onHold;
	}

	this->onHold = new Hold<T>(obj,this, fnc, min, max);
}


template <class T>
void KeyEvt<T>::killOnHold() {
	if (this->hold == NULL) {
		Log::war(LOG_EVENT_KEY, "Trying To Kill NULL Hold Event: %s", this->name);
		return;
	}

	this->hold->kill();
}

template <class T>
void KeyEvt<T>::callOnHold() {
	if (this->onHold == NULL) {
		Log::war(LOG_EVENT_KEY, "Trying To Call NULL Hold Event: %s", this->name);
		return;
	}

	this->onHold->signal();
}

template <class T>
void KeyEvt<T>::callPressed(sf::Event evt) {
	this->evt = evt;

	if (this->onHold != NULL) {
		this->onHold->start();
	}
	else if (this->onPress != NULL) {
		T* obj = (T*) this->target;
		Pressed<T>* p = this->onPress;

		(obj->*(p->fnc))(this);
	}
}

template <class T>
void KeyEvt<T>::callReleased(sf::Event evt) {
	this->evt = evt;


	if (this->onHold != NULL) {
		this->callOnHold();
	}
	else if (this->onRelease != NULL) {
		T* obj = (T*) this->target;
		Released<T>* p = this->onRelease;

		(obj->*(p->fnc))(this);
	}
}