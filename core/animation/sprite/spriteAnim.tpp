template<typename T>
bool SpriteAnim::callAnimLinkFnc(T* obj, AnimLink* link) {
	AnimLinkFnc<T>* l = obj->getAnimLinkFnc(link->name, obj);

	if (l == NULL) {
		//Log::war(LOG_SPRITE_ANIM, "Fail To Find Anim LINK: %s !!!", link->name);
		return false;
	}

	bool b = l->call();
	return b;
}