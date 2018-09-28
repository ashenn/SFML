#include "project.h"

Project::~Project() {
	deleteList(flagList);
}

void Project::init(int argc, char* argv[]) {
	if (isInit) {
		return;
	}

	Log::inf(LOG_JSON, "=== Init Project ===");

	status = PRO_INIT;
	name = Str("Project");
	initFlags();
	
	setArgs(argc, argv);
}

void Project::addFlag(unsigned int f) {
	flags = flags | f;
}

bool Project::flagActive(unsigned int f) {
	return flags & f;
}

bool Project::enableFlag(const char* f) {
	Log::inf(LOG_PROJECT, "==== setting Flag: %s ====", f);

	Node* n = getNodeByName(flagList, f);

	if(n == NULL) {
		Log::err(LOG_PROJECT, "Faild To Found Flag: %s", f);
		return false;
	}

	int value = *((int*)n->value);
	Log::enableTag(value);
	flags = flags | value;

	return true;
}

unsigned int Project::getFlags() {
	return flags;
}

unsigned int Project::getStatus() {
	return status;
}

void Project::initFlags() {
	flagList = initListMgr();

	static unsigned int flags[20] = {
	    LOG_NONE,
		LOG_JSON,
		LOG_MAIN,
		LOG_PROJECT/*,
		LOG_OBJ,
		LOG_ASSET,
		LOG_VIEW,
		LOG_LAYER,
		LOG_EVENT,
		LOG_COMMON,
		LOG_RENDER,
		LOG_ANIM,
		LOG_TIMER,
		LOG_SPRITE,
		LOG_CHAR,
		LOG_CONTROL,
		LOG_CONTROLLER,
		LOG_MOVE,
		LOG_COLLISION*/
	};


	addNodeV(flagList, "none", &flags[0], 0);
	Log::addTag(flags[0], "none", 0);

	addNodeV(flagList, "json", &flags[1], 0);
	Log::addTag(flags[1], "json", 0);

	addNodeV(flagList, "main", &flags[2], 0);
	Log::addTag(flags[2], "main", 0);

	/*
		addNodeV(flagList, "obj", &flags[3], 0);
		Log::addTag(flags[3], "obj", 0);

		addNodeV(flagList, "asset", &flags[4], 0);
		Log::addTag(flags[4], "asset", 0);

		addNodeV(flagList, "view", &flags[5], 0);
		Log::addTag(flags[5], "view", 0);

		addNodeV(flagList, "layer", &flags[6], 0);
		Log::addTag(flags[6], "layer", 0);

		addNodeV(flagList, "event", &flags[7], 0);
		Log::addTag(flags[7], "event", 0);

		addNodeV(flagList, "common", &flags[8], 0);
		Log::addTag(flags[8], "common", 0);

		addNodeV(flagList, "project", &flags[9], 0);
		Log::addTag(flags[9], "project", 0);

		addNodeV(flagList, "render", &flags[10], 0);
		Log::addTag(flags[10], "render", 0);

		addNodeV(flagList, "anim", &flags[11], 0);
		Log::addTag(flags[11], "anim", 0);

		addNodeV(flagList, "timer", &flags[12], 0);
		Log::addTag(flags[12], "timer", 0);

		addNodeV(flagList, "sprite", &flags[13], 0);
		Log::addTag(flags[13], "sprite", 0);

		addNodeV(flagList, "char", &flags[14], 0);
		Log::addTag(flags[14], "char", 0);

		addNodeV(flagList, "control", &flags[15], 0);
		Log::addTag(flags[15], "control", 0);

		addNodeV(flagList, "controller", &flags[16], 0);
		Log::addTag(flags[16], "controller", 0);

		addNodeV(flagList, "move", &flags[17], 0);
		Log::addTag(flags[17], "move", 0);

		addNodeV(flagList, "collision", &flags[18], 0);
		Log::addTag(flags[18], "collision", 0);
	*/
}


void addDebugFlag(void* f, void* p) {
	char* flag = (char*) f; 
	Project::get()->enableFlag(flag);
}

void Project::setArgs(int argc, char* argv[]) {
	static Arg arg1 = {
		NULL,
		1,
		NULL,
		0,
		NULL,
		addDebugFlag,
	};

	arg1.name = Str("-dbg");
	arg1.type = Str("alpha");

	static  Arg* args[] = {
		&arg1,
		NULL
	};

	ListManager* lst = defineArgs(args);
	parseArgs(lst, argc, argv);
	deleteList(lst);
}

void Project::close() {
	Project::get(true);
}