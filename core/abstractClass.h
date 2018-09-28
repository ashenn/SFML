#ifndef ABSTRACT_CLASS
#define ABSTRACT_CLASS

#include "../common.h"
#include "../base/basic.h"

#include <pthread.h>

class AbstractClass
{
	protected:
		char* name = NULL;
		unsigned int id = 0;

	private:
		/** THREAD MANAGEMENT **/
		pthread_cond_t cond;
		pthread_mutex_t mutex;

		pid_t pid = -1;			// Thread ID Who has lock the class
		char* lockTag = NULL;	// Lock Tag Use For Debbugging
		char* unlockTag = NULL;	// UnLock Tag Use For Debbugging
			
	public:
		virtual ~AbstractClass();
		
		unsigned int getId();
		const char* getName();

		void wait();							// Thread Wait
		void signal();							// Thread Signal
		void wait_time(float delay);			// Thread Waite with Time Out

		bool lock(const char* tag);				// Lock Class
		void unlock(const char* tag, bool b);	// UnLock Class
};


#include <iostream>
#include <stdio.h>


class AbstractStaticClass : public AbstractClass {
	protected:
		void init();
		bool isInit = false;

	public:
		void operator=(AbstractStaticClass const&)  = delete;
};

#endif