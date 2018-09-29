#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#include "basic.h"
#include "logger.h"

int Log::f = 0;		// File Id
short Log::lvl = -1;	// Output min lvl
char* Log::file = NULL;	// LogFile Path
bool Log::enabled = true;

char** Log::lvls = NULL; // Existing debug lvl
bool Log::isInit = false;
char** Log::lvlColors = NULL; // Existing debug lvl colors

ListManager* Log::args = NULL;
ListManager* Log::tags = NULL;

pthread_cond_t Log::cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;;
pthread_mutex_t Log::mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;;

void Log::log2file(const char* tagName, const char* lvl, const  char* msg){
	time_t current_time;
	struct tm * time_info;
	char timeString[20];

	time(&current_time);
	time_info = localtime(&current_time);

	strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", time_info);

	if (tagName != NULL) {
		write(f, tagName, strlen(tagName));
	}

	write(f, " [", 2);
	write(f, timeString, 19);
	write(f, "] ", 2);
	write(f, lvl, strlen(lvl));
	write(f, ":\n", 2);
	write(f, msg, strlen(msg));
	write(f, "\n\n\n", 3);
}

void Log::logg(short lv, unsigned int  tag, const char* msg, va_list* args){
	if (lv < WARNING) {
		if (!enabled) {
			return;
		}

		if (lv < lvl){
			return;
		}
	}


	char* tagName = NULL;
	if (tags != NULL && tags->nodeCount > 0) {
        Node* n = getNode(tags, tag);
        if (n == NULL && lv < WARNING) {
            return;
        }
        else if(n != NULL){
        	short val = *((short*) n->value);
        	//fprintf(stdout, "===TEST===\n");
        	//fprintf(stdout, "Check: %s\n", n->name);
            //fprintf(stdout, "TEST: %d\n", val);

        	if (!val && lv < WARNING) {
        		return;
        	}

        	tagName = n->name;
        }

	}


    //fprintf(stdout, "LOCK LOGGER\n");
	pthread_mutex_lock(&mutex);

	char message[10000];
	char* l = lvls[lv];
	vsprintf(message, msg, *args);

    #ifndef _WIN32
        fprintf(stdout, "%s", lvlColors[lv]);
    #endif // _WIN32

    fprintf(stdout, "%s [%s] %s", tagName, l, message);

	#ifndef _WIN32
        fprintf(stdout, KNRM);
    #endif // _WIN32

    fprintf(stdout, "\n\n\n");


	if (f > 0)	{
        //fprintf(stdout, "LOG 2 FILE:\n");
		log2file(tagName, l, message);
	}

	pthread_mutex_unlock(&mutex);
	//fprintf(stdout, "UN-LOCK LOGGER\n");
}


void Log::setFile(const char* path) {
    file = Str((char*) path);
    validatePath(file);

	f = open(file, O_CREAT|O_WRONLY|O_APPEND, LOGGER_FILE_WRITE);
	if (f > 0)
	{
		return;
	}

	int filen = strlen(file);

	char m[] = "Fail to set file";

	int mlen = strlen(m);

	char msg[filen+mlen];
	concat(m, file, msg);
}

void Log::dbg(unsigned int  tag, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	logg(DEBUG, tag, msg, &args);
	va_end(args);
}

void Log::inf(unsigned int  tag, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	logg(INFO, tag, msg, &args);
	va_end(args);
}

void Log::war(unsigned int  tag, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	logg(WARNING, tag, msg, &args);
	va_end(args);
}

void Log::err(unsigned int  tag, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	logg(ERROR, tag, msg, &args);
	va_end(args);
}

void Log::setLvls(){
	lvls = (char**) malloc((sizeof(char*) * 5));
	memset(lvls, 0, 5);


	lvls[0] = (char*) malloc(sizeof(char) * 8);
	memset(lvls[0], 0, 8);
	strcpy(lvls[0], "DEBUG");

	
	lvls[1] = (char*) malloc(sizeof(char) * 8);
	memset(lvls[1], 0, 8);
	strcpy(lvls[1], "INFO");
	
	lvls[2] = (char*) malloc(sizeof(char) * 8);
	memset(lvls[2], 0, 8);
	strcpy(lvls[2], "WARNING");
	
	lvls[3] = (char*) malloc(sizeof(char) * 8);
	memset(lvls[3], 0, 8);
	strcpy(lvls[3], "ERROR");

	lvls[4] = NULL;
}

void Log::setLvlColors(){
	lvlColors = (char**) calloc(5, sizeof(char*));

	lvlColors[0] = (char*) malloc(sizeof(char) * 6);
	memset(lvlColors[0], 0, 6);
	strcpy(lvlColors[0], KBLU);

	lvlColors[1] = (char*) malloc(sizeof(char) * 6);
	memset(lvlColors[1], 0, 6);
	strcpy(lvlColors[1], KNRM);

	lvlColors[2] = (char*) malloc(sizeof(char) * 6);
	memset(lvlColors[2], 0, 6);
	strcpy(lvlColors[2], KYEL);

	lvlColors[3] = (char*) malloc(sizeof(char) * 6);
	memset(lvlColors[3], 0, 6);
	strcpy(lvlColors[3], KRED);


	lvlColors[4] = NULL;
}

void Log::setLvl(int l){
	int newLvl = l;
	if (newLvl < 0 || newLvl > 4)
	{
		lvl = 2;
		return;
	}

	lvl = newLvl;
}

void setLogLvl(void* l,void* p) {
	int lvl = *((int*) l);
	Log::setLvl(lvl);
}

void setLogFile(void* path, void* p) {
	Log::setFile((char*) path);
}

void Log::setArgs(){
	static Arg arg1 = {
		NULL,
		1,
		NULL,
		1,
		NULL,
		&setLogLvl
	};

	arg1.name = Str("-logLvl");
	arg1.type = Str("num");

	static Arg arg2 = {
		NULL,
		1,
		NULL,
		0,
		NULL,
		setLogFile
	};

	arg2.name = Str("-logPath");
	arg2.type = Str("any");

	static  Arg* argList[] = {
		&arg1,
		&arg2,
		NULL
	};

	args = defineArgs(argList);
}

void Log::init(int argc, char** argv){
	if (isInit){
		return;
	}

	setLvls();
	setLvlColors();

	isInit = true;
	enabled = true;

	tags = initListMgr();

	setArgs();
	parseArgs(args, argc, argv);

	if (!f)
	{
		setFile("./journal.log");
	}

	if (lvl < 0)
	{
		lvl = 2;
	}
}

void Log::closeFile(){
	close(f);
	f = 0;
}

void Log::clearlvls(){
	for (int i = 0; lvls[i] != NULL; ++i) {
		free(lvls[i]);
	}

	free(lvls);
}

void Log::clearlvlColors(){
	for (int i = 0; lvlColors[i] != NULL; ++i) {
		free(lvlColors[i]);
	}

	free(lvlColors);
}

void Log::closeLog(){
	closeFile();
	clearlvls();
	clearlvlColors();

	deleteList(args);
	deleteList(tags);

	free(file);
}

void Log::addTag(unsigned int  tag, const char* name, bool active) {
    Node* n = getNode(tags, tag);

    if (n != NULL) {
        return;
    }

    short* p = (short*) malloc(sizeof(short));
    *p = active;
    n = addNodeV(tags, name, p, 1);
    n->id = tag;
}


bool Log::enableTag(unsigned int  tag) {
    Node* n = getNode(tags, tag);
    if (n == NULL) {
        return 0;
    }

    //fprintf(stdout, "ENABLE LOG TAG: %s\n", n->name);
    short* val = (short*) n->value;
    *val = 1;

    return 1;
}

bool Log::disableTag(unsigned int  tag) {
    Node* n = getNode(tags, tag);

    if (n == NULL) {
        return 0;
    }

    //fprintf(stdout, "Disable LOG TAG: %s\n", n->name);
    short* val = (short*) n->value;
    *val = 0;

    return 1;
}

void Log::enable() {
	enabled = true;
}

void Log::disable() {
	enabled = false;
}

bool Log::isEnable() {
	return enabled;
}

void Log::toggle(bool e) {
	enabled = e;
}