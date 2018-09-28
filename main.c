#define LOG_MAIN 1
#include "common.h"

#include "base/basic.h"
#include "base/logger.h"
#include "base/json.h"

int main(int argc, char** argv) {
    logger = initLogger(argc, argv);


    addLoggerTag(LOG_JSON, "json", 1);
    logger->inf(LOG_JSON, "LOAD JSON");

    Json* json  = loadJsonFile("test.json");

    char* t = json2Str(json, 0, 0);

    logger->war(LOG_MAIN, "TEST: Json: \n \n%s", t);

    free(t);
    deleteJson(json);
    closeLogger();

    return 0;
}
