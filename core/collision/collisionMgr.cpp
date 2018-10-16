#include "collisionMgr.h"



CollisionMgr::CollisionMgr() {
	STATIC_CLASS_CONSTRUCT(CollisionMgr)

	this->colObjs = initListMgr();
	this->chanels = initListMgr();

	this->loadChanels();
}

CollisionMgr::~CollisionMgr() {
	Log::inf(LOG_COL, "=== Deleting Collision Manager ===");

	deleteList(this->colObjs);
	deleteList(this->chanels);

	if (this->name != NULL) {
		free(this->name);
	}
}

void deleteChannel(Node* n) {
	if (n->value == NULL) {
		return;
	}

	ListManager* channel = (ListManager*) n->value;
	deleteList(channel);
}

void CollisionMgr::loadChanels() {
	AssetMgr* ast = AssetMgr::get();
	Json* colConf = ast->getConf("collision/collision");

	Log::inf(LOG_COL, "=== Loading Channels ===");
	ListManager* channels = (ListManager*) jsonGetValue(colConf, "channels", NULL);

	Node* n = NULL;
	while ((n = listIterate(channels, n)) != NULL) {
		Json* chan = (Json*) n->value;
		Log::dbg(LOG_COL, "-- Configure: '%s'", chan->key);

		ListManager* channel = initListMgr();

		this->loadChanel(channel, chan);

		Node* chanN = addNodeV(this->chanels, chan->key, channel, true);
		chanN->del = deleteChannel;
	}
}

void CollisionMgr::loadChanel(ListManager* channel, Json* conf) {
	Node* n = NULL;
	ListManager* chanVals = conf->childs;

	while ((n = listIterate(chanVals, n)) != NULL) {
		Json* colConf = (Json*) n->value;
		char* typeName = (char*) jsonGetValue(conf, colConf->key, NULL);
		unsigned int* type = (unsigned int*) malloc(sizeof(unsigned int));

		*type = colTypeValue(typeName);
		Log::dbg(LOG_COL, "-- Value: '%u'", *type);
		
		addNodeV(channel, colConf->key, type, true);
		free(typeName);
	}
}

void CollisionMgr::handle() {
	Node* objN = NULL;
	Log::inf(LOG_COL_LOOP, "=== Handeling Collision ===");

	while((objN = listIterate(this->colObjs, objN)) != NULL) {
		Log::dbg(LOG_COL_LOOP, "############################");
		
		Object* obj = (Object*) objN->value;
		if (obj == NULL) {
			Log::dbg(LOG_COL_LOOP, "-- Empty Node: %s", objN->name);
			Node* tmpN = objN->prev;
			removeAndFreeNode(this->colObjs, objN);
			objN = tmpN;
			continue;
		}

		Log::dbg(LOG_COL_LOOP, "++++ Checking #%d : %s", obj->getId(), obj->getName());

		if (!obj->isEnabled()) {
			Log::dbg(LOG_COL_LOOP, "-- Object Is Disabled");
			continue;
		}
		else if(!obj->getCollisions()->nodeCount) {
			Log::dbg(LOG_COL_LOOP, "-- Object Has No Collision");
			continue;
		}

		this->loopColObjects(obj, objN);
	}
}

void CollisionMgr::addObject(Object* obj) {
	addNodeUniqValue(this->colObjs, obj->getName(), obj, false);
}

void CollisionMgr::loopColObjects(Object* obj, Node* objN) {
	Node* obj2N = objN;

	while((obj2N = listIterate(this->colObjs, obj2N)) != NULL) {
		Object* obj2 = (Object*) obj2N->value;

		if (obj2 == NULL) {
			Node* tmpN = obj2N->prev;
			removeAndFreeNode(this->colObjs, obj2N);
			obj2N = tmpN;
			continue;
		}

		Log::dbg(LOG_COL_LOOP, "++++ Comparing #%d : %s", obj2->getId(), obj2->getName());

		if (!obj2->isEnabled()) {
			Log::dbg(LOG_COL_LOOP, "-- Object Is Disabled");
			continue;
		}
		else if(!obj2->getCollisions()->nodeCount) {
			Log::dbg(LOG_COL_LOOP, "-- Object Has No Collision");
			continue;
		}

		this->loopObjectCollisions(obj, obj2);
	}
}

void CollisionMgr::loopObjectCollisions(Object* obj, Object* obj2) {
	Node* colN = NULL;
	ListManager* cols = obj->getCollisions();

	while((colN = listIterate(cols, colN))) {
		Collision* col = (Collision*) colN->value;
		if (col == NULL) {
			continue;
		}

		Log::dbg(LOG_COL_LOOP, "++++ Collision: %s", col->getName());
		if (!col->isEnabled() || col->getFlag() == COL_NONE) {
			Log::dbg(LOG_COL_LOOP, "-- Collision Is Disabled");
			continue;
		}
		
		this->loopCollisions(col, obj2);
	}
}

void CollisionMgr::loopCollisions(Collision* col, Object* obj2) {
	Node* col2N = NULL;
	ListManager* cols2 = obj2->getCollisions();


	while((col2N = listIterate(cols2, col2N))) {
		Collision* col2 = (Collision*) col2N->value;
		if (col2 == NULL) {
			continue;
		}

		Log::dbg(LOG_COL_LOOP, "++++ Collision: %s", col2->getName());
		if (!col2->isEnabled() || col2->getFlag() == COL_NONE) {
			Log::dbg(LOG_COL_LOOP, "-- Collision Is Disabled");
			continue;
		}

		Log::inf(LOG_COL_LOOP, "Check Collides: %s | %s", col->getName(), col2->getName());		
		
		ColType type = col->collides(col2);		
		if (type != COL_IGNORE) {
			this->applyCollisions(col, col2, type);
		}
	}
}

void CollisionMgr::applyCollisions(Collision* col, Collision* col2, ColType type) {
	// Log::war(LOG_COL, "=== APPLY COL' ===");
	if (type == COL_BLOCK) {
		col->callHit(col2);
	}
}

ListManager* CollisionMgr::getChannels() {
	return this->chanels;
}

Collision** CollisionMgr::searchCollision(Object* refObj, ColType type, vector move) {
	Node* objN = NULL;

	while ((objN = listIterate(this->colObjs, objN)) != NULL) {

		Object* obj = (Object*) objN->value;
		if (!obj->isEnabled()) {
			continue;
		}
		else if (obj == refObj) {
			continue;
		}


		Node* refN = NULL;
		while ((refN = listIterate(refObj->getCollisions(), refN)) != NULL) {
			Collision* ref = (Collision*) refN->value;
			if (!ref->isEnabled() || ref->getFlag() == COL_NONE) {
				continue;
			}

			Node* colN = NULL;
			while ((colN = listIterate(obj->getCollisions(), colN)) != NULL) {
				Collision* col = (Collision*) colN->value;
				if (!col->isEnabled()) {
					continue;
				}
				else if(col->getFlag() == COL_NONE) {
					continue;
				}

				ColType colType = ref->collides(col, move);
				if (colType == COL_IGNORE) {
					continue;
				}
				else if((type != COL_IGNORE) && (type != colType)) {
					continue;
				}

				Collision** cols = (Collision**) malloc(sizeof(Collision*) * 2);
				cols[0] = ref;
				cols[1] = col;

				return cols;
			}
		}
	}

	return NULL;
}