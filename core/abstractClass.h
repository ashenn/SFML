#ifndef ABSTRACT_CLASS
#define ABSTRACT_CLASS

#include "../common.h"
#include "../base/basic.h"


#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <thread>
#include <map>

void deleteCallable(Node* n);

template <class T>
class KeyEvt;


class AbstractClass;
class CallableFncAbstract {
	public:
		virtual ~CallableFncAbstract(){};
};

template <class T> 
class CallableFncEvt : public CallableFncAbstract {
	public:
		virtual ~CallableFncEvt(){}
		void (T::*fnc)(KeyEvt<T>* param);

		CallableFncEvt(void (T::*fnc)(KeyEvt<T>* param)){
			this->fnc = fnc;
		}
};

class Collision;
template <class T> 
class CallableFncCol : public CallableFncAbstract {
	public:
		virtual ~CallableFncCol(){}
		bool (T::*fnc)(Collision*, Collision*);

		CallableFncCol(bool (T::*fnc)(Collision*, Collision*)){
			this->fnc = fnc;
		}
};


class AbstractClass
{
	protected:
		char* name = NULL;
		ListManager* callables = NULL;

	private:

		/** THREAD MANAGEMENT **/
		double pid = -1;			// Thread ID Who has lock the class
		char* lockTag = NULL;		// Lock Tag Use For Debbugging
		char* unlockTag = NULL;		// UnLock Tag Use For Debbugging

		pthread_cond_t cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
		pthread_mutex_t mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
		
		void setLockTag(const char* tag);		// Used For Thread Lock Warning
		void setUnlockTag(const char* tag);		// Used For Thread Lock Warning

	public:
		unsigned int id = 0;
		// void callFnc(const char* name, void* param);
		CallableFncAbstract* getFnc(const char* name);

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

/*** MACRO TO CREATE SINGLETON CLASS ***/
	// To Add in Public Section
#define STATIC_CLASS_BODY(CLASS) 					\
	static CLASS* get(bool deleteInst = false) {	\
		static CLASS* instance = NULL;				\
													\
		if (deleteInst) {							\
			if (instance != NULL) {					\
				delete instance;					\
			}										\
													\
			return NULL;							\
		}											\
													\
		if (instance != NULL) {						\
			return instance;						\
		}											\
													\
		instance = new CLASS();						\
													\
		return instance;							\
	}												\
													\
	void operator=(CLASS const&)  = delete; 		\


/*** MACRO TO CREATE SINGLETON CONSTRUCTOR ***/
	// To Add in constructor definition
#define STATIC_CLASS_CONSTRUCT(CLASS)	(this->name = Str(#CLASS));

class AbstractStaticClass : public AbstractClass {
	protected:
		bool isInit = false;

	public:
		void operator=(AbstractStaticClass const&)  = delete;
};

#endif