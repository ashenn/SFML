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

void log2file(const char* tagName, const char* lvl, const  char* msg){
	Log* logger = getLogger();

	time_t current_time;
	struct tm * time_info;
	char timeString[20];

	time(&current_time);
	time_info = localtime(&current_time);

	strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", time_info);

	if (tagName != NULL) {
		write(logger->f, tagName, strlen(tagName));
	}

	write(logger->f, " [", 2);
	write(logger->f, timeString, 19);
	write(logger->f, "] ", 2);
	write(logger->f, lvl, strlen(lvl));
	write(logger->f, ":\n", 2);
	write(logger->f, msg, strlen(msg));
	write(logger->f, "\n\n\n", 3);
}

void logg(short lvl, unsigned int  tag, const char* msg, va_list* args){
	Log* logger = getLogger();



	if (!logger->enabled && lvl < WARNING) {
		return;
	}

	if (lvl < logger->lvl){
		return;
	}

	char* tagName = NULL;
	if (logger->tags != NULL && logger->tags->nodeCount > 0) {
        Node* n = getNode(logger->tags, tag);
        if (n == NULL && lvl < WARNING) {
            return;
        }
        else if(n != NULL){
        	short val = *((short*) n->value);
        	//fprintf(stdout, "===TEST===\n");
        	//fprintf(stdout, "Check: %s\n", n->name);
            //fprintf(stdout, "TEST: %d\n", val);

        	if (!val && lvl < WARNING) {
        		return;
        	}

        	tagName = n->name;
        }

	}

    //fprintf(stdout, "LOCK LOGGER\n");
	pthread_mutex_lock(&logger->mutex);

	char message[10000];
	char* l = logger->lvls[lvl];
	vsprintf(message, msg, *args);

    //fprintf(stdout, "MSG: %s\n", msg);

    #ifndef _WIN32
        fprintf(stdout, "%s", logger->lvlColors[lvl]);
    #endif // _WIN32

    fprintf(stdout, "%s [%s] %s", tagName, l, message);

	#ifndef _WIN32
        fprintf(stdout, KNRM);
    #endif // _WIN32

    fprintf(stdout, "\n\n\n");


	if (logger->f > 0)	{
        //fprintf(stdout, "LOG 2 FILE:\n");
		log2file(tagName, l, message);
	}

	pthread_mutex_unlock(&logger->mutex);
	//fprintf(stdout, "UN-LOCK LOGGER\n");
}


void setLogFile(void* l, void* file) {
	Log* logger = (Log*) l;
    logger->file = Str((char*) file);


	logger->f = open(logger->file, O_CREAT|O_WRONLY|O_APPEND, LOGGER_FILE_WRITE);
	if (logger->f > 0)
	{
		return;
	}

	int filen = strlen(logger->file);

	char m[] = "Fail to set file";

	int mlen = strlen(m);

	char msg[filen+mlen];
	concat(m, logger->file, msg);
}

void logDebug(unsigned int  tag, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	logg(DEBUG, tag, msg, &args);
	va_end(args);
}

void logInfo(unsigned int  tag, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	logg(INFO, tag, msg, &args);
	va_end(args);
}

void logWarning(unsigned int  tag, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	logg(WARNING, tag, msg, &args);
	va_end(args);
}

void logError(unsigned int  tag, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	logg(ERROR, tag, msg, &args);
	va_end(args);
}

void setLoggerFuncs(Log* logger){
	logger->dbg = logDebug;
	logger->inf = logInfo;
	logger->war = logWarning;
	logger->err = logError;
	logger->close = closeLogger;
}

void setLoggerLvls(Log* logger){
	logger->lvls = (char**) calloc(5, sizeof(char) * 8);

	strcpy(logger->lvls[0], "DEBUG");
	strcpy(logger->lvls[1], "INFO");
	strcpy(logger->lvls[2], "WARNING");
	strcpy(logger->lvls[3], "ERROR");

	logger->lvls[4] = NULL;
}

void setLoggerLvlColors(Log* logger){
	logger->lvlColors = (char**) calloc(5, sizeof(char) * 6);

	strcpy(logger->lvlColors[0], KBLU);
	strcpy(logger->lvlColors[1], KNRM);
	strcpy(logger->lvlColors[2], KYEL);
	strcpy(logger->lvlColors[3], KRED);

	logger->lvlColors[4] = NULL;
}

void setLogLvl(void* l, void* p){
	int lvl = *((int*) l);
	Log* logger = getLogger();
	if (lvl < 0 || lvl > 4)
	{
		logger->lvl = 2;
		return;
	}

	logger->lvl = lvl;
}

void parseLogArgs(){
	Log* logger = getLogger();

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

	arg2.defParam = (void*)logger;

	static  Arg* args[] = {
		&arg1,
		&arg2,
		NULL
	};

	logger->args = defineArgs(args);
}

Log* initLogger(int argc, char** argv){
	static Log* logger = NULL;
	if (logger != NULL){
		return logger;
	}


	logger = (Log*) malloc(sizeof(Log));
	if (logger == NULL){
		return NULL;
	}

	logger->cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	logger->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

	logger->f = 0;
	logger->lvl = -1;

	setLoggerLvls(logger);
	setLoggerLvlColors(logger);
	setLoggerFuncs(logger);


	logger->isInit = 1;
	logger->enabled = 1;

	logger->tags = initListMgr();

	parseLogArgs();
	parseArgs(logger->args, argc, argv);

	if (!logger->f)
	{
		setLogFile(logger, (void*) "./journal.log");
	}

	if (logger->lvl < 0)
	{
		logger->lvl = 2;
	}


	return logger;
}

void closeLogFile(){
	Log*  logger = getLogger();

	close(logger->f);
	logger->f = 0;
}

void clearLoglvls(){
	Log*  logger = getLogger();

	free(logger->lvls);
}

void clearLoglvlColors(){
	Log*  logger = getLogger();

	free(logger->lvlColors);
}

void closeLogger(){
	Log*  logger = getLogger();
	closeLogFile();
	clearLoglvls();
	clearLoglvlColors();

	deleteList(logger->args);
	deleteList(logger->tags);

	free(logger);
}

Log* getLogger(){
	return initLogger(0, NULL);
}

void addLoggerTag(unsigned int  tag, const char* name, short active) {
    Log* logger = getLogger();
    Node* n = getNode(logger->tags, tag);

    if (n != NULL) {
        return;
    }

    short* p = (short*) malloc(sizeof(short));
    *p = active;
    n = addNodeV(logger->tags, name, p, 1);
    n->id = tag;
}


short enableLoggerTag(unsigned int  tag) {
    Log* logger = getLogger();
    Node* n = getNode(logger->tags, tag);
    if (n == NULL) {
        return 0;
    }

    fprintf(stdout, "ENABLE LOG TAG: %s\n", n->name);
    short* val = (short*) n->value;
    *val = 1;

    return 1;
}

short disableLoggerTag(unsigned int  tag) {
    Log* logger = getLogger();
    Node* n = getNode(logger->tags, tag);

    if (n == NULL) {
        return 0;
    }

    fprintf(stdout, "Disable LOG TAG: %s\n", n->name);
    short* val = (short*) n->value;
    *val = 0;

    return 1;
}
