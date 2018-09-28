Logger :
    --logLvl [0-3]
        0: debug
        1: info
        2: warn
        3: error

    Dans logger.h, fonction add flag et enable flag

    --dbg
        ex:
            --dbg main :
            logger->inf(LOG_MAIN, "test");
            tous les flags dans main reporterons

        --dbg collision
            permet de debug les collisions

List Chainées:
    Voir Fichier Example example/list.c