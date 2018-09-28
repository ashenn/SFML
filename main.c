#define LOG_MAIN 1
#include "base/logger.h"

int main(int argc, char** argv) {
    Log* logger = initLogger(argc, argv);
    logger->inf(LOG_MAIN, "TEST");

    closeLogger();

    return 0;
}
