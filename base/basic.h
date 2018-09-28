#ifndef __BASIC_C__
#define __BASIC_C__


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>


int isNum(char c);
int isAlpha(char c);
int strIsNum(const char* str);
int char2int(char ch);
int str2int(const char* str);
int strIsAlpha(const char* str);
int alnum(const char* str);
void substr(const char* str, int offset, int length, char** target);
char* concat(const char* str1, const char* str2, char* res);
int charCnt(char c, const char* str, int limit);
int strpos(char c, const char* str, int offset);
void explode(char c, const char* str, int offset, int limit, char* res[]);

int numBase(int num);
char int2char(int a);
char* int2str(int num);
float str2float(const char* str);
float percent(const float x, const float y);

void validatePath(char* path);
void charReplace(char search, char replace, unsigned int index, char* str);



char* Str(const char* str);
char* StrE(const int len);
char* join(const char* glue, char** arr, unsigned int count, unsigned int size);

#endif
