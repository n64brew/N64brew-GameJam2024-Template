#include <libdragon.h>
#include "../../../core.h"
#include "../../../minigame.h"
#include "sequence_game.h"
#include "sequence_game_input.h"
#include "sequence_game_initialize.h"
#include "sequence_game_graphics.h"

#define BOOST 2.0
#define SQRT_ONE_HALF 0.70710678118

void sequence_game_update(float deltatime)
{
    for (size_t i = 0; i < core_get_playercount(); i++)
    {
        struct Controller *controller = &controllers[i];
        struct Duck *character = &characters[i];

        character->frames++;

        joypad_port_t controllerPort = core_get_playercontroller(i);
        joypad_buttons_t pressed = joypad_get_buttons_pressed(controllerPort);
        joypad_buttons_t held = joypad_get_buttons_held(controllerPort);
        joypad_buttons_t released = joypad_get_buttons_released(controllerPort);
        joypad_8way_t direction = joypad_get_direction(controllerPort, JOYPAD_2D_ANY);

        if (!joypad_is_connected(controllerPort))
        {
            continue;
        }

        // Pause
        if (sequence_game_paused == false && pressed.start)
        {
            controller->start_down = 1;
        }
        if (sequence_game_paused == false && controller->start_down == 1 && released.start)
        {
            controller->start_up = 1;
        }
        if (sequence_game_paused == false && controller->start_up == 1 && controller->start_down == 1)
        {
            sequence_game_paused = true;

            for (size_t i = 0; i < core_get_playercount(); i++)
            {
                controllers->start_down = 0;
                controllers->start_up = 0;
                controllers->start_held_elapsed = 0.0f;
            }
        }

        // Unpause
        if (sequence_game_paused == true && pressed.start)
        {
            controller->start_down = 1;
        }
        if (sequence_game_paused == true && controller->start_down == 1 && released.start)
        {
            controller->start_up = 1;
        }
        if (sequence_game_paused == true && controller->start_up == 1 && controller->start_down == 1)
        {
            if (sequence_game_start_held_elapsed == 0.0f)
            {
                sequence_game_paused = false;
            }

            for (size_t i = 0; i < core_get_playercount(); i++)
            {
                controllers->start_down = 0;
                controllers->start_up = 0;
                controllers->start_held_elapsed = 0.0f;
            }
        }

        // Quit
        if (sequence_game_paused == true)
        {
            if (held.start == true)
            {
                if (sequence_game_player_holding_start == -1)
                {
                    sequence_game_player_holding_start = i;
                }

                if (sequence_game_player_holding_start == i)
                {
                    if (sequence_game_start_held_elapsed >= GAME_EXIT_DURATION)
                    {
                        sequence_game_finished = true;
                    }
                    if (controller->start_held_elapsed >= GAME_EXIT_THRESHOLD_DURATION)
                    {
                        sequence_game_start_held_elapsed += deltatime;
                    }
                    controller->start_held_elapsed += deltatime;
                }
            }
            else
            {
                controller->start_held_elapsed = 0.0f;
                if (sequence_game_player_holding_start == i)
                {
                    sequence_game_player_holding_start = -1;
                    sequence_game_start_held_elapsed = 0.0f;
                }
            }
        }

        // Movement
        switch (direction)
        {
        case JOYPAD_8WAY_UP:

            // Position
            if (held.b)
            {
                character->y -= 1 * BOOST;
            }
            else
            {
                character->y -= 1;
            }
            // Action
            if (character->locked_for_frames == 0)
            {
                if (held.b)
                {
                    character->action = DUCK_RUN;
                }
                else
                {
                    character->action = DUCK_WALK;
                }
            }
            break;

        case JOYPAD_8WAY_UP_RIGHT:

            // Position
            if (held.b)
            {
                character->x += (SQRT_ONE_HALF * BOOST);
                character->y -= (SQRT_ONE_HALF * BOOST);
            }
            else
            {
                character->x += SQRT_ONE_HALF;
                character->y -= SQRT_ONE_HALF;
            }
            // Action
            if (character->locked_for_frames == 0)
            {
                if (held.b)
                {
                    character->action = DUCK_RUN;
                }
                else
                {
                    character->action = DUCK_WALK;
                }
            }
            // Direction
            character->direction = RIGHT;
            break;

        case JOYPAD_8WAY_RIGHT:

            // Position
            if (held.b)
            {
                character->x += 1 * BOOST;
            }
            else
            {
                character->x += 1;
            }
            // Action
            if (character->locked_for_frames == 0)
            {
                if (held.b)
                {
                    character->action = DUCK_RUN;
                }
                else
                {
                    character->action = DUCK_WALK;
                }
            }
            // Direction
            character->direction = RIGHT;
            break;

        case JOYPAD_8WAY_DOWN_RIGHT:

            // Position
            if (held.b)
            {
                character->x += (SQRT_ONE_HALF * BOOST);
                character->y += (SQRT_ONE_HALF * BOOST);
            }
            else
            {
                character->x += SQRT_ONE_HALF;
                character->y += SQRT_ONE_HALF;
            }
            // Action
            if (character->locked_for_frames == 0)
            {
                if (held.b)
                {
                    character->action = DUCK_RUN;
                }
                else
                {
                    character->action = DUCK_WALK;
                }
            }
            // Direction
            character->direction = RIGHT;
            break;

        case JOYPAD_8WAY_DOWN:

            // Position
            if (held.b)
            {
                character->y += 1 * BOOST;
            }
            else
            {
                character->y += 1;
            }
            // Action
            if (character->locked_for_frames == 0)
            {
                if (held.b)
                {
                    character->action = DUCK_RUN;
                }
                else
                {
                    character->action = DUCK_WALK;
                }
            }
            break;

        case JOYPAD_8WAY_DOWN_LEFT:

            // Position
            if (held.b)
            {
                character->x -= (SQRT_ONE_HALF * BOOST);
                character->y += (SQRT_ONE_HALF * BOOST);
            }
            else
            {
                character->x -= SQRT_ONE_HALF;
                character->y += SQRT_ONE_HALF;
            }
            // Action
            if (character->locked_for_frames == 0)
            {
                if (held.b)
                {
                    character->action = DUCK_RUN;
                }
                else
                {
                    character->action = DUCK_WALK;
                }
            }
            // Direction
            character->direction = LEFT;
            break;

        case JOYPAD_8WAY_LEFT:

            // Position
            if (held.b)
            {
                character->x -= 1 * BOOST;
            }
            else
            {
                character->x -= 1;
            }
            // Action
            if (character->locked_for_frames == 0)
            {
                if (held.b)
                {
                    character->action = DUCK_RUN;
                }
                else
                {
                    character->action = DUCK_WALK;
                }
            }
            // Direction
            character->direction = LEFT;
            break;

        case JOYPAD_8WAY_UP_LEFT:

            // Position
            if (held.b)
            {
                character->x -= (SQRT_ONE_HALF * BOOST);
                character->y -= (SQRT_ONE_HALF * BOOST);
            }
            else
            {
                character->x -= SQRT_ONE_HALF;
                character->y -= SQRT_ONE_HALF;
            }
            // Action
            if (character->locked_for_frames == 0)
            {
                if (held.b)
                {
                    character->action = DUCK_RUN;
                }
                else
                {
                    character->action = DUCK_WALK;
                }
            }
            // Direction
            character->direction = LEFT;
            break;

        default:
            if (character->locked_for_frames == 0)
                character->action = DUCK_BASE;
            break;
        }

        if (character->x > MAX_X)
        {
            character->x = MAX_X;
        }

        if (character->x < MIN_X)
        {
            character->x = MIN_X;
        }

        if (character->y > MAX_Y)
        {
            character->y = MAX_Y;
        }

        if (character->y < MIN_Y)
        {
            character->y = MIN_Y;
        }

        if (pressed.a)
        {
            character->locked_for_frames = 4 * SEQUENCE_GAME_MALLARD_SLAP_FRAMES; // Lock for 12 frames.
            character->action = DUCK_SLAP;
            character->frames = 0;
        }

        if (character->locked_for_frames > 0)
        {
            character->locked_for_frames--;
        }
    }
}