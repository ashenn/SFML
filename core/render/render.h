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
		void init(RenderWindow* window);
		RenderWindow* getWindow();

		static Render* get(bool deleteInst = false) {
			static Render* instance = NULL;

			if (deleteInst) {
				if (instance != NULL) {
					delete instance;
				}

				return NULL;
			}

			if (instance != NULL) {
				return instance;
			}

			instance = new Render();

			return instance;
		}

		void render();
		void close();

		ListManager* getObjectList();
		Node* addObject(Object* obj);
};

void* renderThread(void* param);

#endif