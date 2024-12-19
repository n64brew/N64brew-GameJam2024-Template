/***************************************************************
                           results.c
                               
The file contains the results screen
***************************************************************/

#include "results.h"
#include "core.h"
#include "menu.h"
#include <libdragon.h>

#define FONT_TEXT       1

static int points_to_win;
static int global_points[4];

static bool ending;
static rdpq_font_t *font;

int results_get_points(PlyNum player)
{
    return global_points[player];
}

void results_set_points(PlyNum player, int points)
{
    global_points[player] = points;
}

int results_get_points_to_win()
{
    return points_to_win;
}

void results_set_points_to_win(int points)
{
    points_to_win = points;
}

inline bool player_has_won(PlyNum player)
{
    return results_get_points(player) >= results_get_points_to_win();
}

void results_init()
{
    // Award points to winning players
    for (PlyNum i = 0; i < MAXPLAYERS; i++) {
        if (core_get_winner(i)) {
            results_set_points(i, results_get_points(i) + 1);
        }

        if (player_has_won(i)) {
            ending = true;
        }
    }

    font = rdpq_font_load("rom:/squarewave.font64");
    rdpq_text_register_font(FONT_TEXT, font);
    rdpq_font_style(font, 0, &(rdpq_fontstyle_t){.color = RGBA32(0x6C,0xBE,0xED,0xFF), .outline_color = RGBA32(0x31,0x39,0x3C,0xFF) });

    color_t player_colors[] = {
        PLAYERCOLOR_1,
        PLAYERCOLOR_2,
        PLAYERCOLOR_3,
        PLAYERCOLOR_4
    };

    for (size_t i = 0; i < MAXPLAYERS; i++)
    {
        rdpq_font_style(font, i+1, &(rdpq_fontstyle_t){.color = player_colors[i] });
    }

    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
}

void results_loop(float deltatime)
{
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    surface_t *disp = display_get();

    rdpq_attach(disp, NULL);
    rdpq_clear(RGBA32(0xAD,0xBA,0xBD,0xFF));

    rdpq_textparms_t parms = {
        .align = ALIGN_CENTER,
        .width = 260
    };

    int ycur = 100;

    if (ending) {
        rdpq_text_printf(&parms, FONT_TEXT, 30, 50, "WINNER");
        for (PlyNum i = 0; i < MAXPLAYERS; i++)
        {
            if (player_has_won(i)) {
                ycur += rdpq_text_printf(&parms, FONT_TEXT, 30, ycur, "^0%dPlayer %d\n", i+1, i+1).advance_y;
            }
        }
    } else {
        rdpq_text_printf(&parms, FONT_TEXT, 30, 50, "RESULTS");
        for (PlyNum i = 0; i < MAXPLAYERS; i++)
        {
            ycur += rdpq_text_printf(NULL, FONT_TEXT, 30, ycur, "^0%dP%d: %d points\n", i+1, i+1, results_get_points(i)).advance_y;
        }
    }

    rdpq_text_printf(&parms, FONT_TEXT, 30, 200, "Press A to continue");

    rdpq_detach_show();

    if (btn.a) {
        if (ending) menu_reset();
        core_level_changeto(LEVEL_MINIGAMESELECT);
    }
}

void results_cleanup()
{
    rspq_wait();
    rdpq_text_unregister_font(FONT_TEXT);
    rdpq_font_free(font);
    display_close();
}
