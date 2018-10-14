#ifndef KEY_EVENT_H
#define KEY_EVENT_H value

#include "../event.h"

#include <condition_variable>
#include <thread>


template <class T>
class KeyEvt;

// On Key Pressed
template <class T>
struct Pressed { 
	bool enable;
	void (T::*fnc)(KeyEvt<T>*); // Pointer To Object Function
};

// On Key Released
template <class T>
struct Released { 
	bool enable;
	void (T::*fnc)(KeyEvt<T>*); // Pointer To Object Function
};


class HoldAbstract { 
protected:
	void done();

public:
	HoldAbstract();
	~HoldAbstract();

	bool killed;
	bool enable;

	float min; 	// Hold Minimum Time For Call Function
	float max;	// Hold Maximum Time For Call Function
	
	float time;	// Actual Hold Time Before User Has Realeased
	double started;

	std::mutex mutex;				// Thread Mutex
	std::thread* delayed = NULL;			// Thread For Delayed Fnc
	std::condition_variable* cond = NULL;	// Thread condition wait/signal

	virtual void call() = 0;
	virtual void kill() = 0;
	virtual void start() = 0;
	virtual void signal() = 0;
};

void holdThread(HoldAbstract* hold);

// On Key Hold
template <class T>
class Hold : public HoldAbstract { 
	public:
		virtual ~Hold();
		Hold(T* obj, KeyEvt<T>* evt, void (T::*fnc)(KeyEvt<T>*), float min, float max);


		virtual void call();
		virtual void kill();
		virtual void start();
		virtual void signal();

		KeyEvt<T>* evt;
		void (T::*fnc)(KeyEvt<T>*); // Pointer To Object Function
};


class KeyEvtAbstract : public Evt {
	protected:
		Keyboard::Key key;
		bool released = true;


	public:
		sf::Event evt;
		Keyboard::Key getKey();
	
		bool breakEvt;
		bool allowRepeat = false;	// Is Repeated On Key Hold (/!\ Automatically Disable On Hold Function /!\)

		sf::Event getEvent();

		virtual void callPressed(sf::Event evt) = 0;
		virtual void callReleased(sf::Event evt) = 0;
};

template <class T>
class KeyEvt : public KeyEvtAbstract
{
	private:
		Hold<T>* onHold = NULL;
		Pressed<T>* onPress = NULL;
		Released<T>* onRelease = NULL;
		
	public:
		T* target;
		KeyEvt(const char* name, sf::Keyboard::Key key, T* obj);
		KeyEvt(const char* name, sf::Keyboard::Key key, T* obj, Json* conf);
		~KeyEvt();

		bool callHoldOnMax;	// Call Function Automatically Function If Hold Time Reaches Max

		//sf::Event::Keyboard::Key getKey();
		// template<typename T>
		void setOnHold(T* obj, void (T::*fnc)(KeyEvt<T>*), float min, float max);

		void setOnPress(T* obj, void (T::*fnc)(KeyEvt<T>*));
		void setOnRelease(T* obj, void (T::*fnc)(KeyEvt<T>*));

		//DelayedFncLauncher* delayFnc;

		void killOnHold();
		void callOnHold();
		vector getOnHoldTime();

		void callPressed(sf::Event evt);
		void callReleased(sf::Event evt);
};

#include "keyEvent.tpp"

#endif