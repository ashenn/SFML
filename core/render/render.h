#ifndef RENDER_H
#define RENDER_H

#include "../object/object.h"
#include "../project/project.h"

class Render : public AbstractStaticClass
{
	private:
		Render();
		~Render();

		RenderWindow* window = NULL;
		ListManager* objectList = NULL;

	public:
		STATIC_CLASS_BODY(Render)

		void init(RenderWindow* window);
		RenderWindow* getWindow();

		void render();
		void close();

		ListManager* getObjectList();
		Node* addObject(Object* obj);
};

void* renderThread(void* param);

#endif