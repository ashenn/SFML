#ifndef OBJECT_H
#define OBJECT_H

#include "../../base/math.h"
#include "../abstractClass.h"
#include "../../base/libList.h"
#include "../movement/movement.h"
#include "../collision/collision.h"

using namespace sf;

// Visual Object
class Object : public AbstractClass
{
	private:
		int lifetime = -1;			// Life Time In Seconds (Illimited If Negatif);
		unsigned short z = 0;		// Z-Index

		bool visible = true;
		bool enabled = true;		// Enable for Animation / Render / Collision
		
		//IntRect defaultClip;		// Texture Clip

		vector pos;					// position {x, y}
		bool flipped;
		Movement* movement = NULL;

		Node* node = NULL;			// Node Pointer In All Objects List 
		Object* parent = NULL;		// Parent Object


		Texture* texture = NULL;

		ListManager* collisions = NULL;

		static ListManager* objectList;				// All Objects Instanciated
		static void addObject(Object* obj);			// Add Object To List
		static void removeObject(Object* obj);		// Remove Object To List

	protected:
		ListManager* childs = NULL;
		IntRect* clip = NULL;		// Texture Clip
		Sprite* sprite = NULL;
		void updateClip();	// Update Sprite Clip
		bool addChildToList(Object* child);
		bool checkCameraDistance();
		
	public:
		Object(const char* name, vector* pos, Texture* text, IntRect* clip, unsigned short z, bool visible);
		
		Object(const char* name) : Object(name, NULL, NULL, NULL, 0, true){};
		Object(const char* name, vector* pos) : Object(name, pos, NULL, NULL, 0, true){};
		Object(const char* name, vector* pos, unsigned short z) : Object(name, pos, NULL, NULL, z, true){};
		Object(const char* name, vector* pos, Texture* text, IntRect* clip) : Object(name, pos, text, clip, 0, true){};
		Object(const char* name, vector* pos, Texture* text, IntRect* clip, unsigned short z) : Object(name, pos, text, clip, z, true){};

		bool gravity = false;
		sf::Vector2f getOrigin();

		~Object();

		ListManager* layer = NULL;			// Node Pointer In All Objects List 
		unsigned short getZ();
		void setZ(unsigned short z);

		void setMaxSpeed(unsigned int x, unsigned int y);
		void flipH();
		bool isFlipped();

		bool isEnabled();
		void toggle(bool b);

		void setScale(vector s);
		void scale(vector s);

		Sprite* getSprite();
		void removeTexture();				// Delete Texture And Sprite
		void setTexture(Texture* text);		// Set Texture And Update Sprite And Clip

		void addToView();					// Add Object To Render Loop
		void removeFromView();				// Remove Object To Render Loop

		vector getPosition();
		vector getRelativePosition();
		void setPosition(vector pos);

		vector getVelocity();				// Move Object Vector 

		void stopMove();			// Move Object Vector 
		virtual vector canMove(vector m);
		vector move(vector move);			// Move Object Vector 

		Movement* getMovement();			// Move Object Vector 
		virtual void draw(RenderWindow* window, bool grav);

		// IntRect getClip();
		void removeClip(bool clean);		// Set Clip Pointer To Null And Delete Object if Clean = true
		void setClip(IntRect* clip, bool clean);

		static void clearObjects();			// Delete All Objects

		Collision* addCollision(const char* name, IntRect pos, ColChanel chan);

		Object* getParent();
		void removeParent();
		void setParent(Object* parent);


		Object* getChild(const char* name);
		bool addChild(Object* child, bool keepWorldPosition);
		bool addChild(Object* child, vector pos, bool worldPosition);

		bool removeChild(Object* child, bool delChild);
		bool removeChild(bool delChild);

		ListManager* getCollisions();
		Collision* getCollision(const char* name);
};



#endif