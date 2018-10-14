template <class T>
KeyEvt<T>* EventMgr::bindKeyEvent(const char* name, sf::Keyboard::Key key, T* obj) {
	Node* n = getNodeByName(this->keyEvts, name);

	if (n != NULL) {
		Log::war(LOG_EVENT_KEY, "Trying To Add Duplicate KeyEvt Name: '%s'", name);
		return NULL;
	}


	Log::inf(LOG_EVENT_KEY, "=== Binding Event: %s ===", name);
	KeyEvt<T>* evt = new KeyEvt<T>(name, key, obj);

	n = addNodeV(this->keyEvts, name, evt, false);
	n->del = deleteEvent;


	return evt;
}


template <class T>
KeyEvt<T>* EventMgr::bindKeyEvent(const char* name, sf::Keyboard::Key key, T* obj, Json* conf) {
	Node* n = getNodeByName(this->keyEvts, name);

	if (n != NULL) {
		Log::war(LOG_EVENT_KEY, "Trying To Add Duplicate KeyEvt Name: '%s'", name);
		return NULL;
	}


	Log::inf(LOG_EVENT_KEY, "=== Binding Event: %s ===", name);
	KeyEvt<T>* evt = new KeyEvt<T>(name, key, obj, conf);

	n = addNodeV(this->keyEvts, name, evt, false);
	n->del = deleteEvent;


	return evt;

}

template <class T>
KeyEvt<T>* EventMgr::bindKeyEvent(const char* name, sf::Keyboard::Key key, T* obj, void (T::*fnc)(KeyEvt<T>*)) {
	KeyEvt<T>* evt = this->bindKeyEvent(name, key, obj);
	evt->setOnPress(fnc);
}