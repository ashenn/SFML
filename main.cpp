#include "common.h"

#include "core/project/project.h"
#include "core/object/object.h"
#include "core/render/render.h"

#include "core/animation/animation.h"


int main(int argc, char** argv) {
    XInitThreads();
    Log::init(argc, argv);
    
    Project* pro = Project::get();
    pro->init(argc, argv);

    Log::inf(LOG_MAIN, "=== Init Main ===");


    Log::inf(LOG_MAIN, "--- Init Window");
    RenderWindow window(sf::VideoMode(800, 600), "OpenGL");

    // désactivation de son contexte OpenGL
    
    Log::inf(LOG_MAIN, "--- Init Render");
    Render* rend = Render::get();
    rend->init(&window);
        
    Log::inf(LOG_MAIN, "--- Init Image");
    Texture* texture = new Texture();
    char img[] = "asset/adventurer/adventurer.png";
    if (!texture->loadFromFile(img)) {
        Log::err(LOG_MAIN, "Fail To Load Image: %s", img);
        return 1;
    }
    
    Log::inf(LOG_MAIN, "--- Init Object");
    IntRect* clip = new IntRect(0, 0, 500, 380);
    
    Object* obj = new Object("Test", texture, clip);
    obj->addToView();

    pro->runRenderTh();


    // Log::war(LOG_MAIN, "=== Init Move ===");
    // Animator* animator = Animator::get();

    // animator->moveTo(obj, 150, 150, 5.0f, 0);


    Log::inf(LOG_MAIN, "=== Starting Main Loop ===");
    while (pro->getStatus() < PRO_CLOSE) {      // Main Loop

        Event event;
        rend->lock("Event Lock");

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) {
                Log::inf(LOG_MAIN, "=== Set Project To Close");
                pro->changeStatus(PRO_CLOSE);
                break;
            }
        }
        
        rend->unlock("Event UnLock", true);
    }


    Log::inf(LOG_MAIN, "CLOSING PROJECT");
    pro->close();

    delete obj;
    delete clip;
    delete texture;

    Log::closeLog();
    return 0;
}
