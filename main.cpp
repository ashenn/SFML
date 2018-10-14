#include "common.h"
#include "core/time/timeMgr.h"
#include "core/event/eventMgr.h"
#include "core/render/render.h"

#include "core/control/control.h"
#include "core/project/project.h"
#include "core/object/envirement/envirement.h"
#include "core/asset/multiTexture/multiTexture.h"

#include "core/collision/collisionMgr.h"

#include "core/object/sprite/spriteObject.h"

#include "core/animation/sprite/spriteAnim.h"
#include "core/controller/player/playerCtrl.h"

int main(int argc, char** argv) {
    XInitThreads();
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
    


    vector pos = {0, 0};
    PlayerCtrl* pl = new PlayerCtrl(1, "test", "adventurer", &pos, 0);
    //Character* ch = new Character(CHAR_PLAYER, "Test", "adventurer", &pos, 0);
    // pos.x = -200;
    // pl->getObject()->setPosition(pos);
    // SpriteObj* obj = new SpriteObj("Test", &pos, 0, "adventurer");
    
    SpriteObj* obj = pl->getObject();
    obj->addToView();

    
    // sf::Texture* txt = AssetMgr::get()->getMultiTexture("land/sprite", "ground");


    // IntRect* clip = new IntRect(0, 0, 520, 111);

    pos.y = 60;
    pos.x = -20;
    Object* obj2 = new EnvObj("Test Env", "land/sprite", "ground", &pos, 1);
    obj2->addToView();

    // Object* obj2 = new Object("Test Multi", &pos, txt, clip, 1);
    // SpriteObj* obj2 = new SpriteObj("Base", &pos, 1, "adventurer");

    //obj->addChild(obj2, pos, false);

    // IntRect p(0,0,50,50);
    // obj->addCollision(obj->getName(), p);
    // obj2->addCollision(obj2->getName(), p, COL_MONSTER);
    // Log::war(LOG_MAIN, "TEST: %d", *col && col2);


    // KeyEvt<PlayerCtrl>* e = evtMgr->bindKeyEvent("Test Left", sf::Keyboard::Key::Left, pl);
    // e->setOnPress(pl, &PlayerCtrl::moveEvt);
    // e->setOnRelease(pl, &PlayerCtrl::stopEvt);

    // KeyEvt<PlayerCtrl>* e2 = evtMgr->bindKeyEvent("Test Right", sf::Keyboard::Key::Right, pl);
    // e2->setOnPress(pl, &PlayerCtrl::moveEvt);
    // e2->setOnRelease(pl, &PlayerCtrl::stopEvt);

    Animator* anim = Animator::get();
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

        anim->animate();
        colMgr->handle();
        
        rend->render();

        eleapsed = time->getElapsedTime();
        double wait = FRAME - eleapsed;

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
