template<typename T>
void SpriteObj::addAnimLinkFnc(const char* name, T* target, bool (T::*fnc)()) {
	if (getNodeByName(this->animLinkFncs, name)) {
		Log::war(LOG_SPRITE_ANIM, "Trying To Add Duplicate Aim Link Function: %s", name);
		return;
	}

	AnimLinkFnc<T>* animFnc = new AnimLinkFnc<T>(target, fnc);
	addNodeV(this->animLinkFncs, name, animFnc, false);
}

template<typename T>
AnimLinkFnc<T>* SpriteObj::getAnimLinkFnc(const char* name, T* target) {
	Node* n = getNodeByName(this->animLinkFncs, name);

	if(n == NULL) {
		return NULL;
	}

	return (AnimLinkFnc<T>*) n->value;
}


// template<typename T>
// void SpriteObj::clearAnimLinks(T* obj) {
// 	Node* n = NULL;

// 	while ((n = listIterate(this->animLinkFncs, n)) != NULL) {
// 		if (n->value != NULL) {
// 			delete (AnimLinkFnc<T>*) n->value;
// 		}

// 		removeAndFreeNode(this->animLinkFncs, n);
// 		n = NULL;
// 	}
// }