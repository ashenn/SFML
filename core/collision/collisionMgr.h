#ifndef COLLISION_MGR_H
#define COLLISION_MGR_H

#include "collision.h"

class CollisionMgr : public AbstractStaticClass {
	private: 
		CollisionMgr();
		~CollisionMgr();
		
		ListManager* colObjs;
		ListManager* chanels;

		void loadChanels();
		void loadChanel(ListManager* channel, Json* conf);

		void loopColObjects(Object* obj, Node* objN);
		void loopCollisions(Collision* col, Object* obj2);
		void loopObjectCollisions(Object* obj, Object* obj2);
		
		void applyCollisions(Collision* col, Collision* col2, ColType type);


	public:
		STATIC_CLASS_BODY(CollisionMgr)

		void handle();
		ListManager* getChannels();
		void addObject(Object* obj);

		Collision** searchCollision(Object* ref, ColType type, vector move);
};

#include "../object/object.h"


#endif