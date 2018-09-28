#include "base/logger.h"
#include <string>
#define LOG_MAIN 1

using namespace std;

int main(int argc, char** argv) {
    Log* logger = initLogger(argc, argv);
    string str = "TEST";
    const char* s = str.c_str();
    logger->inf(LOG_MAIN, s);

    closeLogger();

    return 0;
}
