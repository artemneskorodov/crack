#ifndef COMMON_H
#define COMMON_H

#include "objects.h"
#include "crack.h"
#include "game.h"
#include "menu.h"
#include "defeat.h"
#include "victory.h"

static const sf::VideoMode WindowMode = sf::VideoMode(1440, 900);
static const sf::Vector2f ScreenSize = sf::Vector2f((float)WindowMode.width, (float)WindowMode.height);
static const char *WindowTitle = "SevaSosal";
static const char *DefaultBackground = "./styles/img/default_background.png";

struct button_info_t {
    sf::Vector2i        texture_size;
    sf::Vector2i        texture_focused;
    sf::Vector2i        texture_unfocused;
    sf::Vector2f        rel_pos;
};

struct textbox_info_t {
    const char         *font_name;
    const char         *text_sound;
    unsigned int        text_size;
    size_t              capacity;
    sf::Vector2i        texture_size;
    sf::Vector2i        texture_focused;
    sf::Vector2i        texture_unfocused;
    sf::Vector2f        rel_pos;
};

struct decoration_info_t {
    sf::Vector2f        pos;
    sf::Time            update_time;
    size_t              texture_offsets_num;
    sf::Vector2i        texture_offsets[MaxDecorationFrames];
    sf::Vector2i        texture_size;
};

struct pbar_info_t {
    sf::Vector2f        pos;
    size_t              frames_num;
    sf::Vector2i        frame_size;
};

struct object_type_info_t {
    object_types_t type;
    crack_state_t (*updater)(crack_t *ctx, object_t *obj);
    crack_state_t (*constructor)(crack_t *ctx, object_t *obj, const object_info_t *obj_info);
};

crack_state_t button_constructor(crack_t *ctx, object_t *obj, const object_info_t *obj_info);
crack_state_t button_updater(crack_t *ctx, object_t *obj);
crack_state_t textbox_constructor(crack_t *ctx, object_t *obj, const object_info_t *obj_info);
crack_state_t textbox_updater(crack_t *ctx, object_t *obj);
crack_state_t decoration_constructor(crack_t *ctx, object_t *obj, const object_info_t *obj_info);
crack_state_t decoration_updater(crack_t *ctx, object_t *obj);
crack_state_t pbar_constructor(crack_t *ctx, object_t *obj, const object_info_t *obj_info);
crack_state_t pbar_updater(crack_t *ctx, object_t *obj);

static const object_type_info_t ObjectTypesInfo[] = {
    {/*Empty field*/},
    {.type = OBJECT_BUTTON,     .updater =     button_updater, .constructor =     button_constructor},
    {.type = OBJECT_TEXTBOX,    .updater =    textbox_updater, .constructor =    textbox_constructor},
    {.type = OBJECT_PLAYER,     .updater =     player_updater, .constructor =     player_constructor},
    {.type = OBJECT_ENEMIES,    .updater =    enemies_updater, .constructor =    enemies_constructor},
    {.type = OBJECT_BULLETS,    .updater =    bullets_updater, .constructor =    bullets_constructor},
    {.type = OBJECT_DECORATION, .updater = decoration_updater, .constructor = decoration_constructor},
    {.type = OBJECT_PBAR,       .updater =       pbar_updater, .constructor =       pbar_constructor},
};

crack_state_t   crack_context_ctor     (crack_t            *ctx);

char           *get_textbox_buffer     (object_t           *obj);

bool            is_button_pressed      (crack_t            *ctx,
                                        object_t           *obj);

crack_state_t   default_button_move    (crack_t            *ctx,
                                        object_t           *obj);

crack_state_t   default_textbox_click  (crack_t            *ctx,
                                        screen_t           *screen,
                                        object_t           *obj);

crack_state_t   default_textbox_move   (crack_t            *ctx,
                                        object_t           *obj);

crack_state_t   default_textbox_text   (crack_t            *ctx,
                                        object_t           *obj,
                                        sf::Event          &event);

crack_state_t handle_button_exit(crack_t *ctx, screen_t *screen, object_t *obj);
crack_state_t handle_button_again(crack_t *ctx, screen_t *screen, object_t *obj);

#endif
