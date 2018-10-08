#ifndef PROJECT_H
#define PROJECT_H

#include <thread>
#include "../abstractClass.h"
#include "../../base/logger.h"

// Macro To Create Enum And Array Of Enum Names 
	#define PROJECT_STATES(STATE) \
		STATE(PRO_NULL) \
		STATE(PRO_INIT) \
		STATE(PRO_START) \
		STATE(PRO_END) \
		STATE(PRO_CLOSE) \

	#define GEN_STATE_ENUM(ENUM) ENUM,
	#define GEN_STATE_NAMES(STRING) #STRING,
// ---

// Project States
typedef enum ProjectState {
	PROJECT_STATES(GEN_STATE_ENUM)
} ProjectState;



class Project : public AbstractStaticClass
{
	private:
	    Project();
	    ~Project();

		unsigned int flags = 0;				// Flags enabled by Program Arguments
		ProjectState status = PRO_NULL;		// Current State

		ListManager* flagList = NULL;		// List Of Available Program Arguments

		void initFlags();						// Fill Program Arguments Flags
		void setArgs(int argc, char* argv[]);	// Enagle Flags From Program Arguments

		// pthread_t renderTh;			// Render Thread (/!\ Un-used: SFML window create massive memory leaks in other thread than main /!\)
		// bool rendering = false;		// Is Render Thread Running
	
	public:
		// Singleton Initialization
		STATIC_CLASS_BODY(Project)


		void close();
		void init(int argc, char* argv[]);

		ProjectState getStatus();
		void changeStatus(ProjectState state);

		unsigned int getFlags();		// Get Active Flags

		bool enableFlag(const char* f);		// Enagle Flag
		bool flagActive(unsigned int f);	// Is Flag active

		//void runRenderTh();					// Launch Render Thread (/!\ Un-used: SFML window create massive memory leaks in other thread than main /!\)

		static const char* getStatusName(ProjectState status);	// Get Project State Enum Name
};

#endif