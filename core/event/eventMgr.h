#ifndef EVENT_MGR_H
#define EVENT_MGR_H

#include "key/keyEvent.h"

void deleteEvent(Node* n);

class EventMgr : public AbstractStaticClass
{
	private:
		EventMgr();
		~EventMgr();
		RenderWindow* window = NULL;
	
		ListManager* keyEvts;
		ListManager* inputEvts;

	public:
		STATIC_CLASS_BODY(EventMgr)

		void handle();
		void handleKey(Event* event);
		void init(RenderWindow* window);

		template <class T>
		KeyEvt<T>* bindKeyEvent(const char* name, sf::Keyboard::Key key, T* obj);

		template <class T>
		KeyEvt<T>* bindKeyEvent(const char* name, sf::Keyboard::Key key, T* obj, Json* conf);
		
		template <class T>
		KeyEvt<T>* bindKeyEvent(const char* name, sf::Keyboard::Key key, T* obj, void (T::*fnc)(KeyEvt<T>*));
};

#include "eventMgr.tpp"

#endif