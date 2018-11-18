#ifndef RENDER_H
#define RENDER_H

#include "../object/object.h"
#include "../project/project.h"

class Render : public AbstractStaticClass
{
	private:
		Render();
		~Render();

		bool started = false;
		bool running = false;

		ListManager* layers = NULL;		// Objects Layers
		ListManager* objectList = NULL;		// Objects To Render

		std::thread th;			// Render Thread
		
		void render();		// Print Object To Window
		void renderThread();

	public:
		// Singleton Initialization
		STATIC_CLASS_BODY(Render)
		RenderWindow* window = NULL;

		void init(RenderWindow* window);
		RenderWindow* getWindow();

		void start();
		void close();


		void setView(sf::View* v);
		const sf::View& getView();

		ListManager* getObjectList();
		Node* addObject(Object* obj);	// Add Object To Render List

		ListManager* getLayer(int z);
		void addToLayer(Object* obj);
		void removeFromLayer(Object* obj);
};


#endif