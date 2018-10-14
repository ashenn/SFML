template <class T>
void Collision::setHit(T* target, bool (T::*fnc)(Collision*, Collision*)) {
	if (this->onHit != NULL) {
		delete this->onHit;
	}

	this->onHit = new CollisionCallBack<T>(target, fnc);
}

template <class T>
void Collision::setOverlap(T* target, bool (T::*fnc)(Collision*, Collision*)) {
	if (this->onOverlap != NULL) {
		delete this->onOverlap;
	}

	this->onOverlap = new CollisionCallBack<T>(target, fnc);
}

template <class T>
bool CollisionCallBack<T>::call(Collision* col, Collision* col2) {
	T* targ = this->target;

	return (targ->*(this->fnc))(col, col2);
}