template<typename T>
bool SpriteAnim::callAnimLinkFnc(T* obj, AnimLink* link) {
	AnimLinkFnc<T>* l = obj->getAnimLinkFnc(link->name, obj);

	if (l == NULL) {
		Log::war(LOG_SPRITE_ANIM, "Fail To Find Anim LINK: %s !!!", link->name);
		return false;
	}

	Log::war(LOG_SPRITE_ANIM, "TEST LINK: %s !!!", link->name);
	bool b = (obj->*(l->fnc))();

	Log::war(LOG_SPRITE_ANIM, "LINK Result: %d !!!", b);
	return b;
}