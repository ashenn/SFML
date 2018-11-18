#ifndef GARBAGE_H
#define GARBAGE_H

#include <set>
#include "../abstractClass.h"

class Garbage : public AbstractStaticClass
{
	Garbage();
	~Garbage();
	
	// std::vector<AbstractClass*> content;
	std::set<AbstractClass*> content;

	public:
		STATIC_CLASS_BODY(Garbage)
	
		bool isEmpty();
		unsigned int size();
		unsigned int count();

		void clean();
		void clear();
		void empty();

		void add(AbstractClass* obj);
};

#endif