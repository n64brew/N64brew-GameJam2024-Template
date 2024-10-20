#ifndef UI_H
#define UI_H

#include <libdragon.h>

#ifdef __cplusplus
extern "C" {
#endif

enum ColorNames
{
    // Standard ROYGBIV
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
    INDIGO,
    VIOLET,
    // RGB 0 (full black) & 1 (full white)
    BLACK,
    WHITE,
    // RGB 1 * (n*.25f)
    LIGHT_GREY, // n = 3
    GREY, // n = 2
    DARK_GREY, // n = 1
    // Transparent Colors
    TRANSPARENT,
    T_RED,
    T_ORANGE,
    T_YELLOW,
    T_GREEN,
    T_BLUE,
    T_INDIGO,
    T_VIOLET,
    T_BLACK,
    T_WHITE,
    T_GREY,
    // Darker Variants
    DARK_RED,
    DARK_GREEN,
    // N64 Logo Colors
    N_RED,
    N_YELLOW,
    N_GREEN,
    N_BLUE,
    COLOR_COUNT
};

const uint32_t COLORS[COLOR_COUNT] = 
{
    0xD90000FF, // RED
    0xFF6822FF, // ORANGE
    0xFFDA21FF, // YELLOW
    0x33DD00FF, // GREEN
    0x1133CCFF, // BLUE
    0x220066FF, // INDIGO
    0x330044FF, // VIOLET
    0x000000FF, // BLACK
    0xFFFFFFFF, // WHITE
    0xC0C0C0FF, // LIGHT_GREY  
    0x808080FF, // GREY        
    0x404040FF, // DARK_GREY   
    0x0000007F, // TRANSPARENT 
    0xD90000C8, // T_RED       
    0xFF6822C8, // T_ORANGE    
    0xFFDA21C8, // T_YELLOW    
    0x33DD00C8, // T_GREEN     
    0x1133CCC8, // T_BLUE      
    0x220066C8, // T_INDIGO    
    0x330044C8, // T_VIOLET    
    0x1F1F1FC8, // T_BLACK     
    0xFFFFFFC8, // T_WHITE     
    0xC0C0C0C8, // T_GREY      
    0x820000FF, // DARK_RED    
    0x006400FF, // DARK_GREEN  
    0xE10916FF, // N_RED       
    0xF5B201FF, // N_YELLOW    
    0x319900FF, // N_GREEN     
    0x01009AFF, // N_BLUE      
};

#define FONT_ID_GAME 2
enum FONT_IDS 
{
    ID_RESERVED,
    ID_DEBUG,
    ID_DEFAULT,
    ID_TITLE,
    ID_COUNT
};
enum FONT_STYLES 
{
    STYLE_DEBUG,
    STYLE_DEFAULT,
    STYLE_TITLE,
    STYLE_BRIGHT,
    STYLE_GREEN,
    STYLE_COUNT
};

T3DVec3 fpsPos = {{32.0f,32.0f,1.0f}};
rdpq_textparms_t txt_debugParms;
rdpq_textparms_t txt_titleParms;
rdpq_textparms_t txt_gameParms;
rdpq_fontstyle_t txt_debug_fontStyle;
rdpq_fontstyle_t txt_title_fontStyle;
rdpq_fontstyle_t txt_game_fontStyle;
rdpq_fontstyle_t txt_bright_fontStyle;
rdpq_fontstyle_t txt_green_fontStyle;

void ui_register_fonts(void)
{
    // Load font64 and register to fonts starting at index 2, 0 reserved by the SDK, 1 should be reserved debug output
    rdpq_font_t *font[ID_COUNT];
    font[ID_DEBUG] = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_VAR);;
    font[ID_DEFAULT] = rdpq_font_load("rom:/game/TitanOne-Regular.font64");
    font[ID_TITLE] = rdpq_font_load("rom:/game/chunkysans.font64");

    // Create and register font styles
    txt_debug_fontStyle.color = color_from_packed32(COLORS[YELLOW]);
    txt_debug_fontStyle.outline_color = color_from_packed32(COLORS[BLACK]);

    txt_game_fontStyle.color = color_from_packed32(COLORS[WHITE]);
    txt_debug_fontStyle.outline_color = color_from_packed32(COLORS[T_BLACK]);

    txt_title_fontStyle.color = color_from_packed32(COLORS[GREEN]);
    txt_title_fontStyle.outline_color = color_from_packed32(COLORS[DARK_GREEN]);

    txt_bright_fontStyle.color = color_from_packed32(COLORS[YELLOW]);
    txt_debug_fontStyle.outline_color = color_from_packed32(COLORS[T_BLACK]);

    txt_green_fontStyle.color = color_from_packed32(COLORS[GREEN]);
    txt_title_fontStyle.outline_color = color_from_packed32(COLORS[DARK_GREEN]);

    for (int i = 1; i < ID_COUNT; i++)
    {
        rdpq_text_register_font(i, font[i]);
        rdpq_font_style(
            font[i], 
            STYLE_DEFAULT, 
            &txt_game_fontStyle
        );

        rdpq_font_style(
            font[i], 
            STYLE_TITLE, 
            &txt_title_fontStyle
        );

        rdpq_font_style(
            font[i], 
            STYLE_BRIGHT, 
            &txt_bright_fontStyle
        );

        rdpq_font_style(
            font[i], 
            STYLE_GREEN, 
            &txt_green_fontStyle
        );
    }

    rdpq_font_style(
        font[ID_DEBUG], 
        STYLE_DEBUG, 
        &txt_debug_fontStyle
    );

    txt_debugParms = (rdpq_textparms_t){.style_id = STYLE_DEBUG};
    txt_titleParms = (rdpq_textparms_t){.style_id = STYLE_TITLE};
    txt_gameParms = (rdpq_textparms_t){.style_id = STYLE_BRIGHT};

}


void ui_init(void)
{
    ui_register_fonts();
}

void ui_fps(void)
{
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_text_printf(&txt_debugParms, ID_DEBUG, fpsPos.v[0], fpsPos.v[1], "FPS %.2f", display_get_fps());
}

void ui_main_menu(void)
{

}

void ui_textbox(void)
{

}

// Step 4/5: call this AFTER your game logic ends each frame
void ui_update(void)
{
    
}

// Step 5/5: render out the UI at the very end
void ui_draw(void)
{
    ui_fps();
    ui_update();
}

#ifdef __cplusplus
}
#endif

#endif // UI_H