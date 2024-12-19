#ifndef GAMEJAM2024_SAVESTATE_H
#define GAMEJAM2024_SAVESTATE_H

    /*==============================
        savestate_initialize
        Initialize the savestate system and return whether EEPROM exists
        @return Whether EEPROM is present
    ==============================*/
    extern char savestate_initialize();
    
    /*==============================
        savestate_checkcrashed
        Check if the game recently crashed
        @return Whether the game recently crashed
    ==============================*/
    extern char savestate_checkcrashed();
    
    /*==============================
        savestate_save
        Save the current game state to EEPROM
    ==============================*/
    extern void savestate_save();
    
    /*==============================
        savestate_load
        Load the game state saved in EEPROM
    ==============================*/
    extern void savestate_load();
    
    /*==============================
        savestate_clear
        Clear the game state saved in EEPROM
    ==============================*/
    extern void savestate_clear();

#endif