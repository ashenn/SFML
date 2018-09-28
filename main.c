#define LOG_MAIN 1
#include "common.h"

#include "base/basic.h"
#include "base/logger.h"
#include "base/json.h"

int main(int argc, char** argv) {
    logger = initLogger(argc, argv);

    fprintf(stderr, "MAIN-0: %s\n", logger->lvls[0]);
    logger->inf(LOG_MAIN, "LOAD JSON");


    char* t = Str("test.json");
    logger->inf(LOG_MAIN, "MAIN-2: %s\n", t);
    Json* json  = loadJsonFile(t);

    jsonPrint(json, 0);

    deleteJson(json);
    free(t);
    closeLogger();

    return 0;
}
