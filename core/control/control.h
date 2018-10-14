#ifndef CONTROL_H
#define CONTROL_H

#include "../abstractClass.h"

class ControlMgr : public AbstractStaticClass {
	private:
		ListManager* keys;
		// ListManager* controls;

		ControlMgr();
		~ControlMgr();

	public:
		STATIC_CLASS_BODY(ControlMgr)
		sf::Keyboard::Key getKey(const char* name);
		const char* getKeyName(sf::Keyboard::Key key);
};

#endif