#include "basic.h"
#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int isNum(char c){
	return (c >= 48 && c <= 57);
}

int isAlpha(char c){
	return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

int strIsNum(const char* str){
	int i;
	for (i=0; str[i] != '\0'; ++i)
	{
		if (!isNum(str[i]))
		{
			return 0;
		}
	}

	return 1;
}

int myPow(int a, int b) {
	if (b == 1)
	{
		return a;
	}
	else if(b == 0){
		return 1;
	}

	int i;
	for (i = 0; i < b; ++i)
	{
		b--;
		return a * myPow(a, b);
	}

	return a;
}


int numBase(int num){
	int i;
	if (num < 0){
		num = num * -1;
	}

	for (i = 0; num >= 10; ++i)
	{
		num /= 10;
	}

	return i+1;
}


int char2int(char ch){
	int c = (int) ch;

	if (c >= 48 && c <= 57)
	{
		return c -48;
	}

	return -1;
}

int str2int(const char* str){
	int result = 0;
	int multiplier = 1;
	int tmp = 0;

	int i;
	for (i = 0; str[i] != '\0'; ++i)
	{
		if (str[i] == '-')
		{
			multiplier = -1;
			continue;
		}

		tmp = char2int(str[i]);
		if (tmp == -1)
		{
			return 0;
		}

		result = (result * 10) + tmp;
	}

	return result * multiplier;
}

float str2float(const char* str) {
	float result = 0;
	int multiplier = 1;
	float tmp = 0;
	short decimal = 0;

	int i;
	for (i = 0; str[i] != '\0'; ++i)
	{
		if (str[i] == '-')
		{
			multiplier = -1;
			continue;
		}
		else if (str[i] == '.' || str[i] == ',') {
			if (decimal) {
				return 0;
			}
			else {
				decimal = 1;
			}
			continue;
		}

		tmp = (float) char2int(str[i]);
		if (tmp == -1)
		{
			return 0;
		}

		if (!decimal) {
			result = (result * 10) + tmp;
		}
		else {
			result = (result) + (tmp / ((float) (myPow(10 ,decimal))));
			decimal++;
		}
	}

	return result * ((float) multiplier);
}

int numAtIndex(int num, int index){
	int base;
	int result;

	if (num < 0)
	{
		num = num * -1;
	}

	if (index > 0)
	{
		base = (num / myPow(10, index + 1));
		base = base * 10;

		result = (num / myPow(10, index));

		return result - base;
	}
	else{
		base = (num / 10);
		base = base * 10;

		return num - base;
	}
}

char int2char(int a){
	return a + 48;
}

char* int2str(int num){
	int i;
	int n;
	char c;
	int index = 0;
	int count = numBase(num);
	short t = 1 + (num < 0);

	char* str = (char*) malloc((sizeof(char) * count) + t);
	if (num < 0){
		str[0] = '-';
	}

	for (i = 0; i < count; ++i)
	{
		n = numAtIndex(num, i);
		c = int2char(n);

		index = (count - i)-1;

		if (num >= 0){
			str[index] = c;
		}
		else{
			str[index+1] = c;
		}
	}


	if (num >= 0){
		str[count] = '\0';
	}
	else{
		str[count+1] = '\0';
	}

	return str;
}

int strIsAlpha(const char* str){
	int i;
	for (i=0; str[i] != '\0'; ++i)
	{
		if (!isAlpha(str[i]))
		{
			return 0;
		}
	}

	return 1;
}

int alnum(const char* str){
	int i;
	char c;
	for (i=0; str[i] != '\0'; ++i)
	{
		c = str[i];
		if (!isAlpha(c) && !isNum(c) && c == 32){
			return 0;
		}
	}

	return 1;
}

void substr(const char* str, int offset, int length, char** target){
	int i;
	int z=0;
	for (i = offset; (z < length || !length) && str[i] != '\0'; ++i, ++z){
		(*target)[z] = str[i];
	}

	(*target)[z] = '\0';
}

char* concat(const char* str1, const char* str2, char* res){
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	int max = len1 + len2;

	int i;
	int y;
	char c;
	for (i = y = 0; i < max; y=++i)
	{
		if (i > len1){
			y = i - len1;
			c = str2[y];
		}
		else{
			c = str1[y];
		}

		res[i] = c;
	}

	res[i] = '\0';
	return res;
}


int charCnt(char c, const char* str, int limit){
	int i;
	int cnt;
	for (i=cnt=0; str[i] != '\0' && (cnt < limit || limit==0); ++i) {
		if (str[i] == c)
		{
			cnt ++;
		}
	}

	return cnt;
}

int strpos(char c, const char* str, int offset){
	int i;
	for (i = offset; str[i] != '\0'; ++i){
		if (str[i] == c){
			return i;
		}
	}

	return -1;
}

void explode(char c, const char* str, int offset, int limit, char* res[]){
	int cnt = charCnt(c, str, limit);
	if (!cnt){
        strcpy(res[0], str);
		//res[0] = str;
		res[1] = NULL;
		return;
	}

	int i=0;
	int pos0 = 0;
	int pos1 = 0;

	if (str[0] == c){
		i=1;
		pos0=1;
		res[0] = NULL;
	}


	for (i = i; i < cnt && (pos1 = strpos(c, str, pos0)) > -1; ++i){
		res[i] = (char*) malloc(((pos1 - (pos0)) * sizeof(char)) +1);
		substr(str, pos0, pos1 - pos0, &res[i]);
		pos0 = pos1 +1;
	}

	pos1 = strlen(str) - pos0;
	res[i] = (char*) malloc((pos1 +1) * sizeof(char) +1);

	substr(str, pos0, 0, &res[i]);
	res[i+1] = NULL;
}

float percent(const float x, const float y) {
	return (y / 100) * x;
}

void charReplace(char search, char replace, unsigned int index, char* str) {
	int len = strlen(str);
	int intIndex = (int) index;
	if (intIndex >= len) {
		return;
	}

	for (int i = intIndex; i < len; ++i) {
		if (str[i] == search) {
			str[i] = replace;
		}
	}
}

void validatePath(char* path) {
	#ifdef _WIN32
		char search = '/';
		char replace = '\\';
	#else
		char search = '\\';
		char replace = '/';
	#endif


	charReplace(search, replace, 0, path);
}


char* Str(const char* str) {
	if (str == NULL) {
		return NULL;
	}

    int len = strlen(str)+1;
    char* res = (char*) malloc(len);

    memset(res, 0, len);
    strcpy(res, str);

    return res;
}

char* StrE(const int len) {
    char* res = (char*) malloc(len);
    memset(res, 0, len);

    return res;
}

char* join(const char* glue, char** arr, unsigned int count, unsigned int size) {
    int len = strlen(glue)+1;
	size += strlen(glue) * (count-1);

	char* res = StrE(size+1);

	int intCount = (int) count;
    char sep[len];
    memset(sep, 0, len);
    strcpy(sep, glue);
	for (int i = 0; i < intCount; ++i) {

		if (i >= intCount-1) {
            memset(sep, 0, len);
            strcpy(sep, "");
		}

		char* tmp = Str(res);
		snprintf(res, size, "%s%s%s", tmp, arr[i], sep);
		free(tmp);
	}

	return res;
}
