#ifndef COMMON_H
#define COMMON_H

#include "objects.h"
#include "crack.h"
#include "game.h"

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

enum all_objects_t {
    OBJECT_BUTTON                           = 0,
    OBJECT_TEXTBOX                          = 1,
    OBJECT_PLAYER                           = 2,
    OBJECT_ENEMIES                          = 3,
    OBJECT_BULLETS                          = 4,
    OBJECT_DECORATION                       = 5,
    OBJECT_PBAR                             = 6,

    /*
    constant OBJECT_INVALID is used as max number of different types
    add constants in this enum only lower than it, or change it
    */
    OBJECT_INVALID                          = 10,
};

enum screens_index_t {
    SCREEN_MENU                             = 0,
    SCREEN_GAME                             = 1,
    SCREEN_VICTORY                          = 2,
    SCREEN_DEFEAT                           = 3,
    SCREEN_PATCH                            = 4,
};

static const size_t PlayerObjectsNum        = 1;
static const size_t EnemiesObjectsNum       = 1;
static const size_t BulletsObjectsNum       = 1;
static const size_t ButtonObjectNum         = 7;
static const size_t TextboxObjectsNum       = 1;
static const size_t DecorationObjectsNum    = 1;
static const size_t PBarObjectsNum          = 1;

struct objects_storage_t {
    player_t            player_objs[PlayerObjectsNum];
    size_t              used_players;
    enemies_t           enemies_objs[EnemiesObjectsNum];
    size_t              used_enemies;
    bullets_t           bullets_objs[BulletsObjectsNum];
    size_t              used_bullets;
    button_t            button_objs[ButtonObjectNum];
    size_t              used_buttons;
    textbox_t           textbox_objs[TextboxObjectsNum];
    size_t              used_textboxs;
    decoration_t        decoration_objs[DecorationObjectsNum];
    size_t              used_decorations;
    pbar_t              pbar_objs[PBarObjectsNum];
    size_t              used_pbars;
};

crack_state_t   crack_context_ctor     (crack_t            *ctx,
                                        object_type_t       *object_types);

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

void           *get_free_object        (void               *storage,
                                        int                 object);

crack_state_t handle_button_exit(crack_t *ctx, screen_t *screen, object_t *obj);
crack_state_t handle_button_again(crack_t *ctx, screen_t *screen, object_t *obj);

#endif
