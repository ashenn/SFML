template<typename T>
void SpriteObj::addAnimLinkFnc(const char* name, bool (T::*fnc)()) {
	if (getNodeByName(this->animLinkFncs, name)) {
		Log::war(LOG_SPRITE_ANIM, "Trying To Add Duplicate Aim Link Function: %s", name);
		return;
	}

	AnimLinkFnc<T>* animFnc = new AnimLinkFnc<T>();
	//animFnc->obj = obj;
	animFnc->fnc = fnc;

	addNodeV(this->animLinkFncs, name, animFnc, false);
}

template<typename T>
AnimLinkFnc<T>* SpriteObj::getAnimLinkFnc(const char* name, T* obj) {
	Node* n = getNodeByName(this->animLinkFncs, name);

	if(n == NULL) {
		return NULL;
	}

	return (AnimLinkFnc<T>*) n->value;
}


template<typename T>
void SpriteObj::clearAnimLinks(T* obj) {
	Node* n = NULL;

	while ((n = listIterate(this->animLinkFncs, n)) != NULL) {
		if (n->value != NULL) {
			delete (AnimLinkFnc<T>*) n->value;
		}

		removeAndFreeNode(this->animLinkFncs, n);
		n = NULL;
	}
}