#include "exception.h"

Exception::Exception(unsigned short code, const char* msg) throw() {
	this->code = code;
	this->msg = Str(msg);
}

Exception::~Exception() {
	if (this->msg != NULL) {
		free(msg);
	}
}

const char* Exception::getMsg() {
	return this->msg;
}

const unsigned short Exception::getCode() {
	return this->code;
}

const char* Exception::what() const throw() {
	return this->msg;
}