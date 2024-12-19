/***************************************************************
                             menu.c
                               
This file contains the code for the basic menu
***************************************************************/

#include <libdragon.h>
#include <string.h>
#include "menu.h"
#include "core.h"
#include "minigame.h"
#include "config.h"
#include "results.h"


/*********************************
           Definitions
*********************************/

#define FONT_TEXT       1
#define FONT_DEBUG      2

typedef enum
{
    SCREEN_PLAYERCOUNT,
    SCREEN_AIDIFFICULTY,
    SCREEN_MINIGAME
} menu_screen;

/*==============================
    minigame_sort
    Sorts two names alphabetically
    @param  The first name
    @param  The second name
    @return -1 if a is less than b, 1 if a is greater than b, and 0 if they are equal
==============================*/

static int minigame_sort(const void *a, const void *b)
{
    int idx1 = *(int*)a, idx2 = *(int*)b;
    return strcasecmp(global_minigame_list[idx1].definition.gamename, global_minigame_list[idx2].definition.gamename);
}

/*==============================
    get_selection_offset
    Converts a joypad 8-way direction into a vertical selection offset
    @param  The joypad direction
    @return The selection offset
==============================*/

int get_selection_offset(joypad_8way_t direction)
{
    switch (direction) {
    case JOYPAD_8WAY_UP_RIGHT:
    case JOYPAD_8WAY_UP:
    case JOYPAD_8WAY_UP_LEFT:
    case JOYPAD_8WAY_LEFT:
        return -1;
    case JOYPAD_8WAY_DOWN_LEFT:
    case JOYPAD_8WAY_DOWN:
    case JOYPAD_8WAY_DOWN_RIGHT:
    case JOYPAD_8WAY_RIGHT:
        return 1;
    default:
        return 0;
    }
}

/*==============================
    get_difficulty_name
    Gets the display name of an AI difficulty level
    @param  The AI difficulty
    @return The display name
==============================*/

const char *get_difficulty_name(AiDiff difficulty)
{
    switch (difficulty)
    {
    case DIFF_EASY:
        return "Easy";
    case DIFF_MEDIUM:
        return "Medium";
    case DIFF_HARD:
        return "Hard";
    default:
        return "Unknown";
    }
}

static bool is_first_time = true;

static menu_screen current_screen;  // Current menu screen
static menu_screen targetscreen;
static int item_count;              // The number of selection items in the current screen
static const char *heading;         // The heading of the menu screen
static int select;                  // The currently selected item
static int yscroll;

static bool was_minigame = false;
static surface_t minigame_frame;

// Locals that I now made global
static bool menu_done;
static bool has_moved_selection;
static float yselect;
static float yselect_target;
static AiDiff difficulty;
static uint8_t playercount;
static color_t BLACK;
static color_t ASH_GRAY;
static color_t MAYA_BLUE;
static color_t GUN_METAL;
static color_t REDWOOD;
static color_t BREWFONT;
static color_t TEXT_COLOR;
static color_t WHITE;
static heap_stats_t heap_stats;
static int selected_minigame;
static sprite_t *logo;
static sprite_t *jam;
static sprite_t *bg_pattern;
static sprite_t *bg_gradient;
static sprite_t *btn_round;
static sprite_t *btn_wide;
static sprite_t *btn_game;
static sprite_t *slider;

static rdpq_font_t *font;
static rdpq_font_t *fontdbg;
static int* sorted_indices;

static float time;

void menu_reset()
{
    is_first_time = true;
}

/*==============================
    set_menu_screen
    Switches the menu to another screen
    @param  The new screen
==============================*/

void set_menu_screen(menu_screen screen)
{
    current_screen = screen;
    yscroll = 0;
    switch (current_screen) {
    case SCREEN_PLAYERCOUNT:
        item_count = MAXPLAYERS;
        select = core_get_playercount()-1;

        if (MAXPLAYERS == 0) {
            heading = "No controllers connected!\n";
        } else {
            heading = "How many players?\n";
        }
        break;
    case SCREEN_AIDIFFICULTY:
        item_count = DIFF_HARD+1;
        select = core_get_aidifficulty();
        heading = "AI difficulty?\n";
        break;
    case SCREEN_MINIGAME:
        item_count = global_minigame_count;
        select = 0;
        heading = "Pick a game!\n";
        break;
    }
}

/**
 * @brief Draws scrolling background
 * @param pattern texture for the checkerboard pattern
 * @param gradient gradient on the Y axis
 * @param offset scroll offset
 */
void menu_draw_bg(sprite_t* pattern, sprite_t* gradient, float offset)
{ 
  rdpq_set_mode_standard();
  rdpq_mode_begin();
    rdpq_mode_blender(0);
    rdpq_mode_alphacompare(0);
    rdpq_mode_combiner(RDPQ_COMBINER2(
      (TEX0,0,TEX1,0), (0,0,0,1),
      (COMBINED,0,PRIM,0), (0,0,0,1)
    ));
    rdpq_mode_dithering(DITHER_BAYER_BAYER);
    rdpq_mode_filter(FILTER_BILINEAR);
  rdpq_mode_end();

  float brightness = 0.75f;
  rdpq_set_prim_color((color_t){0xFF*brightness, 0xCC*brightness, 0xAA*brightness, 0xFF});

  offset = fmodf(offset, 64.0f);
  rdpq_texparms_t param_pattern = {
    .s = {.repeats = REPEAT_INFINITE, .mirror = true, .translate = offset, .scale_log = 0},
    .t = {.repeats = REPEAT_INFINITE, .mirror = true, .translate = offset, .scale_log = 0},
  };
  rdpq_texparms_t param_grad = {
    .s = {.repeats = REPEAT_INFINITE},
    .t = {.repeats = 1, .scale_log = 2},
  };
  rdpq_tex_multi_begin();
    rdpq_sprite_upload(TILE0, pattern, &param_pattern);
    rdpq_sprite_upload(TILE1, gradient, &param_grad);
  rdpq_tex_multi_end();

  rdpq_texture_rectangle(TILE0, 0,0, display_get_width(), display_get_height(), 0, 0);
}

void menu_init()
{
    time = 0.0f;
    difficulty = AI_DIFFICULTY;
    playercount = PLAYER_COUNT;
    BLACK = RGBA32(0x00,0x00,0x00,0xFF);
    ASH_GRAY = RGBA32(0xAD,0xBA,0xBD,0xFF);
    MAYA_BLUE = RGBA32(0x6C,0xBE,0xED,0xFF);
    GUN_METAL = RGBA32(0x31,0x39,0x3C,0xFF);
    REDWOOD = RGBA32(0xB2,0x3A,0x7A,0xFF);
    BREWFONT = RGBA32(242,209,155,0xFF);
    TEXT_COLOR = RGBA32(0xFF,0xDD,0xDD,0xFF);
    WHITE = RGBA32(0xFF,0xFF,0xFF,0xFF);

    sys_get_heap_stats(&heap_stats);

    yselect = -1;
    yselect_target = -1;
    has_moved_selection = false;

    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);

    core_set_aidifficulty(difficulty);
    core_set_playercount(playercount);
    results_set_points_to_win(3);

    logo = sprite_load("rom:/n64brew.ia8.sprite");
    jam = sprite_load("rom:/jam.rgba32.sprite");
    bg_pattern = sprite_load("rom:/pattern.i8.sprite");
    bg_gradient = sprite_load("rom:/gradient.i8.sprite");
    btn_round = sprite_load("rom:/btnRound.ia8.sprite");
    btn_wide = sprite_load("rom:/btnWide.ia8.sprite");
    btn_game = sprite_load("rom:/btnGame.i4.sprite");
    slider = sprite_load("rom:/slider.ia4.sprite");
    
    font = rdpq_font_load("rom:/squarewave.font64");
    rdpq_text_register_font(FONT_TEXT, font);
    rdpq_font_style(font, 0, &(rdpq_fontstyle_t){.color = TEXT_COLOR, .outline_color = GUN_METAL });
    rdpq_font_style(font, 1, &(rdpq_fontstyle_t){.color = ASH_GRAY,  .outline_color = GUN_METAL });
    rdpq_font_style(font, 2, &(rdpq_fontstyle_t){.color = WHITE, .outline_color = GUN_METAL });
    rdpq_font_style(font, 3, &(rdpq_fontstyle_t){.color = BREWFONT, .outline_color = GUN_METAL });

    fontdbg = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_VAR);
    rdpq_text_register_font(FONT_DEBUG, fontdbg);
    
    playercount = 0;
    for (int i = 0; i < MAXPLAYERS; i++)
        if (joypad_is_connected(i)) 
            playercount++;

    sorted_indices = malloc(global_minigame_count * sizeof(int));
    for (int i = 0; i < global_minigame_count; i++)
        sorted_indices[i] = i;
    qsort(sorted_indices, global_minigame_count, sizeof(int), minigame_sort);

    selected_minigame = -1;
    if (SKIP_MINIGAMESELECTION) {
        for (int i = 0; i < global_minigame_count; i++) {
            if (!strcasecmp(global_minigame_list[sorted_indices[i]].internalname, MINIGAME_TO_TEST)) {
                selected_minigame = i;
                break;
            }
        }
    }

    // Set the initial menu screen
    menu_done = false;
    targetscreen = SCREEN_MINIGAME;
    if (is_first_time)
        targetscreen = SCREEN_PLAYERCOUNT;
    if (targetscreen == SCREEN_PLAYERCOUNT && SKIP_PLAYERSELECTION)
        targetscreen = SCREEN_AIDIFFICULTY;
    if (targetscreen == SCREEN_AIDIFFICULTY && (SKIP_DIFFICULTYSELECTION || playercount == MAXPLAYERS))
        targetscreen = SCREEN_MINIGAME;
    if (targetscreen == SCREEN_MINIGAME && SKIP_MINIGAMESELECTION)
        menu_done = true;
    set_menu_screen(targetscreen);
}


void menu_loop(float deltatime)
{
    int selection_offset = get_selection_offset(joypad_get_direction(JOYPAD_PORT_1, JOYPAD_2D_ANY));
    if (selection_offset != 0) {
        if (!has_moved_selection) select += selection_offset;
        has_moved_selection = true;
    } else {
        has_moved_selection = false;
    }

    if (select < 0) select = 0;
    if (select > item_count-1) select = item_count-1;

    if (select < yscroll) {
        yscroll -= 1;
    }

    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if (btn.a) {
        switch (current_screen) {
            case SCREEN_PLAYERCOUNT:
                targetscreen = SCREEN_AIDIFFICULTY;
                if (targetscreen == SCREEN_AIDIFFICULTY && (SKIP_DIFFICULTYSELECTION || (select+1) == MAXPLAYERS))
                    targetscreen = SCREEN_MINIGAME;
                if (targetscreen == SCREEN_MINIGAME && SKIP_MINIGAMESELECTION)
                    menu_done = true;
                core_set_playercount(select+1);
                set_menu_screen(targetscreen);
                break;
            case SCREEN_AIDIFFICULTY:
                core_set_aidifficulty(select);
                if (SKIP_MINIGAMESELECTION)
                    menu_done = true;
                else
                    set_menu_screen(SCREEN_MINIGAME);
                break;
            case SCREEN_MINIGAME:
                selected_minigame = select;
                menu_done = true;
                break;
        }
    } else if (btn.b) {
        switch (current_screen) {
            case SCREEN_AIDIFFICULTY:
                set_menu_screen(SCREEN_PLAYERCOUNT);
                break;
            case SCREEN_MINIGAME:
                if (playercount == MAXPLAYERS) {
                    set_menu_screen(SCREEN_PLAYERCOUNT);
                } else {
                    set_menu_screen(SCREEN_AIDIFFICULTY);
                }
                break;
            default:
                break;
        }
    }

    time += display_get_delta_time();
    surface_t *disp = display_get();

    rdpq_attach(disp, NULL);
    menu_draw_bg(bg_pattern, bg_gradient, time * 12.0f);

    rdpq_textparms_t textparms = {
        .width = 200, .tabstops = (int16_t[]){ 15 },
        .disable_aa_fix = true,
    };
    rdpq_textparms_t textparmsCenter = {
        .align = ALIGN_CENTER,
        .width = 200,
        .disable_aa_fix = true,
    };
    rdpq_textparms_t textparmsRight = {
        .align = ALIGN_RIGHT,
        .width = 200,
        .disable_aa_fix = true,
    };

    rdpq_set_mode_standard();
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_mode_combiner(RDPQ_COMBINER1((PRIM,ENV,TEX0,ENV), (0,0,0,TEX0)));
    rdpq_set_prim_color(BREWFONT);  // fill color
    rdpq_set_env_color(BLACK);      // outline color
    rdpq_mode_filter(FILTER_BILINEAR);

    int x0 = display_get_width() / 2;
    int y0 = 38;

    if(current_screen != SCREEN_MINIGAME) 
    {
      int logo_pos_x = display_get_width() / 2;
      logo_pos_x -= (logo->width + jam->width - 20) / 2;
      int logo_pos_y = 24;

      rdpq_sprite_blit(logo, logo_pos_x, logo_pos_y, NULL);

      rdpq_blitparms_t param_jam = {
        .scale_x = 0.9f, 
        .cx = jam->width / 2,
        .cy = jam->height / 2,
        .theta = fm_sinf(time) * 0.3f
      };
      param_jam.scale_y = param_jam.scale_x;

      rdpq_set_prim_color(WHITE);
      rdpq_sprite_blit(jam, logo_pos_x + logo->width + jam->width/2, 
        logo_pos_y + 15, &param_jam);
      rdpq_mode_filter(FILTER_POINT);
      
      y0 += 20 + logo->height;

      if (yselect_target >= 0) {
          if (yselect < 0) yselect = yselect_target;
          yselect = yselect * 0.7 + yselect_target * 0.3;
      }
    }

    if(current_screen == SCREEN_PLAYERCOUNT || current_screen == SCREEN_AIDIFFICULTY) 
    {
      int ycur = y0;
      ycur += rdpq_text_print(&textparmsCenter, FONT_TEXT, x0-100, ycur, heading).advance_y;
      ycur += 4;

      bool is_ai = current_screen == SCREEN_AIDIFFICULTY;
      sprite_t *btn = is_ai ? btn_wide : btn_round;

      rdpq_set_mode_standard();
      rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
      rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
      rdpq_mode_filter(FILTER_BILINEAR);
      rdpq_sprite_upload(TILE0, btn, &(rdpq_texparms_t){.s.repeats = 1, .t.repeats = 1});

      int posXBase = (display_get_width() / 2) 
        - (btn->width*item_count/2) 
        + btn->width/2;

      int posX = posXBase;
      int posY = 120;

      for(int p=0; p<item_count; ++p) 
      {  
        float btnScale = 0.8f;
        if(select == p) {
          btnScale = 1.0f + (fm_sinf(time * 4.0f) * 0.09f);
        }
        float half_size_x = btn->width * 0.5f * btnScale;
        float half_size_y = btn->height * 0.5f * btnScale;

        rdpq_set_prim_color(select == p ? TEXT_COLOR : ASH_GRAY);
        rdpq_texture_rectangle_scaled(TILE0, 
          posX - half_size_x, posY - half_size_y, 
          posX + half_size_x, posY + half_size_y,
          -1.5f, -1.5f, btn->width, btn->height
        );
        posX += btn->width;
      }

      rdpq_set_mode_standard();
      rspq_wait();

      posX = posXBase;
      for(int p=0; p<item_count; ++p) {
        if(is_ai) {
          rdpq_text_printf(&textparmsCenter, FONT_TEXT, posX-100+1, posY+3, 
            select == p ? "^00%s" : "^01%s", get_difficulty_name(p)
          );
        } else {
          rdpq_text_printf(&textparmsCenter, FONT_TEXT, posX-100+1, posY+3, 
            select == p ? "^00%d" : "^01%d", p+1
          );
        }
        posX += btn->width;
      }
    }

    if(current_screen == SCREEN_MINIGAME) {
      int ycur = y0;

      rdpq_set_mode_standard();
      rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
      rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
      rdpq_mode_filter(FILTER_BILINEAR);

      rdpq_tex_multi_begin();
        rdpq_sprite_upload(TILE0, btn_game, &(rdpq_texparms_t){
          .s.repeats = 1, .t.repeats = 1,
        });
        rdpq_tex_reuse_sub(TILE1, &(rdpq_texparms_t){
          .s.repeats = 1, .t.repeats = 1,
        }, 0, 0, 16, 16);
      rdpq_tex_multi_end();
      
      int text_i = yscroll;
      int text_count = 0;
      float pos_x = display_get_width() / 2;

      for (int i = yscroll; i < item_count; i++) {
            if (ycur > 100) {
                if (select == i) {
                    yscroll += 1;
                }
                break;
            }

            ++text_count;
            if (select == i) yselect_target = ycur;

            rdpq_set_prim_color(select == i ? TEXT_COLOR : ASH_GRAY);

            float btnScale = 1.0f;
            if(select == i) {
              btnScale = 1.0f + (fm_sinf(time * 4.0f) * 0.04f);
            }

            int button_width = 80;
            float half_size_x = button_width * btnScale;
            float half_size_y = btn_game->height * 0.5f * btnScale;

            // left side
            rdpq_texture_rectangle_scaled(TILE0, 
              pos_x - half_size_x + 0, ycur - half_size_y, 
              pos_x + half_size_x - 8, ycur + half_size_y,
              0, 0, button_width * 2 - 8, btn_game->height
            );
            // end piece (flipped on X)
            rdpq_texture_rectangle_scaled(TILE0, 
              pos_x + half_size_x - 8, ycur - half_size_y, 
              pos_x + half_size_x + 0, ycur + half_size_y,
              8, 0, 0, btn_game->height
            );

            ycur += 24;
        }

        // Description box background
        ycur = y0 + 64;
        int rect_width = 260;
        int rect_height = 100;
        pos_x = display_get_width() / 2 - (rect_width/2)-8;

        rdpq_set_prim_color(ASH_GRAY);
        rdpq_texture_rectangle(TILE1, // left, top
          pos_x, ycur, 
          pos_x + rect_width, ycur+rect_height, 0, 0);
        rdpq_texture_rectangle(TILE1, // right, top
          pos_x + rect_width + 14, ycur, 
          pos_x + rect_width -  1, ycur+rect_height, 0, 0);
        rdpq_texture_rectangle(TILE1, // left, bottom
          pos_x,              ycur + rect_height + 14, 
          pos_x + rect_width, ycur + rect_height - 1, 0, 0);
        rdpq_texture_rectangle(TILE1, // right, bottom
          pos_x + rect_width + 14, ycur + rect_height + 14, 
          pos_x + rect_width -  1, ycur + rect_height - 1, 0, 0);
      
        
        for(int s=0; s<2; ++s) 
        {
          // Slider
          float slider_y = y0-10;
          float slider_x = s == 0 ? 60 : (display_get_width() - 60 - slider->width);
          rdpq_sprite_upload(TILE0, slider, &(rdpq_texparms_t){
            .s.repeats = 1, .t.repeats = 1,
          });
          rdpq_set_prim_color((color_t){0xAA, 0xAA, 0xAA, 0x99});
          rdpq_texture_rectangle(TILE0, 
            slider_x, slider_y, slider_x + slider->width, slider_y + slider->height, 0, 0
          );
          // point
          rdpq_set_prim_color(WHITE);
          slider_y += (float)select / item_count * (slider->height);
          rdpq_texture_rectangle(TILE0, 
            slider_x, slider_y, slider_x + slider->width, slider_y + 3.5f, 0, 0 
          );
        }

        // Show the description of the selected minigame
        Minigame *cur = &global_minigame_list[sorted_indices[select]];
        rdpq_textparms_t parms = {
            .width = rect_width + 10, 
            .wrap = WRAP_WORD,
            .align = ALIGN_RIGHT,
            .disable_aa_fix = true
        };

        rdpq_set_mode_standard();

        rdpq_text_printf(&parms, FONT_TEXT, pos_x-4, ycur+rect_height+2, "^03© %s\n", cur->definition.developername);
        parms.align = ALIGN_LEFT;
        parms.width = rect_width;

        ycur += 16;
        ycur += rdpq_text_printf(&parms, FONT_TEXT, pos_x+10, ycur, "%s\n", cur->definition.description).advance_y;
        ycur += 6;
        ycur += rdpq_text_printf(&parms, FONT_TEXT, pos_x+10, ycur, "^02%s\n", cur->definition.instructions).advance_y;

      // Minigame nanes in the list
        pos_x = display_get_width() / 2;
        ycur = y0;
        for(int i=0; i<text_count; ++i) {
          int global_i = text_i+i;
          rdpq_text_printf(&textparmsCenter, FONT_TEXT, 
            pos_x-100, ycur+3, 
            select == global_i ? "^00%s" : "^01%s",
            global_minigame_list[sorted_indices[global_i]].definition.gamename
          );
          ycur += 24;
        }
    }

    if (true) {
        rdpq_text_printf(NULL, FONT_DEBUG, 10, 15, 
            "Mem: %d KiB", heap_stats.used/1024);
    }
    rdpq_detach_show();

    if (menu_done)
    {
        minigame_loadnext(global_minigame_list[sorted_indices[selected_minigame]].internalname);
        core_level_changeto(LEVEL_MINIGAME);
    }
}

void menu_cleanup()
{
    is_first_time = false;
    free(sorted_indices);
    rspq_wait();
    
    sprite_free(jam);
    sprite_free(logo);
    sprite_free(bg_pattern);
    sprite_free(bg_gradient);
    sprite_free(btn_round);
    sprite_free(btn_wide);
    sprite_free(btn_game);
    sprite_free(slider);

    rdpq_text_unregister_font(FONT_TEXT);
    rdpq_text_unregister_font(FONT_DEBUG);
    rdpq_font_free(font);
    rdpq_font_free(fontdbg);
    display_close();
}

void menu_copy_minigame_frame()
{
    surface_t frame = display_get_current_framebuffer();

    was_minigame = true;
    
    surface_free(&minigame_frame);
    minigame_frame = surface_alloc(surface_get_format(&frame), frame.width, frame.height);
    memcpy(minigame_frame.buffer, frame.buffer, frame.stride * frame.height);
}
