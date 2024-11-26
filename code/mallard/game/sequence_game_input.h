#ifndef SEQUENCE_GAME_INPUT_H
#define SEQUENCE_GAME_INPUT_H

#include <libdragon.h> // Can i remove this????

extern bool sequence_game_finished;
extern bool sequence_game_paused;
extern int sequence_game_player_holding_start;

extern float sequence_game_start_held_elapsed;

extern struct Character *characters;

void sequence_game_update(float deltatime);

#endif // SEQUENCE_GAME_INPUT_H
