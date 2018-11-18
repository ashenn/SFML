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

bool work = true;

void renderingThread(sf::RenderWindow* window)
{
    fprintf(stderr, "START\n");
    // activation du contexte de la fenêtre
    fprintf(stderr, "Thread: Activate\n");
    window->setActive(true);

	while (work && window->isOpen()) {
       window->display();
	}

    fprintf(stderr, "Thread: De-Activate\n");
    window->setActive(false);
    fprintf(stderr, "THRED DONE !!!\n");
}

int main(int argc, char** argv) {
	XInitThreads();
	// création de la fenêtre
    // (rappelez-vous : il est plus prudent de le faire dans le thread principal à cause des limitations de l'OS)
    sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL");

    // désactivation de son contexte OpenGL
    fprintf(stderr, "Main: De-Activate\n");
    window.setActive(false);

    // lancement du thread de dessin
    sf::Thread thread(&renderingThread, &window);
    thread.launch();

    // la boucle d'évènements/logique/ce que vous voulez...
    
    Event event;
    while (work) {
        while (window.pollEvent(event)) {
            switch (event.type) {
                case Event::Closed:
                	work = false;
                    break;

                default:
                    break;
            }
        }
    }
    
    fprintf(stderr, "WAIT\n");
    thread.wait();

    fprintf(stderr, "Main: Re-Activate\n");
    window.setActive(true);

    window.close();
    fprintf(stderr, "FINISH\n");

    return 0;
}