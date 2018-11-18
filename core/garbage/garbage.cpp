#include "garbage.h"

Garbage::Garbage() {
	STATIC_CLASS_CONSTRUCT(Garbage)
}

Garbage::~Garbage() {
	this->clear();
}

void Garbage::add(AbstractClass* obj) {
	this->content.insert(obj);
}

void Garbage::clear() {
	for (auto&& obj : this->content) {
		delete obj;
	}

	this->content.clear();
}

void Garbage::clean() {
	this->clear();
}

void Garbage::empty() {
	this->clear();
}

bool Garbage::isEmpty() {
	return this->content.empty();
}

unsigned int Garbage::size() {
	return (unsigned int) this->content.size();
}

unsigned int Garbage::count() {
	return this->size();
}