#include "project.h"
#include "../asset/asset.h"
#include "../render/render.h"
#include "../event/eventMgr.h"
#include "../animation/sprite/spriteAnim.h"
#include "../time/timeMgr.h"

// Project State Enum Name
const char* STATE_NAMES[] = {
    PROJECT_STATES(GEN_STATE_NAMES)
};

const char* Project::getStatusName(ProjectState status) {
	return STATE_NAMES[status];
}

Project::Project() {
	STATIC_CLASS_CONSTRUCT(Project)

	TimeMgr* time = TimeMgr::get();
	time->start();
	status = PRO_INIT;
	initFlags();
}

Project::~Project() {
	deleteList(flagList);
}

void Project::init(int argc, char* argv[]) {
	if (isInit) {
		return;
	}

	Log::inf(LOG_JSON, "=== Init Project ===");
	setArgs(argc, argv);
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

ProjectState Project::getStatus() {
	return status;
}

void Project::initFlags() {
	flagList = initListMgr();

	// Logger Flags
	static unsigned int flags[20] = {
	    LOG_NONE,
		LOG_MAIN,
		LOG_JSON,
		LOG_PROJECT,
		LOG_OBJ,
		LOG_RENDER,
		LOG_ANIM,
		LOG_EVENT,
		LOG_ASSET,
		LOG_SPRITE,
		LOG_SPRITE_OBJ,
		LOG_SPRITE_ANIM,
		LOG_EVENT_KEY,
		LOG_CHAR,
		LOG_CTRL,
		LOG_CTRL_PLAYER,
		LOG_MOVE,
		LOG_VIEW/*,

		LOG_VIEW,
		LOG_LAYER,
		LOG_COMMON,
		LOG_ANIM,
		LOG_TIMER,
		LOG_SPRITE,
		LOG_CHAR,
		LOG_CONTROL,
		LOG_CONTROLLER,
		LOG_MOVE,
		LOG_COLLISION*/
	};


	// Add Flags To Logger
	const char* flagsNames[19] = {
		"none",
		"main",
		"json",
		"project",
		"obj",
		"render",
		"anim",
		"event",
		"asset",
		"sprite",
		"spriteObj",
		"spriteAnim",
		"eventKey",
		"char",
		"ctrl",
		"ctrlPlayer",
		"move",
		"view",
		NULL
	};

	for (int i = 0; flagsNames[i] != NULL; ++i) {
		addNodeV(flagList, flagsNames[i], &flags[i], 0);
		Log::addTag(flags[i], flagsNames[i], 0);
	}

	/*
	addNodeV(flagList, "none", &flags[0], 0);
	Log::addTag(flags[0], "none", 0);

	addNodeV(flagList, "main", &flags[1], 0);
	Log::addTag(flags[1], "main", 0);

	addNodeV(flagList, "json", &flags[2], 0);
	Log::addTag(flags[2], "json", 0);

	addNodeV(flagList, "project", &flags[3], 0);
	Log::addTag(flags[3], "project", 0);

	addNodeV(flagList, "obj", &flags[4], 0);
	Log::addTag(flags[4], "obj", 0);

	addNodeV(flagList, "render", &flags[5], 0);
	Log::addTag(flags[5], "render", 0);

	addNodeV(flagList, "anim", &flags[6], 0);
	Log::addTag(flags[6], "anim", 0);

	addNodeV(flagList, "event", &flags[7], 0);
	Log::addTag(flags[7], "event", 0);

	addNodeV(flagList, "asset", &flags[8], 0);
	Log::addTag(flags[8], "asset", 0);

	addNodeV(flagList, "sprite", &flags[9], 0);
	Log::addTag(flags[9], "sprite", 0);

	addNodeV(flagList, "spriteObj", &flags[10], 0);
	Log::addTag(flags[10], "spriteObj", 0);

	addNodeV(flagList, "spriteAnim", &flags[11], 0);
	Log::addTag(flags[11], "spriteAnim", 0);


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
	this->status = PRO_CLOSE;
	Log::inf(LOG_PROJECT, "=== Closing Project ===");
	
	// if (this->rendering) {
	// 	Log::dbg(LOG_PROJECT, "-- Waiting Render Thread");
	// 	pthread_join(this->renderTh, NULL);
	// 	Log::dbg(LOG_PROJECT, "-- Thread Joned");
	// }

	Log::dbg(LOG_PROJECT, "-- Clearing Animator");
	Animator::get(true);
	
	// Log::dbg(LOG_PROJECT, "-- Clearing Anim Links");
	// SpriteAnim::clearAnimLinkFnc();

	Log::dbg(LOG_PROJECT, "-- Clearing Events");
	EventMgr::get(true);

	Log::dbg(LOG_PROJECT, "-- Clearing Render");
	Render::get(true);

	Log::dbg(LOG_PROJECT, "-- Clearing Objects");
	Object::clearObjects();

	Log::dbg(LOG_PROJECT, "-- Clearing Asset");
	AssetMgr::get(true);

	Log::dbg(LOG_PROJECT, "-- Clearing TimeMgr");
	TimeMgr::get(true);

	Log::dbg(LOG_PROJECT, "-- Deleting Project");
	Project::get(true);
}


void Project::changeStatus(ProjectState status) {
	bool b = this->lock("Changing Project Status");	

	Log::inf(LOG_PROJECT, "=== Changing Project Status: %s ===", STATE_NAMES[status]);
	this->status = status;

	this->unlock("Unlock Project Status", b);	
}


// void Project::runRenderTh() {
// 	Log::inf(LOG_PROJECT, "=== Call Render Thread ===");
	
// 	this->rendering = true;
// 	pthread_create(&this->renderTh, NULL, renderThread, NULL);

// 	Log::dbg(LOG_PROJECT, "-- Waiting Render");
// 	this->wait();

// 	Log::dbg(LOG_PROJECT, "-- Render Thread Ready");
// 	this->unlock("Render Thread", true);
// }