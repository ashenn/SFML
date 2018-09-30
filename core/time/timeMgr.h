#ifndef TIME_MGR_H
#define TIME_MGR_H

#include "../abstractClass.h"

class TimeMgr 
{
    private:
        TimeMgr() {}

    public:
        STATIC_CLASS_BODY(TimeMgr)

        double startAt = 0;
        double updateAt = 0;

        double update1 = 0;
        double update2 = 0;

        double getTime();


        void start();
        void update();

        double getStartedTime() const;
        double getElapsedTime() const;
};

#endif // TIME_MANAGER_H
