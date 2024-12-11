#include <unistd.h>
#include <time.h>
#include <libdragon.h>
#include <display.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include "station.h"
#include "enemycraft.h"
#include "types.h"
#include "gfx.h"
#include "gamestatus.h"
#include "scoreui.h"
#include "bonus.h"

int w,h, w2, h2;

const char* targnames[3] = {"Spacecraft", "Asteroid", "Rocket"};

typedef enum targtypes_s{
    TYPE_CRAFT,
    TYPE_ASTEROID,
    TYPE_ROCKET
} targtypes_t;

typedef struct targinfo_s{
    bool enabled;
    targtypes_t type;
    float hp;
    float distance;
} targinfo_t;
targinfo_t targinfo;

void userinterface_playerdraw(PlyNum player, float hppercent, int points, int rockets, float powerup, float shield){   
    int playercount = core_get_playercount();
    int playernum = (int)player + 1;
    int offset = 30 + player*150;
    if(playernum > 2) offset = w - 300 + (player - 2)*150;

    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(5);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    surface_t nobonus = sprite_get_pixels(sprites[spr_ui_bonus0]);
    rdpq_tex_upload(TILE0, &nobonus, NULL);
    
    if(!(rockets > 0)) rdpq_texture_rectangle(TILE0, offset, 60, offset + 32, 60 + 32, 0, 0);
    if(!(powerup > 0)) rdpq_texture_rectangle(TILE0, offset + 40, 60, offset + 32 + 40, 60 + 32, 0, 0);
    if(!(shield > 0)) rdpq_texture_rectangle(TILE0, offset + 80, 60, offset + 32 + 80, 60 + 32, 0, 0);

    if(rockets > 0) rdpq_sprite_blit(sprites[spr_ui_bonus1], offset, 60, NULL);
    if(powerup > 0) rdpq_sprite_blit(sprites[spr_ui_bonus2], offset + 40, 60, NULL);
    if(shield > 0)  rdpq_sprite_blit(sprites[spr_ui_bonus3], offset + 80, 60, NULL);

    rdpq_text_printf(NULL, FONT_TEXT, offset, 30, "%s %i - %.1f%%", playernum <= playercount? "PLAYER" : "BOT", playernum, hppercent);
    rdpq_text_printf(NULL, FONT_TEXT, offset, 50, "%06i PTS", points);
    rdpq_text_printf(NULL, FONT_TEXT, offset + 20, 90, "%i", rockets);
    rdpq_text_printf(NULL, FONT_TEXT, offset + 60, 90, "%.1f", powerup);
    rdpq_text_printf(NULL, FONT_TEXT, offset + 100, 90, "%.1f", shield);
    
}

void userinterface_draw(){
    T3DVec3 viewpos, worldpos;
    float xpos, ypos;
    int playercount = core_get_playercount();

    w = display_get_width();
    h = display_get_height();
    w2 = display_get_width() /  2;
    h2 = display_get_height() / 2;

    float fps = display_get_fps();
    heap_stats_t heap_stats;
    sys_get_heap_stats(&heap_stats);
    rdpq_text_printf(NULL, FONT_TEXT, 20, h - 40, "FPS: %.2f", fps);
    rdpq_text_printf(NULL, FONT_TEXT, 20, h - 20, "Mem: %d KiB", heap_stats.used/1024);

    if(gamestatus.state == GAMESTATE_PLAY){

        rdpq_set_mode_standard();
        rdpq_set_prim_color(uicolor);
        rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
        rdpq_mode_alphacompare(5);

        rdpq_sprite_blit(sprites[spr_ui_crosshair], (w - sprites[spr_ui_crosshair]->width) / 2, (h - sprites[spr_ui_crosshair]->height) / 2, NULL);
        worldpos = gfx_worldpos_from_polar(station.pitch, station.yaw, 1000.0f);
        t3d_viewport_calc_viewspace_pos(&viewport, &viewpos, &worldpos);
        xpos = viewpos.v[0]; ypos = viewpos.v[1];
        rdpq_sprite_blit(sprites[spr_ui_crosshair2], (xpos - sprites[spr_ui_crosshair2]->width / 2), ypos - (sprites[spr_ui_crosshair2]->height / 2), NULL);

        surface_t target = sprite_get_pixels(sprites[spr_ui_target]);
        targinfo.enabled = false;
        rdpq_tex_upload(TILE0, &target, NULL);
        rdpq_set_env_color(RGBA32(0xFF,0xFF,0xFF,0xAF));
        rdpq_mode_combiner(RDPQ_COMBINER1((PRIM,ENV,TEX0,ENV),(TEX0,0,ENV,0)));
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        float Xb = sprites[spr_ui_target]->width / 2;
        float Yb = sprites[spr_ui_target]->height / 2;

        rdpq_set_prim_color(RGBA32(255,0,255,255));
        for(int b = 0; b < MAX_BONUSES; b++)
            if(bonuses[b].enabled){
                worldpos = gfx_worldpos_from_polar(bonuses[b].polarpos.v[0], bonuses[b].polarpos.v[1], 1000.0f);
                t3d_viewport_calc_viewspace_pos(&viewport, &viewpos, &worldpos);
                xpos = viewpos.v[0]; ypos = viewpos.v[1];
                if(gfx_pos_within_viewport(xpos, ypos) && t3d_vec3_dot(&station.forward, &worldpos) > 0.0f){
                    rdpq_texture_rectangle(TILE0, xpos - Xb, ypos - Yb, xpos + Xb, ypos + Yb, 0, 0);
                }
                else {
                    xpos = fclampr(xpos, 100, w - 100);
                    ypos = fclampr(ypos, 100, h - 100);
                    rdpq_texture_rectangle(TILE0, xpos, ypos, xpos + 16, ypos + 16, 40, 4);
                }
            }

        for(int i = 0; i < 3; i++){
            if(crafts[i].enabled){
                rdpq_set_prim_color(gfx_get_playercolor(crafts[i].currentplayer));
                worldpos = gfx_worldpos_from_polar(crafts[i].pitch, crafts[i].yaw, 1000.0f);
                t3d_viewport_calc_viewspace_pos(&viewport, &viewpos, &worldpos);
                xpos = viewpos.v[0]; ypos = viewpos.v[1];
                if(gfx_pos_within_viewport(xpos, ypos) && t3d_vec3_dot(&station.forward, &worldpos) > 0.0f){
                    if(gfx_pos_within_rect(xpos, ypos, w2 - 30, h2 - 30, w2 + 30, h2 + 30)){
                        targinfo.enabled = true;
                        targinfo.hp = crafts[i].hp;
                        targinfo.type = TYPE_CRAFT;
                        targinfo.distance = crafts[i].distanceoff;
                    }
                    rdpq_texture_rectangle(TILE0, xpos - Xb, ypos - Yb, xpos + Xb, ypos + Yb, 0, 0);
                }
                else {
                    xpos = fclampr(xpos, 100, w - 100);
                    ypos = fclampr(ypos, 100, h - 100);
                    rdpq_texture_rectangle(TILE0, xpos, ypos, xpos + 16, ypos + 16, 40, 4);
                }
            }

            rdpq_set_prim_color(RGBA32(0x8F,0x8F,0x8F,0x8F));
            for(int c = 0; c < MAX_PROJECTILES; c++){
                if(crafts[i].arm.asteroids[c].enabled){
                    worldpos = gfx_worldpos_from_polar(crafts[i].arm.asteroids[c].polarpos.v[0], crafts[i].arm.asteroids[c].polarpos.v[1], 1000.0f);
                    t3d_viewport_calc_viewspace_pos(&viewport, &viewpos, &worldpos);
                    xpos = viewpos.v[0]; ypos = viewpos.v[1];
                    if(gfx_pos_within_viewport(xpos, ypos) && t3d_vec3_dot(&station.forward, &worldpos) > 0.0f){
                        if(gfx_pos_within_rect(xpos, ypos, w2 - 30, h2 - 30, w2 + 30, h2 + 30)){
                            targinfo.enabled = true;
                            targinfo.hp = crafts[i].arm.asteroids[c].hp;
                            targinfo.type = TYPE_ASTEROID;
                            targinfo.distance = crafts[i].arm.asteroids[c].polarpos.v[2];
                        }
                        rdpq_texture_rectangle(TILE0, xpos - Xb, ypos - Yb, xpos + Xb, ypos + Yb, 0, 0);
                    }
                    else {
                        xpos = fclampr(xpos, 100, w - 100);
                        ypos = fclampr(ypos, 100, h - 100);
                        rdpq_texture_rectangle(TILE0, xpos, ypos, xpos + 16, ypos + 16, 40, 4);
                    }
                }

                if(crafts[i].arm.rockets[c].enabled){
                    worldpos = gfx_worldpos_from_polar(crafts[i].arm.rockets[c].polarpos.v[0], crafts[i].arm.rockets[c].polarpos.v[1], 1000.0f);
                    t3d_viewport_calc_viewspace_pos(&viewport, &viewpos, &worldpos);
                    xpos = viewpos.v[0]; ypos = viewpos.v[1];
                    if(gfx_pos_within_viewport(xpos, ypos) && t3d_vec3_dot(&station.forward, &worldpos) > 0.0f){
                        if(gfx_pos_within_rect(xpos, ypos, w2 - 30, h2 - 30, w2 + 30, h2 + 30)){
                            targinfo.enabled = true;
                            targinfo.hp = crafts[i].arm.rockets[c].hp;
                            targinfo.type = TYPE_ROCKET;
                            targinfo.distance = crafts[i].arm.rockets[c].polarpos.v[2];
                        }
                        rdpq_texture_rectangle(TILE0, xpos - Xb, ypos - Yb, xpos + Xb, ypos + Yb, 0, 0);
                    }
                    else {
                        xpos = fclampr(xpos, 100, w - 100);
                        ypos = fclampr(ypos, 100, h - 100);
                        rdpq_texture_rectangle(TILE0, xpos, ypos, xpos + 16, ypos + 16, 40, 4);
                    }
                }
            }
        }

        if(targinfo.enabled){
            rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 600}, 
            FONT_TEXT, w2 - 300, h - 20, 
            "Target in sight. Type: %s, Distance: %.2f, Health: %.2f", 
            targnames[targinfo.type], targinfo.distance, targinfo.hp);
        }

        userinterface_playerdraw(station.currentplayer, 100 * station.hp / station.maxhp, gamestatus.playerscores[station.currentplayer], station.arm.rocketcount, station.arm.powerup, station.arm.shield);
        for(int i = 0; i < 3; i++)
            userinterface_playerdraw(crafts[i].currentplayer, 100 * crafts[i].hp / crafts[i].maxhp, gamestatus.playerscores[crafts[i].currentplayer], crafts[i].arm.rocketcount, crafts[i].arm.powerup, crafts[i].arm.shield);
        
    }

    if(gamestatus.state != GAMESTATE_PLAY){
        rdpq_set_mode_standard();
        rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_mode_dithering(dither);
        rdpq_set_prim_color(RGBA32(180,140,50,128));
        rdpq_mode_antialias(AA_STANDARD);
        if(gamestatus.state == GAMESTATE_TRANSITION || gamestatus.state == GAMESTATE_FINISHED)
            rdpq_set_prim_color(RGBA32(0,0,0, t3d_lerp(128,255, fclampr(1 - (gamestatus.statetime * 0.2), 0,1))));
        rdpq_texture_rectangle(TILE0,0,0,w, h, 0,0);
    }
    else    
        rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 600}, 
        FONT_TEXT, w2 - 300, h - 40, 
        "%02i:%02i", (int)gamestatus.statetime / 60, (int)gamestatus.statetime % 60);
    if(gamestatus.state == GAMESTATE_PAUSED){
        rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 600}, 
        FONT_HEADING, w2 - 300, h2, 
        "Paused. Press Start to continue...");
    }
    if(gamestatus.state == GAMESTATE_GETREADY){
        rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 600}, 
        FONT_HEADING, w2 - 300, h2 - 100, 
        "Player %i is now in Defense.\nPress Start to get ready!", station.currentplayer + 1);
        for(int i = 0; i < MAXPLAYERS; i++)
            rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 600}, 
            FONT_HEADING, w2 - 300, h2 + 40 * i + 40, 
            "%s %i score is %06i PTS", i < playercount? "Player" : "BOT", i + 1, gamestatus.playerscores[i]);
    }
    if(gamestatus.state == GAMESTATE_COUNTDOWN){
        rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 600}, 
        FONT_HEADING, w2 - 300, h2, 
        "Match will begin in %.1f...", gamestatus.statetime);
    }
    if(gamestatus.state == GAMESTATE_TRANSITION){
        rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 600}, 
        FONT_HEADING, w2 - 300, h2 - 100, 
        "Match has ended!\nMoving to the next galaxy in %.2f...", gamestatus.statetime);
        for(int i = 0; i < MAXPLAYERS; i++)
            rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 600}, 
            FONT_HEADING, w2 - 300, h2 + 40 * i + 40, 
            "%s %i score is %06i PTS", i < playercount? "Player" : "BOT", i + 1, gamestatus.playerscores[i]);
    }
    if(gamestatus.state == GAMESTATE_FINISHED){
        for(int i = 0; i < MAXPLAYERS; i++){
            rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 600}, 
            FONT_HEADING, w2 - 300, h2 + 40 * i + 40, 
            "%s %i score is %06i PTS", i < playercount? "Player" : "BOT", i + 1, gamestatus.playerscores[i]);
        }

        rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 600}, 
        FONT_HEADING, w2 - 300, h2 - 100, 
        "Game over!\nThe winner is %s %i", gamestatus.winner < playercount? "Player" : "BOT", gamestatus.winner + 1);
    }
    
}