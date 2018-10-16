#ifndef COLLISION_H
#define COLLISION_H

#include "../../common.h"
#include "../../base/math.h"
#include "../abstractClass.h"
#include "../asset/asset.h"
using namespace sf;

#define COL_TYPES(TYPE) \
 	TYPE(COL_IGNORE) 	\
 	TYPE(COL_BLOCK)		\
 	TYPE(COL_OVERLAP)

 #define GEN_COL_TYPE_ENUM(ENUM) ENUM,
 #define GEN_COL_TYPE_STRING(STRING) #STRING,

#define COL_CHANELS(TYPE) \
	TYPE(COL_NONE)		\
	TYPE(COL_ALL) 		\
	TYPE(COL_WALL) 		\
	TYPE(COL_ITEM)		\
	TYPE(COL_PLAYER)	\
	TYPE(COL_MONSTER)	\
	TYPE(COL_PLATFORM)

#define GEN_COL_CHANEL_ENUM(ENUM) ENUM,
#define GEN_COL_CHANEL_STRING(STRING) #STRING,


class Object;
typedef enum ColType {
	COL_TYPES(GEN_COL_TYPE_ENUM)
} ColType;

typedef enum ColChanel {
	COL_CHANELS(GEN_COL_CHANEL_ENUM)
} ColChanel;


const char* colTypeName(ColType v);
ColType colTypeValue(const char* name);

const char* colChanelName(ColChanel v);
ColChanel colChanelValue(const char* name);


class Collision;
class CollisionCallBackAbstract {
	public:
		virtual bool call(Collision* col, Collision* col2) = 0;
		virtual ~CollisionCallBackAbstract() {}
};

template <class T>
class CollisionCallBack : public CollisionCallBackAbstract {
	private:
		T* target;
		bool (T::*fnc)(Collision*, Collision*);

	public:
		~CollisionCallBack() {}
		virtual bool call(Collision* col, Collision* col2);

		CollisionCallBack(T* obj, bool (T::*fnc)(Collision*, Collision*)) {
			this->target = obj;
			this->fnc = fnc;
		}
};

class Collision : public AbstractClass
{
	private:
		Object* obj;
		IntRect pos;
		bool enabled = true;

		CollisionCallBackAbstract* onHit = NULL;
		CollisionCallBackAbstract* onOverlap = NULL;
	
		unsigned int flag;
		unsigned short hitFlags = 0;
		unsigned short overlapFlags = 0;
		Collision(const char* name, Object* obj, IntRect pos);

		// void initFlags();

	public:
		Object* colObj = NULL;
		bool isContinuous = false;
		bool callHit(Collision* col2);
		bool callOverlap(Collision* col2);

		const unsigned int getFlag() const;

		template <class T>
		void setHit(T* target, bool (T::*fnc)(Collision*, Collision*));

		template <class T>
		void setOverlap(T* target, bool (T::*fnc)(Collision*, Collision*));

		~Collision();
		Collision(const char* name, Object* obj, IntRect pos, ColChanel chanel);

		IntRect getPosition()  const;
		IntRect getWorldPosition()  const;

		ColType collides(const Collision* col) const;
		ColType collides(Collision* col, vector move);

		bool operator&&(const Collision& col) const;
		bool operator&&(const Collision* col) const;

		bool isEnabled();
		void toggle(bool b);

		bool isLeft(Collision* col2);
		bool isLeft(Collision* col2, vector move);

		bool isRight(Collision* col2);
		bool isRight(Collision* col2, vector move);

		bool isOver(Collision* col2);
		bool isOver(Collision* col2, vector move);

		bool isUnder(Collision* col2);
		bool isUnder(Collision* col2, vector move);
};

#include "collision.tpp"

// class CollisionMgr : public AbstractStaticClass {
// 	private: 
// 		CollisionMgr();
// 		~CollisionMgr();
		
// 		static ListManager* colObjs;
// 		static ListManager* chanels;

// 		static void loadChanels();
// 		static void loadChanel(ListManager* channel, Json* conf);

// 		static void checkCollisions(Collision* col, Collision* col2);
// 		static void loopCollisions(Collision* col, Object* obj2);
// 		static void loopObjectCollisions(Object* obj, Object* obj2);
// 		static void loopColObjects(Object* obj, Node* objN);

// 	public:
// 		STATIC_CLASS_BODY(CollisionMgr)

// 		static void handle();
// 		static ListManager* getChannels();
// 		static void addObject(Object* obj);
// };

// #include "../object/object.h"

#endif