#ifndef ABSTRACT_CLASS
#define ABSTRACT_CLASS

#include "../common.h"
#include "../base/basic.h"


#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <thread>

class AbstractClass
{
	protected:
		char* name = NULL;

	private:
		/** THREAD MANAGEMENT **/

		double pid = -1;			// Thread ID Who has lock the class
		char* lockTag = NULL;	// Lock Tag Use For Debbugging
		char* unlockTag = NULL;	// UnLock Tag Use For Debbugging

		pthread_cond_t cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
		pthread_mutex_t mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
		
		void setLockTag(const char* tag);
		void setUnlockTag(const char* tag);

	public:
		unsigned int id = 0;

		virtual ~AbstractClass();
		
		unsigned int getId();
		virtual const char* getName();

		void wait();							// Thread Wait
		void signal();							// Thread Signal
		void wait_time(float delay);			// Thread Waite with Time Out

		bool lock(const char* tag);				// Lock Class
		void unlock(const char* tag, bool b);	// UnLock Class

		double getThreadId();
};



class AbstractStaticClass : public AbstractClass {
	protected:
		bool isInit = false;

	public:
		void operator=(AbstractStaticClass const&)  = delete;
};

#endif