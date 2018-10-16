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

    pos.y = -130;
    pos.x = 880;
    Object* obj7 = new EnvObj("Test Env3", "land/sprite", "ground", &pos, 1);
    obj7->addToView();

    pos.y = -30;
    pos.x = 780;
    Object* obj6 = new EnvObj("Test Env3", "land/sprite", "ground", &pos, 1);
    obj6->addToView();

    pos.y = 40;
    pos.x = 520;
    Object* obj5 = new EnvObj("Test Env3", "land/sprite", "ground", &pos, 1);
    obj5->addToView();

    pos.y = 60;
    pos.x = 380;
    Object* obj4 = new EnvObj("Test Env2", "land/sprite", "ground", &pos, 1);
    obj4->addToView();

    pos.y = 60;
    pos.x = 100;
    Object* obj3 = new EnvObj("Test Env1", "land/sprite", "ground", &pos, 1);
    obj3->addToView();

    pos.y = 60;
    pos.x = -40;
    Object* obj2 = new EnvObj("Test Env", "land/sprite", "ground", &pos, 1);
    obj2->addToView();


    int fi = (1 << (int)COL_ITEM);
    int fw = (1 << (int)COL_WALL);
    int fp = (1 << (int)COL_PLAYER);

    int cw = fw | fi | fp;
    int ci = fw;
    int cp = fp | fw;

    /*Log::war(LOG_COL, "Testing Wall: %u", COL_WALL);
    Log::war(LOG_COL, "Testing Player: %u", COL_PLAYER);
    Log::war(LOG_COL, "Testing Item: %u", COL_ITEM);

    Log::war(LOG_COL, "Testing Collision Wall Item: %d", cw & ci);
    Log::war(LOG_COL, "Testing Collision Player Item: %d", cp & ci);
    Log::war(LOG_COL, "Testing Collision Player Wall: %d", cp & cw);*/

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

        colMgr->handle();

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
