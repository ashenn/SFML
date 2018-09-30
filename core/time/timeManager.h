#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include "../abstractClass.h"

class TimeManager 
{
    private:
        TimeManager() {}

    public:
        double startAt = 0;
        double updateAt = 0;

        double update1 = 0;
        double update2 = 0;

        double getTime();


        void start();
        void update();

        double getStartedTime() const;
        double getElapsedTime() const;

        void operator=(TimeManager const&)  = delete;

        static TimeManager* get(bool deleteInst = false) {
            static TimeManager* instance = NULL;

            if (deleteInst) {
                if (instance != NULL) {
                    delete instance;
                }

                return NULL;
            }

            if (instance != NULL) {
                return instance;
            }

            instance = new TimeManager();

            return instance;
        }
};

#endif // TIME_MANAGER_H
