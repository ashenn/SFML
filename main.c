#define LOG_MAIN 1
#include "base/logger.h"

int main(int argc, char** argv) {
    Log* logger = initLogger(argc, argv);

    fprintf(stderr, "MAIN-0: %s\n", logger->lvls[0]);
    logger->inf(LOG_MAIN, "TEST");



    closeLogger();

    return 0;
}
