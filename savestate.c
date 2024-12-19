/***************************************************************
                          savestate.c
                               
The file contains a savestate system so that the game can be
loaded if it crashes
***************************************************************/

#include <libdragon.h>
#include "core.h"
#include "minigame.h"
#include "results.h"
#include "savestate.h"


/*********************************
            Structures
*********************************/

typedef struct {
    char header[4];
    uint32_t blacklist;
    uint8_t crashedflag;
    uint8_t playercount;
    uint8_t aidiff;
    uint8_t pointstowin;
    uint8_t points[4];
    uint8_t nextplaystyle;
    uint8_t chooser;
    uint8_t curgame;
    uint8_t checksum;
} GameSave;


/*********************************
             Globals
*********************************/

static uint8_t global_cansave;
static GameSave global_gamesave;


/*==============================
    calc_checksum
    Calculate a basic checksum for the save state
    This is done by just adding all the bytes together
    @return The checksum
==============================*/

static uint8_t calc_checksum()
{
    uint8_t checksum = 0;
    uint8_t* asarray = (uint8_t*)&global_gamesave;
    for (int i=0; i<sizeof(GameSave); i++)
        checksum += asarray[i];
    return checksum;
}


/*==============================
    savestate_test
    Test that EEPROM is present to save a game state to
    @return Whether EEPROM is present
==============================*/

char savestate_initialize()
{
    global_cansave = 0;
    
    // Test for EEPROM
    if (eeprom_present() == EEPROM_NONE)
        return 0;
    global_cansave = 1;
        
    // Read the savestate from EEPROM
    eeprom_read_bytes((uint8_t*)(&global_gamesave), 0, sizeof(GameSave));
   
    // If the EEPROM hasn't been initialized before, do so now
    if (strncmp(global_gamesave.header, "NBGJ", 4) != 0)
    {
        memset(&global_gamesave, 0, sizeof(GameSave));
        global_gamesave.header[0] = 'N';
        global_gamesave.header[1] = 'B';
        global_gamesave.header[2] = 'G';
        global_gamesave.header[3] = 'J';
        global_gamesave.checksum = calc_checksum();
    }
    
    // Success
    return 1;
}


/*==============================
    savestate_checkcrashed
    Check if the game recently crashed
    @return Whether the game recently crashed
==============================*/

char savestate_checkcrashed()
{
    return global_gamesave.crashedflag;
}


/*==============================
    savestate_save
    Save the current game state to EEPROM
==============================*/

void savestate_save()
{
    if (!global_cansave)
        return;
    
    // Grab the game state
    global_gamesave.playercount = core_get_playercount();
    global_gamesave.aidiff = core_get_aidifficulty();
    global_gamesave.pointstowin = results_get_points_to_win();
    global_gamesave.points[0] = results_get_points(PLAYER_1);
    global_gamesave.points[1] = results_get_points(PLAYER_2);
    global_gamesave.points[2] = results_get_points(PLAYER_3);
    global_gamesave.points[3] = results_get_points(PLAYER_4);
    global_gamesave.nextplaystyle = 0; // TODO
    global_gamesave.chooser = 0; // TODO
    global_gamesave.curgame = 0; // TODO
    global_gamesave.checksum = calc_checksum();
    
    // Save to EEPROM
    eeprom_write_bytes((uint8_t*)(&global_gamesave), 0, sizeof(GameSave));
}


/*==============================
    savestate_load
    Load the game state saved in EEPROM
==============================*/

void savestate_load()
{
    if (!global_cansave)
        return;
        
    // Recover the game state
    core_set_playercount(global_gamesave.playercount);
    core_set_aidifficulty(global_gamesave.aidiff);
    results_set_points_to_win(global_gamesave.pointstowin);
    results_set_points(PLAYER_1, global_gamesave.points[0]);
    results_set_points(PLAYER_2, global_gamesave.points[1]);
    results_set_points(PLAYER_3, global_gamesave.points[2]);
    results_set_points(PLAYER_4, global_gamesave.points[3]);
    //global_gamesave.nextplaystyle; // TODO
    //global_gamesave.chooser; // TODO
    //global_gamesave.curgame; // TODO
}


/*==============================
    savestate_clear
    Clear the game state saved in EEPROM
==============================*/

void savestate_clear()
{
    if (!global_cansave)
        return;
    global_gamesave.crashedflag = 0;
    eeprom_write_bytes((uint8_t*)(&global_gamesave), 0, sizeof(GameSave));
}