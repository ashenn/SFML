#ifndef OBJECT_H
#define OBJECT_H

#include "../../base/math.h"
#include "../abstractClass.h"
#include "../../base/libList.h"
#include "../movement/movement.h"

using namespace sf;

// Visual Object
class Object : public AbstractClass
{
	private:
		int lifetime = -1;			// Life Time In Seconds (Illimited If Negatif);
		unsigned short z = 0;		// Z-Index

		bool visible = true;
		bool enabled = true;		// Enable for Animation / Render / Collision

		vector pos;					// position {x, y}
		Movement* movement = NULL;

		Node* node = NULL;			// Node Pointer In All Objects List 
		Object* parent = NULL;		// Parent Object

		IntRect* clip = NULL;		// Texture Clip
		Sprite* sprite = NULL;
		Texture* texture = NULL;

		static ListManager* objectList;				// All Objects Instanciated
		static void addObject(Object* obj);			// Add Object To List
		static void removeObject(Object* obj);		// Remove Object To List

	protected:
		void updateClip();	// Update Sprite Clip

	public:
		Object(const char* name, vector* pos, Texture* text, IntRect* clip, unsigned short z, bool visible);
		
		Object(const char* name) : Object(name, NULL, NULL, NULL, 0, true){};
		Object(const char* name, vector* pos, Texture* text, IntRect* clip) : Object(name, pos, text, clip, 0, true){};
		Object(const char* name, vector* pos, Texture* text, IntRect* clip, unsigned short z) : Object(name, pos, text, clip, z, true){};

		~Object();

		void flipH();

		Sprite* getSprite();
		void removeTexture();				// Delete Texture And Sprite
		void setTexture(Texture* text);		// Set Texture And Update Sprite And Clip

		void addToView();					// Add Object To Render Loop
		void removeFromView();				// Remove Object To Render Loop

		vector getPosition();
		void setPosition(vector pos);

		vector getVelocity();				// Move Object Vector 
		vector move(vector move);			// Move Object Vector 
		Movement* getMovement();			// Move Object Vector 
		virtual void draw(RenderWindow* window);

		void removeClip(bool clean);		// Set Clip Pointer To Null And Delete Object if Clean = true
		void setClip(IntRect* clip, bool clean);

		static void clearObjects();			// Delete All Objects
};



#endif