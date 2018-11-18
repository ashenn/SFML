#include "control.h"

static sf::Keyboard::Key keysVals[] = {
	sf::Keyboard::Key::Unknown,
	sf::Keyboard::Key::Left,
	sf::Keyboard::Key::Right,
	sf::Keyboard::Key::Up,
	sf::Keyboard::Key::Down,
	sf::Keyboard::Key::Space,
	sf::Keyboard::Key::Z
};

const char* names[] = {
	"Unknown",
	"LEFT",
	"RIGHT",
	"UP",
	"DOWN",
	"SPACE",
	"Z",
	NULL
};

ControlMgr::ControlMgr() {
	STATIC_CLASS_CONSTRUCT(ControlMgr)

	this->keys = initListMgr();
	for (int i = 0; names[i] != NULL; ++i) {
		addNodeV(this->keys, names[i], &keysVals[i], 0);
	}
}

ControlMgr::~ControlMgr() {
	deleteList(this->keys);
}

sf::Keyboard::Key ControlMgr::getKey(const char* name) {
	Node* n = getNodeByName(this->keys, name);

	if (n == NULL) {
		return sf::Keyboard::Unknown;
	}

	sf::Keyboard::Key* key = (sf::Keyboard::Key*) n->value;
	return *key;
}

const char* ControlMgr::getKeyName(sf::Keyboard::Key key) {
	Node* n = NULL;

	while ((n = listIterate(this->keys, n)) != NULL) {
		sf::Keyboard::Key* keyV = (sf::Keyboard::Key*) n->value;		
		if (*keyV == key) { 
			return n->name;
		}
	}

	return names[0];
}