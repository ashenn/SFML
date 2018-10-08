#ifndef RENDER_H
#define RENDER_H

#include "../object/object.h"
#include "../project/project.h"

class Render : public AbstractStaticClass
{
	private:
		Render();
		~Render();

		ListManager* objectList = NULL;		// Objects To Render

	public:
		// Singleton Initialization
		STATIC_CLASS_BODY(Render)
		RenderWindow* window = NULL;

		void init(RenderWindow* window);
		RenderWindow* getWindow();

		void render();		// Print Object To Window
		void close();

		void setView(sf::View* v);
		const sf::View& getView();

		ListManager* getObjectList();
		Node* addObject(Object* obj);	// Add Object To Render List
};

void* renderThread(void* param);

#endif