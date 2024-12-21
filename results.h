#ifndef GAMEJAM2024_RESULTS_H
#define GAMEJAM2024_RESULTS_H

#include "core.h"

    /***************************************************************
              You have no reason to be incuding this file
    ***************************************************************/

    int results_get_points(PlyNum player);
    void results_set_points(PlyNum player, int points);
    int results_get_points_to_win();
    void results_set_points_to_win(int points);

    void results_init();
    void results_loop(float deltatime);
    void results_cleanup();

#endif
