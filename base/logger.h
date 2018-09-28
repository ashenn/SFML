#ifndef __LOGGER_H__
#define __LOGGER_H__

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define DEBUG 0
#define INFO 1
#define WARNING 2
#define ERROR 3

#define LOG_NONE 0

#include "libParse.h"
#include <pthread.h>

#ifdef _WIN32
    #include <sys\stat.h>
    #define LOGGER_FILE_WRITE _S_IRUSR|_S_IWUSR
#else
    #define LOGGER_FILE_WRITE S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP
#endif // _WIN32

#include <iostream>
#include <stdio.h>

class Log
{
    public:
        Log(Log const&)     = delete;
        void operator=(Log const&)  = delete;


		static void init(int argc, char** argv);

		static void setLvl(int lvl);
		static void setFile(const char* path);

		static void enable();
		static void disable();
		static bool isEnable();
		static void toggle(bool e);

		static bool enableTag(unsigned int  tag);
		static bool disableTag(unsigned int  tag);
		static void addTag(unsigned int  tag, const char* name, bool active);

		static void dbg(unsigned int tag, const char* msg, ...);
		static void inf(unsigned int tag, const char* msg, ...);
		static void err(unsigned int tag, const char* msg, ...);
		static void war(unsigned int tag, const char* msg, ...);
		static void closeLog();
		
    private:
        Log() {}
        static void setLvls();
        static void setLvlColors();

        static void clearlvls();
        static void closeFile();
        static void clearlvlColors();
		
		static int f;		// File Id
    	static bool isInit;
		static bool enabled;

		static short lvl;	// Output min lvl
		static char* file;	// LogFile Path
		static char** lvls; // Existing debug lvl
		static char** lvlColors; // Existing debug lvl colors

		static ListManager* args;
		static ListManager* tags;

		static pthread_cond_t cond;
		static pthread_mutex_t mutex;

		static void setArgs();
    	static void log2file(const char* tagName, const char* lvl, const  char* msg);
    	static void logg(short lvl, unsigned int tag, const char* msg, va_list* args);
};

#endif
