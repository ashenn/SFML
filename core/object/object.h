#ifndef OBJECT_H
#define OBJECT_H

#include "../../base/math.h"
#include "../abstractClass.h"
#include "../../base/libList.h"

using namespace sf;

class Object : public AbstractClass
{
	private:
		int lifetime = -1;
		unsigned short z = 0;

		bool visible = true;
		bool enabled = true;

		vector pos;
		Node* node = NULL;
		Object* parent = NULL;

		Sprite* sprite = NULL;
		Texture* texture = NULL;

	protected:
		IntRect clip;

	public:
		Object(const char* name, vector* pos, Texture* text, IntRect* clip, unsigned short z, bool visible);
		
		Object(const char* name) : Object(name, NULL, NULL, NULL, 0, true){};
		Object(const char* name, vector* pos, Texture* text, IntRect* clip) : Object(name, pos, text, clip, 0, true){};
		Object(const char* name, vector* pos, Texture* text, IntRect* clip, unsigned short z) : Object(name, pos, text, clip, z, true){};

		~Object();

		Sprite* getSprite();
		void removeTexture();
		void setTexture(Texture* text);

		void addToView();
		void removeFromView();

		vector getPosition();
		void setPosition(vector pos);

		vector move(vector move);


		void draw(RenderWindow* window);
};


#endif