#include "common.h"
#include "core/time/timeMgr.h"
#include "core/event/eventMgr.h"
#include "core/render/render.h"

#include "core/control/control.h"
#include "core/project/project.h"
#include "core/object/envirement/level.h"
#include "core/asset/multiTexture/multiTexture.h"

#include "core/collision/collisionMgr.h"

#include "core/object/sprite/spriteObject.h"

#include "core/animation/sprite/spriteAnim.h"
#include "core/controller/player/playerCtrl.h"
#include "core/controller/ai/aiCtrl.h"

void mainLoop(RenderWindow* window) {
    Project* pro = Project::get();
    Garbage* garbage = Garbage::get();
    EventMgr* evtMgr = EventMgr::get();
    // vector pos = {0, 0};
    // sf::Texture* text = AssetMgr::get()->getTexture("land/sea");

    while (pro->getStatus() < PRO_CLOSE) {      // Main Loop
        pro->updateTick();

        evtMgr->handle();
        garbage->clean();

        pro->unlock("Main Loop: Un-Lock", true);

        usleep(500);
        pro->lock("Main Loop: Lock");
    }
}

int main(int argc, char** argv) {
    XInitThreads();
    Log::init(argc, argv);

    Project* pro = Project::get();
    pro->init(argc, argv);

    Log::inf(LOG_MAIN, "=== Init Main ===");
    Log::inf(LOG_MAIN, "--- Init Garbage Collector");
    Garbage::get();

    Log::inf(LOG_MAIN, "--- Init Window");
    RenderWindow window(sf::VideoMode(800, 600), "OpenGL");
    window.setActive(false);

    Log::inf(LOG_MAIN, "--- Init Render");
    Render* rend = Render::get();
    rend->init(&window);

    Log::inf(LOG_MAIN, "=== Init Event Mgr ===");
    EventMgr* evtMgr = EventMgr::get();
    evtMgr->init(&window);

    Log::inf(LOG_MAIN, "--- Init Level");
    Level::load("2");

    pro->lock("Start Loop: Lock");
    rend->start();
    
    Log::inf(LOG_MAIN, "=== Starting Main Loop ===");
    mainLoop(&window);

    Log::inf(LOG_MAIN, "##### CLOSING PROJECT #####");
    pro->close();

    Log::inf(LOG_MAIN, "==== CLOSING: Window ====");
    window.setActive(true);
    window.close();
    
    Log::inf(LOG_MAIN, "==== CLOSING: Logger ====");
    Log::closeLog();

    return 0;
}
