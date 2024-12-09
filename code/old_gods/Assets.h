/*================
ASSETS_H
Contains paths to the assets to be loaded
================*/
#ifndef ASSETS_H
#define ASSETS_H

// ================ MODELS ==============
#define MODEL_COUNT 9
extern const char *model_paths[MODEL_COUNT];
#define MODEL_BOX 0         //"rom:/old_gods/box.t3dm",        
#define MODEL_FOOD 1        //"rom:/old_gods/food.t3dm",       
#define MODEL_MAP 2         //"rom:/old_gods/map.t3dm",       
#define MODEL_SNAKE 3       //"rom:/old_gods/snake.t3dm",     
#define MODEL_RAT 4
#define MODEL_FOAM 5
#define MODEL_CYLINDER 6
#define MODEL_TORUS 7
#define MODEL_TRAIL 8


// ================ TEXTURES ==============
#define TEXTURE_COUNT 11
extern const char *texture_path[TEXTURE_COUNT];

// Define for texture IDs
#define TEXTURE_ID_0 0      // "rom:/old_gods/panel_white_64_64.sprite"  
#define TEXTURE_ID_1 1      // "rom:/old_gods/panel_pink_64_64.sprite"  
#define TEXTURE_ID_2 2      // "rom:/old_gods/player1_panel_64_64.sprite"
#define TEXTURE_ID_3 3      // "rom:/old_gods/player2_panel_64_64.sprite"
#define TEXTURE_ID_4 4      // "rom:/old_gods/player3_panel_64_64.sprite"
#define TEXTURE_ID_5 5      // "rom:/old_gods/player4_panel_64_64.sprite"

extern const char* animatedSpritePath;

// ================ AUDIO ==============
//extern const char* cannonFXPath;
//extern const char* laserFXPath;
//extern const char* musicFXPath;
extern const char* feedGodSoundFXPath;
extern const char* pickupSoundFXPath;

// ================ FONT ==============
extern const char* fontPath2;
extern const char* fontPath3;
extern const char* fontPath4;
extern const char* fontPath5;
#define FONT2_ID 2
#define FONT3_ID 3
#define FONT4_ID 4
#define FONT5_ID 5



#endif  // ASSETS_H
