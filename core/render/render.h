#ifndef RENDER_H
#define RENDER_H

#include "../../base/libList.h"
#include "../object/object.h"



class Render : public AbstractStaticClass
{
	private:
		Render();
		~Render();

		ListManager* objectList = NULL;

	public:
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

		ListManager* getObjectList();
		void addObject(Object* obj);
};

#endif