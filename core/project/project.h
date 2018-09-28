#ifndef PROJECT_H
#define PROJECT_H

#include "../abstractClass.h"
#include "../../base/logger.h"

#define LOG_PROJECT 1 << 1

#define PROJECT_STATES(STATE) \
	STATE(PRO_NULL) \
	STATE(PRO_INIT) \
	STATE(PRO_START) \
	STATE(PRO_END) \
	STATE(PRO_CLOSE) \

#define GEN_STATE_ENUM(ENUM) ENUM,
#define GEN_STATE_STRING(STRING) #STRING,


typedef enum ProjectState {
	PROJECT_STATES(GEN_STATE_ENUM)
} ProjectState;


static const char* GEN_STATE_STRING[] = {
    PROJECT_STATES(GEN_STATE_STRING)
};



class Project : public AbstractStaticClass
{
	private:
	    Project(){}
	    ~Project();
		unsigned int flags = 0;
		unsigned int status = PRO_NULL;

		ListManager* flagList = NULL;

		void initFlags();
		void setArgs(int argc, char* argv[]);

	
	public:		
		//Project(Project const&)    		= delete;
		void operator=(Project const&)  = delete;

		void close();
		void addFlag(unsigned int f);
		void init(int argc, char* argv[]);


		unsigned int getFlags();
		unsigned int getStatus();
		bool enableFlag(const char* f);
		void changeStatus(ProjectState state);
		bool flagActive(unsigned int f);

		static Project* get(bool deleteInst = false) {
			static Project* instance = NULL;

			if (deleteInst) {
				if (instance != NULL) {
					delete instance;
				}

				return NULL;
			}

			if (instance != NULL) {
				return instance;
			}

			instance = new Project();

			return instance;
		}
};

#endif