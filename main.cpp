#include "common.h"

#include "core/event/event.h"
#include "core/render/render.h"
#include "core/project/project.h"

#include "core/object/sprite/spriteObject.h"

#include "core/animation/sprite/spriteAnim.h"

int main(int argc, char** argv) {
    XInitThreads();
    Log::init(argc, argv);
    
    Project* pro = Project::get();
    pro->init(argc, argv);

    Log::inf(LOG_MAIN, "=== Init Main ===");
    Log::inf(LOG_MAIN, "--- Init Window");
    RenderWindow window(sf::VideoMode(800, 600), "OpenGL");


    Log::inf(LOG_MAIN, "=== Init Event Mgr ===");
    EventMgr* evtMgr = EventMgr::get();
    evtMgr->init(&window);

    
    Log::inf(LOG_MAIN, "--- Init Render");
    Render* rend = Render::get();
    rend->init(&window);


    Log::inf(LOG_MAIN, "--- Init Object");
    
    vector pos = {0, 0};
    SpriteObj* obj = new SpriteObj("Test", &pos, 0, "adventurer");

    //AnimLinkFnc<SpriteObj>* lnk = obj->getAnimLinkFnc("test", obj);
    
    //bool (SpriteObj::*fnc)() = lnk->fnc;
    // Log::war(LOG_MAIN, "TEST LINK: %p !!!", lnk);
    // (obj->*(lnk->fnc))();
    //SpriteAnim::getAnimLinkFnc("test");

    // pro->close();
    // return 0;

    obj->addToView();
    Log::inf(LOG_MAIN, "--- Launch Render Thread");
    pro->runRenderTh();


    // Log::war(LOG_MAIN, "=== Init Move ===");
    // Animator* animator = Animator::get();
    // animator->moveTo(obj, 150, 150, 5.0f, 0);


    Log::inf(LOG_MAIN, "=== Starting Main Loop ===");
    while (pro->getStatus() < PRO_CLOSE) {      // Main Loop
        rend->lock("Event Lock");

        evtMgr->handle();

        rend->unlock("Event UnLock", true);

        usleep(100);
    }


    Log::inf(LOG_MAIN, "CLOSING PROJECT");
    pro->close();

    Log::closeLog();
    return 0;
}
