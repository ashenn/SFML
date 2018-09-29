#include "common.h"

#include "base/basic.h"
#include "base/logger.h"
#include "base/json.h"
#include "core/project/project.h"


int main(int argc, char** argv) {
    Log::init(argc, argv);
    
    Project* pro = Project::get();
    pro->init(argc, argv);



    pro->close();
    Log::closeLog();

    return 0;
}
