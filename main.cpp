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

int main(int argc, char** argv) {

    Log::init(argc, argv);

    Project* pro = Project::get();
    pro->init(argc, argv);

    Log::inf(LOG_MAIN, "=== Init Main ===");
    Log::inf(LOG_MAIN, "--- Init Window");
    RenderWindow window(sf::VideoMode(800, 600), "OpenGL");

    // sf::View view(sf::FloatRect(-150, 0, 800, 600));
    // window.setView(view);

    Log::inf(LOG_MAIN, "--- Init Render");
    Render* rend = Render::get();
    rend->init(&window);

    Log::inf(LOG_MAIN, "=== Init Event Mgr ===");
    EventMgr* evtMgr = EventMgr::get();
    evtMgr->init(&window);

    Log::inf(LOG_MAIN, "--- Init Object");

    Level::load("1");
    vector pos = {0, 0};
    PlayerCtrl* pl = new PlayerCtrl(1, "test", "adventurer", &pos, 1);

    pos.x += 90;
    new Character(CHAR_MONSTER, "test Monster", "adventurer", &pos, 1);
    //Animator* anim = Animator::get();
    TimeMgr* time = TimeMgr::get();
    unsigned int eleapsed = 0;

    // ControlMgr* ctrls = ControlMgr::get();
    //anim->moveTo(obj, 150, 0,1.0f, 0);

    Log::inf(LOG_RENDER, "=== Start Render Loop ===");
    pro->signal();
    rend->lock("Start Render");

    CollisionMgr* colMgr = CollisionMgr::get();

    Log::inf(LOG_MAIN, "=== Starting Main Loop ===");
    while (pro->getStatus() < PRO_CLOSE) {      // Main Loop
        rend->lock("Event Lock");

        evtMgr->handle();

        time->update();

        //colMgr->handle();

        //anim->animate();
        rend->render();

        // eleapsed = time->getElapsedTime();
        double wait = FRAME - eleapsed;
        // double wait = 500;

        if (wait > 0) {
            usleep(wait);
        }

        rend->unlock("Event UnLock", true);

        //usleep(100);
    }


    delete pl;
    Log::inf(LOG_MAIN, "CLOSING PROJECT");

    pro->close();
    Log::closeLog();

    return 0;
}
