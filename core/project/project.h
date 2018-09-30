#ifndef PROJECT_H
#define PROJECT_H

#include "../abstractClass.h"
#include "../../base/logger.h"

#define PROJECT_STATES(STATE) \
	STATE(PRO_NULL) \
	STATE(PRO_INIT) \
	STATE(PRO_START) \
	STATE(PRO_END) \
	STATE(PRO_CLOSE) \

#define GEN_STATE_ENUM(ENUM) ENUM,
#define GEN_STATE_NAMES(STRING) #STRING,


typedef enum ProjectState {
	PROJECT_STATES(GEN_STATE_ENUM)
} ProjectState;



class Project : public AbstractStaticClass
{
	private:
	    Project(){}
	    ~Project();
		unsigned int flags = 0;
		ProjectState status = PRO_NULL;

		ListManager* flagList = NULL;

		void initFlags();
		void setArgs(int argc, char* argv[]);

		pthread_t renderTh;
	
	public:
		static const char* getStatusName(ProjectState status);

		void operator=(Project const&)  = delete;

		void close();
		void init(int argc, char* argv[]);

		ProjectState getStatus();
		void changeStatus(ProjectState state);

		unsigned int getFlags();
		void addFlag(unsigned int f);

		bool enableFlag(const char* f);
		bool flagActive(unsigned int f);

		void runRenderTh();


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