#ifndef VIEW_MGR_H
#define VIEW_MGR_H

#include "../character/character.h"

typedef enum ViewType {
	VIEW_MAP,
	VIEW_CAMERA
} ViewType;

typedef enum ViewBoundType {
	VIEW_BOUND_FREE,
	VIEW_BOUND_FLEX,
	VIEW_BOUND_FIXED
} ViewBoundType;

class ViewMgr : public AbstractClass
{
	private:
		ViewType type;
		Character* ch;
		sf::View* view;
		ViewBoundType boundType;

		vector bound;
		vector boundLimit;

		//static ListManager* viewList;

	public:
		ViewMgr(Character* ch, ViewType type, sf::FloatRect pos, ViewBoundType boundType);
		ViewMgr(Character* ch, ViewType type, sf::FloatRect pos) : ViewMgr(ch, type, pos, VIEW_BOUND_FIXED){};
		~ViewMgr();

		void setBound(vector b);
		void setBoundLimit(vector bl);

		ViewType getType();
		ViewBoundType getBoundType();

		void update();	
};

#endif