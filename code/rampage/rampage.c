#include <libdragon.h>
#include "../../core.h"
#include "../../minigame.h"
#include <stdio.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#include "./rampage.h"
#include "./assets.h"

surface_t *depthBuffer;
T3DViewport viewport;

T3DVec3 camPos = {{0.0f, SCALE_FIXED_POINT(12.0f), SCALE_FIXED_POINT(4.0f)}};
T3DVec3 camTarget = {{0.0f, 0.0f, 0.0f}};

struct Rampage gRampage;

const MinigameDef minigame_def = {
    .gamename = "Rampage",
    .developername = "Ultrarare",
    .description = "Destroy the most buildings to win",
    .instructions = "Press A to win."
};

void minigame_init() {
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    t3d_init((T3DInitParams){});

    depthBuffer = display_get_zbuf();
    viewport = t3d_viewport_create();

    rampage_init(&gRampage);
}

void minigame_fixedloop(float deltatime) {
    for (int i = 0; i < PLAYER_COUNT; i += 1) {
        rampage_player_update(&gRampage.players[i], deltatime);
    }
}

void minigame_loop(float deltatime) {   
    uint8_t colorAmbient[4] = {0xAA, 0xAA, 0xAA, 0xFF};
    uint8_t colorDir[4]     = {0xFF, 0xAA, 0xAA, 0xFF};

    T3DVec3 lightDirVec = (T3DVec3){{1.0f, 1.0f, 1.0f}};
    t3d_vec3_norm(&lightDirVec);

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(90.0f), SCALE_FIXED_POINT(6.0f), SCALE_FIXED_POINT(16.0f));
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    rdpq_attach(display_get(), depthBuffer);
    t3d_frame_start();
    t3d_viewport_attach(&viewport);
    t3d_screen_clear_color(RGBA32(224, 180, 96, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    for (int i = 0; i < PLAYER_COUNT; i += 1) {
        rampage_player_render(&gRampage.players[i]);
    }

    for (int y = 0; y < BUILDING_COUNT_Y; y += 1) {
        for (int x = 0; x < BUILDING_COUNT_X; x += 1) {
            rampage_building_render(&gRampage.buildings[y][x]);
        }
    }

    rdpq_detach_show();
}

void minigame_cleanup() {
    rampage_destroy(&gRampage);
    t3d_destroy();
}

static T3DVec3 gStartingPositions[] = {
    {{SCALE_FIXED_POINT(-8.0f), 0.0f, SCALE_FIXED_POINT(-8.0f)}},
    {{SCALE_FIXED_POINT(8.0f), 0.0f, SCALE_FIXED_POINT(-8.0f)}},
    {{SCALE_FIXED_POINT(-8.0f), 0.0f, SCALE_FIXED_POINT(8.0f)}},
    {{SCALE_FIXED_POINT(8.0f), 0.0f, SCALE_FIXED_POINT(8.0f)}},
};

enum PlayerType rampage_player_type(int index) {
    if (index < core_get_playercount()) {
        return (enum PlayerType)index;
    } else {
        return (enum PlayerType)(PLAYER_TYPE_EASY + core_get_aidifficulty());
    }
}

#define BUILDING_SPACING    3.0f

void rampage_init(struct Rampage* rampage) {
    rampage_assets_init();

    for (int i = 0; i < PLAYER_COUNT; i += 1) {
        rampage_player_init(&rampage->players[i], &gStartingPositions[i], rampage_player_type(i));
    }

    for (int y = 0; y < BUILDING_COUNT_Y; y += 1) {
        for (int x = 0; x < BUILDING_COUNT_X; x += 1) {
            T3DVec3 position = {{
                SCALE_FIXED_POINT((x - (BUILDING_COUNT_X - 1) * 0.5f) * BUILDING_SPACING),
                0.0f,
                SCALE_FIXED_POINT((y - (BUILDING_COUNT_Y - 1) * 0.5f) * BUILDING_SPACING),
            }};
            rampage_building_init(&rampage->buildings[y][x], &position);
        }
    }
}

void rampage_destroy(struct Rampage* rampage) {
    for (int i = 0; i < PLAYER_COUNT; i += 1) {
        rampage_player_destroy(&rampage->players[i]);
    }

    rampage_assets_destroy();
}