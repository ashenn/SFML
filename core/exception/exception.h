#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "../../base/basic.h"
#include "../../base/logger.h"
#include <exception>

class Exception : public std::exception
{
	private:
		char* msg = NULL;
		unsigned short code = 0;

	public:
		Exception(unsigned short code, const char* msg) throw();
		~Exception();

		virtual const char* what() const throw();

		const char* getMsg(); 
		const unsigned short getCode();
};

#endif