#include "../common.h"
#include "json.h"
#include "lib/jsmn/jsmn.h"

#include <assert.h>
#include <stdarg.h>

void jsonFill(Json* json, char* data) {
	Log::inf(LOG_JSON, "==== FILLING JSON ====");

	jsmn_parser parser;
	Log::dbg(LOG_JSON, "-- DATA: \n%s", data);

	Log::dbg(LOG_JSON, "-- init parser");
	jsmn_init(&parser);

	Log::dbg(LOG_JSON, "-- Count Tokens");
	int tokenCnt = jsmn_parse(&parser, data, strlen(data), NULL, 1500);

	if (tokenCnt < 0) {
        switch (tokenCnt) {
            case JSMN_ERROR_INVAL:
                Log::err(LOG_JSON, "JSON CORRUPTED");
                break;

            case JSMN_ERROR_NOMEM:
                Log::err(LOG_JSON, "Not Enought TOKENS");
                break;

            case JSMN_ERROR_PART:
                Log::err(LOG_JSON, "JSON string is too short, expecting more JSON data");
                break;

            default:
                Log::err(LOG_JSON,"-- UNKNOWN Error !!!!");
                break;
        }

        return;
	}

	Log::dbg(LOG_JSON, "-- Re-Init Parser");
	jsmn_init(&parser);

	Log::dbg(LOG_JSON, "-- Init Tokens");
	jsmntok_t tokens[tokenCnt];

	Log::dbg(LOG_JSON, "-- Parse JSON");
	int res = jsmn_parse(&parser, data, strlen(data), tokens, tokenCnt);
    if (res == JSMN_ERROR_NOMEM) {
        Log::err(LOG_JSON, "FAIL TO PARSE JSON, NOT ENOUGTH MEMORY !!!");
    }

	int index = 0;
	short isKey = 1;
	short inArray = 0;

	char key[1500];

	json->childCount = tokens[0].size;

	if (data[tokens[0].start] == '{')
	{
		Log::dbg(LOG_JSON, "-- Is Object: %c", data[tokens[0].start]);
		json->type = JSON_OBJECT;
	}
	else if (data[tokens[0].start] == '[') {
		inArray = true;
		Log::dbg(LOG_JSON, "-- Is Array: %c", data[tokens[0].start]);
		json->type = JSON_ARRAY;
	}
	else {
		Log::err(LOG_JSON, "JSON CORRUPTED");
		return;
	}

	Log::dbg(LOG_JSON, "-- JSON Type: %s", GEN_JSON_TYPE_STRING[json->type]);
	Log::dbg(LOG_JSON, "-- LOOP JSON");
	for (int i = 1; i < res; ++i) {

		int length = (tokens[i].end - tokens[i].start) + 1;
		char buffer[length];
		memset(buffer, 0, length);
		memcpy(buffer, &data[tokens[i].start], tokens[i].end - tokens[i].start);
		Log::dbg(LOG_JSON, "-- Buffer: %s", buffer);

		if (isKey && !inArray) {
			isKey = 0;
			strcpy(key, buffer);
			Log::dbg(LOG_JSON, "-- Key %s", buffer);
		}
		else{
			int num = 0;
			float val = 0;
			bool boolean = 0;
			if (inArray) {
				snprintf(key, 100, "%d", index);
				Log::dbg(LOG_JSON, "-- Key %s", key);
			}

			switch (tokens[i].type) {
				case JSMN_UNDEFINED:
					Log::dbg(LOG_JSON, "-- Type: Undefined");
					jsonSetValue(json, key, NULL, JSON_NULL);
					break;

				case JSMN_ARRAY:
					inArray = 1;
					index = 0;

					Log::dbg(LOG_JSON, "-- Type: Array: %d", tokens[i].size);
					json = jsonSetValue(json, key,  initListMgr(), JSON_ARRAY);
					json->type = JSON_ARRAY;
					json->childCount = tokens[i].size;

					//strcpy(key, buffer);
					continue;
					break;

				case JSMN_OBJECT:
					isKey = 1;
					index = 0;

					Log::dbg(LOG_JSON, "-- Type: Object: %d", tokens[i].size);
					json = jsonSetValue(json, key,  initListMgr(), JSON_OBJECT);
					json->type = JSON_OBJECT;
					json->childCount = tokens[i].size;

					//strcpy(key, buffer);
					continue;
					break;


				case JSMN_STRING:
					Log::inf(LOG_JSON, "-- Type: String");
					jsonSetValue(json, key, buffer, JSON_STRING);
					break;

				case JSMN_PRIMITIVE:
					Log::inf(LOG_JSON, "-- Type: Primitive");

					if (buffer[0] == 'n') {
						jsonSetValue(json, key, NULL, JSON_NULL);
					}
					else if(buffer[0] == 't') {
						boolean = true;
						jsonSetValue(json, key, &boolean, JSON_BOOL);
					}
					else if(buffer[0] == 'f') {
						boolean = false;
						jsonSetValue(json, key, &boolean, JSON_BOOL);
					}
					else {
						if (strpos('.', buffer, 0) >= 0) {
							val = str2float(buffer);
							jsonSetValue(json, key, &val, JSON_NUM);
						}
						else {
							num = str2int(buffer);
							jsonSetValue(json, key, &num, JSON_INT);
						}
					}

					break;
			}

			if (!inArray) {
				isKey = 1;
			}

			while (json->id >= 0 && (inArray || json->type == JSON_OBJECT)) {
				if (index+1 >= json->childCount) {
					Log::inf(LOG_JSON, "-- Back To Parent");

					if (json->parent == NULL) {
						Log::inf(LOG_JSON, "-- Parent IS NULL");
					}

					Log::inf(LOG_JSON, "-- Become Parent: %s", json->parent->key);
					json = json->parent;

					Log::inf(LOG_JSON, "-- Check Count");
					index = json->childs->nodeCount-1;

					Log::inf(LOG_JSON, "-- Set Is Key");
					isKey = 1;

					Log::inf(LOG_JSON, "-- Parent Type");
					inArray = json->type == JSON_ARRAY;

					Log::inf(LOG_JSON, "-- End Of Container Reached");
				}
				else{
					break;
				}
			}

			index++;
		}
	}
	//jsonPrint(json, 0);
}

Json* newJson() {
	Log::inf(LOG_JSON, "==== Creating New Json ===");

	Json* json = (Json*) malloc(sizeof(Json));
	json->id = -1;
	json->num = 0;
	json->key = NULL;
	json->string = NULL;

	json->parent = NULL;
	json->childs = NULL;
	json->type = JSON_NULL;

	Log::dbg(LOG_JSON, "-- Json Ready");
	return json;
}

Json* loadJsonFile(const char* p) {
	char path[strlen(p)];
	memset(path, 0, strlen(p));
	strcpy(path, p);
		
	validatePath(path);
	Log::inf(LOG_JSON, "==== New Json From Path: %s ===", p);

	Log::inf(LOG_JSON, "-- TEST-0");
	char* content = fileGetContent(path);
	Log::inf(LOG_JSON, "-- TEST-1");
	if (content == NULL) {
		Log::err(LOG_JSON, "Fail to Load Json File: %s", path);
		return NULL;
	}

	Log::inf(LOG_JSON, "-- Content Length: %d ===", strlen(content));
	Json* json = newJson();
	Log::inf(LOG_JSON, "-- CALL FILL JSON");
	jsonFill(json, content);

	free(content);
	return json;
}

Json* jsonSetValue(Json* json, char* key, void* value, JsonDataEnum type) {
	Log::inf(LOG_JSON, "=== Setting Json %s Key: %s | type: %s", json->key, key, GEN_JSON_TYPE_STRING[type]);

	if (json == NULL) {
		Log::err(LOG_JSON, "Trying to Set Json Value But Json Is Null !!!");
		return NULL;
	}
	else if (key == NULL || !strlen(key)) {
		Log::err(LOG_JSON, "Trying to Set Json Value But Key Is Empty !!!");
		return NULL;
	}

	if (json->type == JSON_NULL) {
		json->type = JSON_OBJECT;
	}
	else if(json->type != JSON_ARRAY && json->type != JSON_OBJECT){
		Log::war(LOG_JSON, "Trying to Set Json Value But JSON IS NOT ARRAY OR OBJECT !!!");
		json->type = JSON_OBJECT;
	}

	if (value == NULL && type != JSON_NULL) {
		Log::war(LOG_JSON, "Trying to Set Json Value As NULL But Type is not JSON_NULL !!!");
		type = JSON_NULL;
	}

	Json* child = NULL;
	Node* childNode = NULL;

	if (json->childs == NULL) {
		// Log::dbg(LOG_JSON, "-- Init Child List");
		json->childs = initListMgr();

		// Log::dbg(LOG_JSON, "-- Adding Key To Parent");
		childNode = addNode(json->childs, key);
	}
	else{
		childNode = getNodeByName(json->childs, key);

		if (childNode != NULL) {
			// Log::dbg(LOG_JSON, "-- Replacing Existing Child");
			child = (Json*) childNode->value;
			child->parent = NULL;

			deleteJson(child);
		}
		else {
			childNode = addNode(json->childs, key);
		}
	}

	child = newJson();
	child->id = childNode->id;
	childNode->value = (void*) child;


	child->type = type;
	child->parent = json;
	child->key = Str(key);

	switch (type) {
		case JSON_NULL:
			// Log::dbg(LOG_JSON, "-- Setting Null Value");
			break;

		case JSON_BOOL:
			// Log::dbg(LOG_JSON, "-- Setting Boolean Value");
			child->boolean = *((bool*) value);
			break;

		case JSON_STRING:
			// Log::dbg(LOG_JSON, "-- Setting String Value");
			child->string = Str((char*) value);
			break;

		case JSON_INT:
			// Log::dbg(LOG_JSON, "-- Setting Int Value");
			child->integer = *((int*) value);
			break;

		case JSON_NUM:
			// Log::dbg(LOG_JSON, "-- Setting Numeric Value");
			child->num = *((float*) value);
			break;

		case JSON_ARRAY:
		case JSON_OBJECT:
			// Log::dbg(LOG_JSON, "-- Prepare Child->Child List");
			child->childs = (ListManager*) value;
			break;
	}

	// Log::dbg(LOG_JSON, "==== Setting Key: %s Done ====", key);
	return child;
}

short jsonClearChilds(int i, Node* n, short* deleteChild, void* param, va_list* args) {
	Log::inf(LOG_JSON, "-- Child Delete: #%d => %s", i, n->name);

	if (n->value == NULL) {
		Log::war(LOG_JSON, "-- Deleting NULL Json Child");
		*deleteChild = true;
		return true;
	}

	Log::dbg(LOG_JSON, "-- Remove Parent Json Reference");
	Json* json = (Json*) n->value;
	json->parent = NULL;

	Log::dbg(LOG_JSON, "-- Deleting Json");
	deleteJson(json);

	return true;
}

void deleteJson(Json* json) {
	Log::inf(LOG_JSON, "==== Deleting Json: #%d => %s ===", json->id, json->key);

	if (json->key != NULL) {
		Log::inf(LOG_JSON, "-- Free Json KEY");
		free(json->key);
		json->key = NULL;
	}

	if (json->string != NULL) {
		Log::inf(LOG_JSON, "-- Free String Data Json");
		free(json->string);
		json->string = NULL;
	}

	if (json->parent != NULL) {
		Log::inf(LOG_JSON, "-- Removing From Parent Json");
		deleteNodeByName(json->parent->childs, json->key);
	}

	if (json->childs != NULL) {
		Log::inf(LOG_JSON, "-- Removing Json Childs");
		listIterateFnc(json->childs, jsonClearChilds, NULL, NULL);
		deleteList(json->childs);
		json->childs = NULL;
	}

	free(json);
}

short jsonPrintData(int i, Node* n, short* deleteJson, void* param, va_list* args) {
	Log::inf(LOG_JSON, "=== TEST-0");
	Json* json = (Json*) n->value;

	Log::inf(LOG_JSON, "=== TEST-1");

	int tab = *((int*) param);
	Log::inf(LOG_JSON, "=== TEST-1.2");

	int curTab = tab+1;
	Log::inf(LOG_JSON, "=== Cur Tab: %d", curTab);

	if (json == NULL) {
		Log::err(LOG_JSON, "=== JSON IS NULL");
		return true;
	}

	char* key;
	char tmpFormat[250];
	memset(tmpFormat, 0, 250);
	Log::inf(LOG_JSON, "=== TEST-1.1");

	char format[250];
	memset(format, 0, 250);

	if (json->key != NULL && json->parent->type != JSON_ARRAY) {
		Log::dbg(LOG_JSON, "=== TEST-2.0");

		short keyLen = strlen(json->key)+1;
		Log::dbg(LOG_JSON, "-- Key: %s", json->key);
		Log::dbg(LOG_JSON, "-- Len: %d", keyLen);

		Log::dbg(LOG_JSON, "-- Init Format");
		
		memset(format, 0, 250);
		memset(tmpFormat, 0, 250);
		Log::dbg(LOG_JSON, "-- Tabs: %d", tab);
		
		for (int i = 0; i <= curTab; ++i) {
			strcpy(tmpFormat, format);
			snprintf(format, 250, "%s%s", "  ", tmpFormat);
		}

		snprintf(format, 250, "%s\"%%s\": ", tmpFormat);
		Log::dbg(LOG_JSON, "-- Format: %s", format);

		keyLen += strlen(format);

		Log::dbg(LOG_JSON, "-- Init Key");
		char keyName[keyLen];
		memset(keyName, 0, keyLen);
		snprintf(keyName, 250, format, json->key);

		key = Str(keyName);
		Log::dbg(LOG_JSON, "-- Key Result: %s", key);
	}
	else{
		Log::inf(LOG_JSON, "=== TEST-2.1");
		
		memset(format, 0, 250);
		memset(tmpFormat, 0, 250);
		
		for (int i = 0; i <= curTab; ++i) {
			strcpy(tmpFormat, format);
			snprintf(format, 250, "%s%s", "  ", tmpFormat);
		}

		Log::dbg(LOG_JSON, "-- Format: %s", format);
		int keyLen = strlen(format);

		Log::dbg(LOG_JSON, "-- Init Key");
		
		char keyName[keyLen];
		memset(keyName, 0, keyLen);
		snprintf(keyName, 250, format, json->key);

		key = Str(keyName);
		Log::dbg(LOG_JSON, "-- Key Result: %s", key);
	}

	Log::inf(LOG_JSON, "-- Key-0: %s", key);

	char sep = ',';
	if (i+1 >= json->parent->childs->nodeCount) {
		sep = ' ';
	}
	Log::inf(LOG_JSON, "-- SEPARATOR: %c", sep);
	Log::inf(LOG_JSON, "-- Key-1: %s", key);

	char value[5500];
	memset(value, 0, 5500);
	switch (json->type) {
		case JSON_NULL:
			Log::inf(LOG_JSON, "-- Key-2: %s", key);
			snprintf(value, 5500, "NULL");
			break;

		case JSON_BOOL:
			Log::inf(LOG_JSON, "-- Key-3: %s", key);
			if (json->boolean) {
				snprintf(value, 5500, "true");
			}
			else{
				snprintf(value, 5500, "false");
			}
			Log::inf(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_INT:
			Log::inf(LOG_JSON, "-- Key-3: %s", key);
			snprintf(value, 5500, "%d", json->integer);
			Log::inf(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_NUM:
			Log::inf(LOG_JSON, "-- Key-3: %s", key);
			snprintf(value, 5500, "%f", json->num);
			Log::inf(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_STRING:
			Log::inf(LOG_JSON, "-- Key-4: %s", key);
			snprintf(value, 5500, "\"%s\"", json->string);
			break;

		case JSON_ARRAY:
			Log::inf(LOG_JSON, "-- Key-5: %s", key);
			snprintf(value, 5500, "[\n");
			break;

		case JSON_OBJECT:
			Log::inf(LOG_JSON, "-- Key-6: %s", key);
			snprintf(value, 5500, "{\n");
			break;
	}

	Log::inf(LOG_JSON, "-- Key-7: %s", key);
	fprintf(stdout, "%s%s", key, value);

	free(key);

	if (json->type == JSON_ARRAY) {
		listIterateFnc(json->childs, jsonPrintData, NULL, (void*) &curTab);
		fprintf(stdout, "\n%s]", tmpFormat);
	}
	else if(json->type == JSON_OBJECT) {
		listIterateFnc(json->childs, jsonPrintData, NULL, (void*) &curTab);
		fprintf(stdout, "\n%s}", tmpFormat);
	}

	fprintf(stdout, "%c\n", sep);
	return true;
}

void jsonPrint(const Json* json, int tab) {
	if (json == NULL) {
		Log::war(LOG_JSON, "Trying to Print NULL JSON");
		return;
	}

	char* key;
	char tmpFormat[250];
	memset(tmpFormat, 0, 250);

	if (json->key != NULL) {
		short keyLen = strlen(json->key)+1;

		char keyName[keyLen];
		memset(keyName, 0, keyLen);

		char format[250];
		memset(format, 0, 250);

		for (int i = 0; i < tab; ++i) {
			strcpy(tmpFormat, format);
			snprintf(format, 250, "%s%s", "  ", tmpFormat);
		}

		snprintf(format, 250, "%s\"%%s\": ", tmpFormat);
		snprintf(keyName, 250, "%s", json->key);

		key = keyName;
	}
	else{

		char format[250];
		memset(format, 0, 250);

		for (int i = 0; i < tab; ++i) {
			strcpy(tmpFormat, format);
			snprintf(format, 250, "%s%s", "  ", tmpFormat);
		}
		
		snprintf(format, 250, "%s ", tmpFormat);
		Log::err(LOG_JSON, "FORMAT: %s", format);

		key = Str(format);
		Log::err(LOG_JSON, "Key Result: %s", format);
	}

	char value[5500];
	memset(value, 0, 5500);
	switch (json->type) {
		case JSON_NULL:
			snprintf(value, 5500, "NULL\n");
			break;

		case JSON_BOOL:
		    if (json->boolean) {
                snprintf(value, 5500, "true\n");
		    }
		    else{
                snprintf(value, 5500, "false\n");
		    }
			break;

		case JSON_INT:
			snprintf(value, 5500, "%d\n", json->integer);
			break;

		case JSON_NUM:
			snprintf(value, 5500, "%f\n", json->num);
			break;

		case JSON_STRING:
			snprintf(value, 5500, "\"%s\"\n", json->string);
			break;

		case JSON_ARRAY:
			snprintf(value, 5500, "[\n");
			break;

		case JSON_OBJECT:
			snprintf(value, 5500, "{\n");
			break;
	}

	fprintf(stdout, "%s%s", key, value);
	free(key);

	if (json->type == JSON_ARRAY) {
		listIterateFnc(json->childs, jsonPrintData, NULL, (void*) &tab);
		fprintf(stdout, "\n%s]", tmpFormat);
	}
	else if(json->type == JSON_OBJECT) {
		listIterateFnc(json->childs, jsonPrintData, NULL, (void*) &tab);
		fprintf(stdout, "\n%s}", tmpFormat);
	}

	fprintf(stdout, "\n\n");
}






short json2StrData(int i, Node* n, short* deleteJson, void* param, va_list* args) {
	Json* json = (Json*) n->value;
	int tab = *((int*) param);
	int curTab = tab+1;

	bool breakLine = *(va_arg(*args, bool*));
	bool indent = *(va_arg(*args, bool*));
	char** parentStr = va_arg(*args, char**);
	int* parentStrLen = va_arg(*args, int*);

	if (json == NULL) {
		Log::err(LOG_JSON, "=== JSON IS NULL");
		return true;
	}

	char* key;
	char tmpFormat[250];
	memset(tmpFormat, 0, 250);

	char format[250];
	memset(format, 0, 250);

	if (json->key != NULL && json->parent->type != JSON_ARRAY) {
		short keyLen = strlen(json->key)+1;

		char keyName[keyLen];
		memset(keyName, 0, keyLen);

		if (breakLine && indent) {
			for (int i = 0; i <= curTab; ++i) {
				strcpy(tmpFormat, format);
				snprintf(format, 250, "%s%s", "  ", tmpFormat);
			}

		}

		snprintf(format, 250, "%s\"%%s\": ", tmpFormat);
		snprintf(keyName, 250, format, json->key);

		Log::dbg(LOG_JSON, "-- Format: %s", format);
		key = Str(keyName);
		Log::dbg(LOG_JSON, "-- Key Result: %s",key);
	}
	else{
		char keyName[curTab+1];
		memset(keyName, 0, curTab+1);

		if (breakLine && indent) {
			for (int i = 0; i <= curTab; ++i) {
				strcpy(tmpFormat, format);
				snprintf(format, 250, "%s%s", "  ", tmpFormat);
			}
		}
		Log::dbg(LOG_JSON, "-- Format: %s", format);

		key = Str(format);
	}

	Log::dbg(LOG_JSON, "-- Child: Key: %s", key);
	Log::dbg(LOG_JSON, "-- Child type: %s", GEN_JSON_TYPE_STRING[json->type]);

	int write = 0;
	char value[5500];
	memset(value, 0, 5500);
	switch (json->type) {
		case JSON_NULL:
			//Log::dbg(LOG_JSON, "-- Key-2: %s", key);
			write = snprintf(value, 5500, "null");
			break;

		case JSON_BOOL:
			//Log::dbg(LOG_JSON, "-- Key-3: %s", key);
			if (json->boolean) {
				write = snprintf(value, 5500, "true");
			}
			else{
				write = snprintf(value, 5500, "false");
			}
			//Log::dbg(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_INT:
			//Log::dbg(LOG_JSON, "-- Key-3: %s", key);
			write = snprintf(value, 5500, "%d", json->integer);
			//Log::dbg(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_NUM:
			//Log::dbg(LOG_JSON, "-- Key-3: %s", key);
			write = snprintf(value, 5500, "%f", json->num);
			//Log::dbg(LOG_JSON, "-- Key-4: %s", key);
			break;

		case JSON_STRING:
			//Log::dbg(LOG_JSON, "-- Key-4: %s", key);
			write = snprintf(value, 5500, "\"%s\"", json->string);
			break;

		case JSON_ARRAY:
			//Log::dbg(LOG_JSON, "-- Key-5: %s", key);
			write = snprintf(value, 5500, "[");
			break;

		case JSON_OBJECT:
			//Log::dbg(LOG_JSON, "-- Key-6: %s", key);
			write = snprintf(value, 5500, "{");
			break;
	}

	if (breakLine) {
		value[write] = '\n';
	}

	Log::dbg(LOG_JSON, "-- Value: %s", value);
	//fprintf(stdout, "%s%s", key, value);


	char* resultStr = NULL;

	int childStrLen = 0;

	Log::dbg(LOG_JSON, "-- Test Type: %s", GEN_JSON_TYPE_STRING[json->type]);


	Log::dbg(LOG_JSON, "-- Prepare End");
	int end = 0;
	if (json->type == JSON_ARRAY || json->type == JSON_OBJECT) {
		end = 1;
		Log::dbg(LOG_JSON, "-- Prepare Child");
		char* childStr[json->childs->nodeCount+1];
		memset(childStr, 0, json->childs->nodeCount+1);

		Log::dbg(LOG_JSON, "-- Call Child");
		listIterateFnc(json->childs, json2StrData, NULL, (void*) &curTab, (void*) &breakLine, (void*) &indent, (void*) &childStr, (void*) &childStrLen);

		Log::dbg(LOG_JSON, "-- Join Childs");
		resultStr = join(", ", childStr, json->childs->nodeCount, childStrLen);
		Log::dbg(LOG_JSON, "-- Childs Results: %s", resultStr);

		for (int i = 0; i < json->childs->nodeCount; ++i)
		{
			free(childStr[i]);
		}
	}

	Log::dbg(LOG_JSON, "-- get Length");
	Log::dbg(LOG_JSON, "-- Value Len: %d", strlen(value));
	Log::dbg(LOG_JSON, "-- key Len: %d", strlen(key));

	if (resultStr != NULL) {
		Log::dbg(LOG_JSON, "-- resultStr Len: %d", strlen(resultStr));
	}

	int len = strlen(value) + strlen(key) + end + 1;
	if (resultStr != NULL) {
		len += + strlen(resultStr);
	}
	
	Log::dbg(LOG_JSON, "-- Total Length: %d", len);

	*parentStrLen += len;
	parentStr[i] = StrE(len);
	char* parentResultStr = parentStr[i];

	Log::dbg(LOG_JSON, "-- Print 2 Result");
	Log::dbg(LOG_JSON, "-- Value: %s", (value));
	Log::dbg(LOG_JSON, "-- key: %s", (key));

	if (resultStr != NULL) {
		Log::dbg(LOG_JSON, "-- resultStr: %s", (resultStr));
	}

	//int total = snprintf(parentResultStr, len, "%s", key);
	//Log::dbg(LOG_JSON, "-- Set Key:\n %s", (parentResultStr));

    int total = snprintf(parentResultStr, len, "%s%s", key, value);
	Log::dbg(LOG_JSON, "-- Set Key / Value:\n %s", (parentResultStr));

	if (resultStr != NULL) {
		char* tmp = Str(parentResultStr);

		total += snprintf(parentResultStr, len, "%s%s", tmp, resultStr);
		Log::dbg(LOG_JSON, "-- Set Childs:\n %s", (parentResultStr));
		free(tmp);
		free(resultStr);
	}

	Log::dbg(LOG_JSON, "-- Set Result:\n %s", (parentResultStr));


	if (json->type == JSON_ARRAY) {
		char* tmp = Str(parentResultStr);
		total += snprintf(parentResultStr, len, "%s]", tmp);

		free(tmp);
	}
	else if(json->type == JSON_OBJECT) {
		char* tmp = Str(parentResultStr);
		total += snprintf(parentResultStr, len, "%s}", tmp);

		free(tmp);
	}

	free(key);
	Log::dbg(LOG_JSON, "-- Set End:\n %s", (parentResultStr));
	Log::dbg(LOG_JSON, "-- Print Result: %s", parentResultStr);

	return true;
}

char* json2Str(const Json* json, bool breakLine, bool indent) {
	Log::inf(LOG_JSON, "===== JSON 2 STRING ====");

	if (json == NULL) {
		Log::war(LOG_JSON, "Trying to Print NULL JSON");
		return NULL;
	}

	char value[5500];
	memset(value, 0, 5500);
	switch (json->type) {
		case JSON_NULL:
			snprintf(value, 5500, "null\n");
			break;

		case JSON_BOOL:
		    if (json->boolean) {
                snprintf(value, 5500, "true\n");
		    }
		    else{
                snprintf(value, 5500, "false\n");
		    }

			break;

		case JSON_INT:
			snprintf(value, 5500, "%d\n", json->integer);
			break;

		case JSON_NUM:
			snprintf(value, 5500, "%f\n", json->num);
			break;

		case JSON_STRING:
			snprintf(value, 5500, "\"%s\"\n", json->string);
			break;

		case JSON_ARRAY:
			snprintf(value, 5500, "[");
			break;

		case JSON_OBJECT:
			snprintf(value, 5500, "{");
			break;
	}

	char* end = NULL;
	char* resultStr = NULL;

	int childStrLen = 0;
	int childCount = 0;
	if (json->childs != NULL) {
		childCount = json->childs->nodeCount;
	}

	char* childStr[childCount+1];
	memset(childStr, 0, childCount+1);
	int tab = 0;

	if (json->type == JSON_ARRAY) {
		end = Str("]");
	}
	else if(json->type == JSON_OBJECT) {
		end = Str("}");
	}

	if (strlen(end)) {
		listIterateFnc(json->childs, json2StrData, NULL, (void*) &tab, (void*) &breakLine, (void*) &indent, (void*) &childStr, (void*) &childStrLen);
		
		Log::dbg(LOG_JSON, "+++++ Child Count: %d", childCount);
		for (int i = 0; i < childCount; ++i)
		{
			Log::dbg(LOG_JSON, "+++++ Child #%d Results: %s", i, childStr[i]);
		}

		resultStr = join(", ", childStr, json->childs->nodeCount, childStrLen);
	}

	Log::dbg(LOG_JSON, "RESULT STR: %s", resultStr);

	int totalLen = strlen(value) + strlen(resultStr) + strlen(end) + 2;
	char* finalStr = StrE(totalLen);

	snprintf(finalStr, totalLen, "%s%s%s", value, resultStr, end);

	if (strlen(end)) {
		free(end);
	}

	if (strlen(resultStr)) {
		free(resultStr);
	}

	for (int i = 0; i < childCount; ++i) {
		Log::dbg(LOG_JSON, "--- Free Child %d", i);
		free(childStr[i]);
	}

	return finalStr;
}





Json* jsonGetData(const Json* json, const char* key) {
	Log::inf(LOG_JSON, "Getting JSON data: %s", key);

	if (json == NULL) {
		Log::war(LOG_JSON, "Trying To Get Value of A NULL JSON !!!");
		return NULL;
	}


	if (json->childs == NULL) {
		Log::war(LOG_JSON, "Trying To Get Value of A None Parent JSON !!!");
		return NULL;
	}

	Node* n = getNodeByName(json->childs, key);
	if (n == NULL) {
		Log::inf(LOG_JSON, "Fail To Find JSON Data: %s", key);
		return NULL;
	}

	return (Json*) n->value;
}

void* jsonGetValue(const Json* json, const char* key, void* val) {
	Json* res = jsonGetData(json, key);
	if (res == NULL) {
		return NULL;
	}

	switch (res->type) {
		case JSON_NULL:
			return NULL;
			break;

		case JSON_BOOL:
			*((bool*) val) = res->boolean;
			return (void*) 1;
			break;

		case JSON_STRING:
			return Str(res->string);
			break;

		case JSON_INT:
			*((int*) val) = res->integer;
			return (void*) 1;
			break;

		case JSON_NUM:
			*((float*) val) = res->num;
			return (void*) 1;
			break;

		case JSON_ARRAY:
		case JSON_OBJECT:
			return res->childs;
			break;
	}

	return NULL;
}

short jsonIteraterator(int i, Node* n, short* deleteVal, void* param, va_list* args) {
	JsonIterator* it = (JsonIterator*) param;

	if (it->fnc == NULL) {
		Log::err(LOG_JSON, "JSON ITERATOR FUNCTION IS NULL !!!");
		return false;
	}

	return (short) it->fnc((unsigned int) i, (Json*) n->value, (void*) va_arg(*args, ListManager*));
}

void jsonIterate(const Json* json, bool (*fnc)(unsigned int, Json*, void*), void* param, ...) {
	if (json == NULL) {
		Log::war(LOG_JSON, "Trying To Iterate Through NULL JSON !!!");
		return;
	}

	if (json->childs == NULL) {
		Log::war(LOG_JSON, "Trying To Iterate Through JSON BUT CHILDS IS NULL !!!");
		return;
	}

	JsonIterator it;
	it.fnc = fnc;

	listIterateFnc(json->childs, jsonIteraterator, NULL, (void*) &it, param);
}
