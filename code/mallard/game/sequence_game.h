#ifndef SEQUENCE_GAME_H
#define SEQUENCE_GAME_H

#define GAME_FADE_IN_DURATION 1.0f
#define GAME_EXIT_DURATION 2.0f
#define GAME_EXIT_THRESHOLD_DURATION 0.1f

extern bool sequence_game_finished;

typedef enum
{
    LEFT = 0,
    RIGHT = 1,
} Direction;

typedef enum
{
    BASE = 0,
    WALK = 1,
    IDLE = 2,
    SLAP = 3,
    RUN = 4,
} Action;

struct Character
{
    float x;
    float y;
    Action action;
    Direction direction;
    sprite_t *base_sprite;
    sprite_t *walk_sprite;
    sprite_t *slap_sprite;
    sprite_t *run_sprite;
    int frames;
    int locked_for_frames;
};

struct Controller
{
    unsigned int start_down;
    unsigned int start_up;
    float start_held_elapsed;
};

void sequence_game(float deltatime);

#endif // SEQUENCE_GAME_H
