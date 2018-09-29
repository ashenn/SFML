#include "common.h"

#include "core/project/project.h"
#include "core/object/object.h"

#include <SFML/Graphics.hpp>
using namespace sf;


int main(int argc, char** argv) {
    Log::init(argc, argv);
    
    Project* pro = Project::get();
    pro->init(argc, argv);

    RenderWindow window(VideoMode(800, 600), "Mario");

    
    
    Texture* texture = new Texture();
    char img[] = "asset/adventurer/adventurer.png";
    if (!texture->loadFromFile(img)) {
        Log::err(LOG_MAIN, "Fail To Load Image: %s", img);
        return 1;
    }
    
    IntRect* clip = new IntRect(0, 0, 50, 38);
    Object* obj = new Object("Test", texture, clip, 0);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(*obj->getSprite());
        window.display();
    }

    pro->close();
    delete obj;
    delete clip;
    delete texture;

    Log::closeLog();
    return 0;
}
