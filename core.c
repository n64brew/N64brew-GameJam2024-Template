/***************************************************************
                             core.c
                               
The file contains Core minigame code, which handles stuff like
player, AI, and game loop information.
***************************************************************/

#include <libdragon.h>
#include "core.h"
#include "minigame.h"
#include "config.h"
#include "menu.h"
#include "results.h"


/*********************************
            Structures
*********************************/

typedef struct {
    PlyNum number;
    joypad_port_t port;
} Player;


/*********************************
             Globals
*********************************/

// Player info
static Player   global_core_players[JOYPAD_PORT_COUNT];
static uint32_t global_core_playercount;
static AiDiff   global_core_aidifficulty = AI_DIFFICULTY;

// Minigame info
static bool global_core_playeriswinner[MAXPLAYERS];

// Core info
static double global_core_subtick = 0;

// Level info
static Level* global_core_curlevel;
static Level* global_core_nextlevel = NULL;
static Level global_core_alllevels[LEVELCOUNT];


/*==============================
    core_get_subtick
    Gets the current subtick. Use this to help smooth
    movements in your draw loop
    @return The current subtick
==============================*/

void core_set_subtick(double subtick)
{
    global_core_subtick = subtick;
}


/*==============================
    core_set_playercount
    Sets the number of human players
    @param  The number of players
==============================*/

void core_set_playercount(uint32_t playercount)
{
    int lastcont = 0;

    // Attempt to find the first N left-most available controllers
    for (int i=0; i<playercount; i++)
    {
        bool found = false;
        for (int j=lastcont; j<JOYPAD_PORT_COUNT; j++)
        {
            if (joypad_is_connected(j))
            {
                global_core_players[i].port = j;
                found = true;
                lastcont = ++j;
                break;
            }
        }
        assertf(found, "Unable to find an available controller for player %d\n", i+1);
    }
    global_core_playercount = playercount;
}

/*==============================
    core_set_aidifficulty
    Sets the AI difficulty
    @param  The AI difficulty
==============================*/

void core_set_aidifficulty(AiDiff difficulty)
{
    global_core_aidifficulty = difficulty;
}


/*==============================
    core_get_winner
    Returns whether a player has won the last minigame.
    @param  The player to query
    @return True if the player has won, false otherwise.
==============================*/

bool core_get_winner(PlyNum ply)
{
    return global_core_playeriswinner[ply];
}

/*==============================
    core_set_winner
    Set the winner of the minigame. You can call this
    multiple times to set multiple winners.
    @param  The winning player
==============================*/

void core_set_winner(PlyNum ply)
{
    global_core_playeriswinner[ply] = true;
}


/*==============================
    core_get_aidifficulty
    Gets the current AI difficulty
    @return The AI difficulty
==============================*/

AiDiff core_get_aidifficulty()
{
    return global_core_aidifficulty;
}


/*==============================
    core_get_subtick
    Gets the subtick of the current frame
    @return The frame's subtick
==============================*/

double core_get_subtick()
{
    return global_core_subtick;
}


/*==============================
    core_get_playercount
    Get the number of human players
    @return The number of players
==============================*/

uint32_t core_get_playercount()
{
    return global_core_playercount;
}


/*==============================
    core_get_playercontroller
    Get the controller port of this player.
    Because player 1's controller might not be plugged 
    into port number 1.
    @param  The player we want
    @return The controller port
==============================*/

joypad_port_t core_get_playercontroller(PlyNum ply)
{
    return global_core_players[ply].port;
}


/*==============================
    core_reset_winners
    Resets the winners
==============================*/

void core_reset_winners()
{
    for (int i=0; i<MAXPLAYERS; i++)
        global_core_playeriswinner[i] = false;
}


/*==============================
    core_initlevels
    Initializes the levels struct
==============================*/

void core_initlevels()
{
    global_core_nextlevel = NULL;
    global_core_curlevel = NULL;

    //global_core_alllevels[LEVEL_BOOT].funcPointer_init      = NULL;
    //global_core_alllevels[LEVEL_BOOT].funcPointer_loop      = NULL;
    //global_core_alllevels[LEVEL_BOOT].funcPointer_fixedloop = NULL;
    //global_core_alllevels[LEVEL_BOOT].funcPointer_cleanup   = NULL;

    global_core_alllevels[LEVEL_MINIGAMESELECT].funcPointer_init      = menu_init;
    global_core_alllevels[LEVEL_MINIGAMESELECT].funcPointer_loop      = menu_loop;
    global_core_alllevels[LEVEL_MINIGAMESELECT].funcPointer_fixedloop = NULL;
    global_core_alllevels[LEVEL_MINIGAMESELECT].funcPointer_cleanup   = menu_cleanup;

    global_core_alllevels[LEVEL_RESULTS].funcPointer_init      = results_init;
    global_core_alllevels[LEVEL_RESULTS].funcPointer_loop      = results_loop;
    global_core_alllevels[LEVEL_RESULTS].funcPointer_fixedloop = NULL;
    global_core_alllevels[LEVEL_RESULTS].funcPointer_cleanup   = results_cleanup;
}


/*==============================
    core_level_changeto
    Changes the level
    @param  The level to change to
==============================*/

void core_level_changeto(LevelDef level)
{
    if (level == LEVEL_MINIGAME)
    {
        global_core_alllevels[LEVEL_MINIGAME].funcPointer_init = minigame_get_game()->funcPointer_init;      
        global_core_alllevels[LEVEL_MINIGAME].funcPointer_loop = minigame_get_game()->funcPointer_loop;      
        global_core_alllevels[LEVEL_MINIGAME].funcPointer_fixedloop = minigame_get_game()->funcPointer_fixedloop; 
        global_core_alllevels[LEVEL_MINIGAME].funcPointer_cleanup = minigame_get_game()->funcPointer_cleanup;   
    }
    global_core_nextlevel = &global_core_alllevels[level];
}


/*==============================
    core_level_doinit
    Calls the level's init function
==============================*/

void core_level_doinit()
{
    if (global_core_nextlevel != NULL)
    {
        global_core_curlevel = global_core_nextlevel;
        global_core_nextlevel = NULL;
    }

    if (global_core_curlevel == &global_core_alllevels[LEVEL_MINIGAME])
        core_reset_winners();
    if (global_core_curlevel->funcPointer_init)
        global_core_curlevel->funcPointer_init();
}


/*==============================
    core_level_doloop
    Calls the level's loop function
    @param  The deltatime
==============================*/

void core_level_doloop(float deltatime)
{
    if (global_core_curlevel->funcPointer_loop)
        global_core_curlevel->funcPointer_loop(deltatime);
}


/*==============================
    core_level_dofixedloop
    Calls the level's fixed loop function
    @param  The deltatime
==============================*/

void core_level_dofixedloop(float deltatime)
{
    if (global_core_curlevel->funcPointer_fixedloop)
        global_core_curlevel->funcPointer_fixedloop(deltatime);
}


/*==============================
    core_level_docleanup
    Calls the level's cleanup function
==============================*/

void core_level_docleanup()
{
    rspq_wait();
    for (int i=0; i<32; i++)
        mixer_ch_stop(i);
    //menu_copy_minigame_frame();
    if (global_core_curlevel->funcPointer_cleanup)
        global_core_curlevel->funcPointer_cleanup();
    if (global_core_curlevel == &global_core_alllevels[LEVEL_MINIGAME])
        minigame_cleanup();
    mixer_close();
    mixer_init(32);
}


/*==============================
    core_level_waschanged
    Checks if the level was recently changed
    @return If the level was recently changed
==============================*/

bool core_level_waschanged()
{
    return global_core_nextlevel != NULL;
}