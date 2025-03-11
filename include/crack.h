/*============================================================================*/

#ifndef CRACK_H
#define CRACK_H

/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

/*============================================================================*/

static const size_t         MaxObjectTypesNum   = 10;
static const size_t         MaxObjectsNumber    = 10;

/*============================================================================*/

struct crack_t;
struct screen_t;
struct object_t;
struct object_info_t;
struct sfml_obj_allocator_t;

/*============================================================================*/

enum crack_state_t {
    CRACK_SUCCESS                           = 0,
    CRACK_EXIT_SUCCESS                      = 1,
    CRACK_TYPE_IN_USE                       = 2,
    CRACK_MEMORY_ERROR                      = 3,
    CRACK_FONT_LOADING_ERROR                = 4,
    CRACK_PLAYER_LOST                       = 5,
    CRACK_GAME_WON                          = 6,
    CRACK_PLAY_AGAIN                        = 7,
    CRACK_GO_PATCH                          = 8,
    CRACK_NOT_SUPPORTED                     = 9,
    CRACK_READING_FILE_ERROR                = 10,
    CRACK_WRITING_FILE_ERROR                = 11,
    CRACK_NO_SUPPORTED_PROT                 = 12,
    CRACK_READING_SUPPORTED_ERROR           = 13,
};

/*============================================================================*/

enum object_types_t {
    OBJECT_EMPTY                            = 0,
    OBJECT_BUTTON                           = 1,
    OBJECT_TEXTBOX                          = 2,
    OBJECT_PLAYER                           = 3,
    OBJECT_ENEMIES                          = 4,
    OBJECT_BULLETS                          = 5,
    OBJECT_DECORATION                       = 6,
    OBJECT_PBAR                             = 7,

    /*
    constant OBJECT_MAX is used as max number of different types
    add constants in this enum only lower than it, or change it
    */
    OBJECT_MAX                              = 10,
};

/*============================================================================*/

struct object_info_t {
    crack_state_t     (*on_mouse_click )(crack_t *, screen_t *, object_t *);
    crack_state_t     (*on_mouse_move  )(crack_t *, object_t *);
    crack_state_t     (*on_text_entered)(crack_t *, object_t *, sf::Event &);
    crack_state_t     (*handle_buttons )(crack_t *, object_t *);
    const char         *texture;
    sf::Vector2f        size;
    const void         *object_private_info;
};

/*============================================================================*/

struct object_t {
    object_types_t      type;
    sf::Texture         texture;
    crack_state_t     (*on_mouse_click )(crack_t *, screen_t *, object_t *);
    crack_state_t     (*on_mouse_move  )(crack_t *, object_t *);
    crack_state_t     (*on_text_entered)(crack_t *, object_t *, sf::Event &);
    crack_state_t     (*handle_buttons )(crack_t *, object_t *);
    bool                waits_for_text;
    void               *object_private;
};

/*============================================================================*/

struct screen_t {
    object_t            objects[MaxObjectsNumber];
    size_t              objects_num;
    sf::Music           music;
    crack_state_t     (*updater )(crack_t *, screen_t *);
    crack_state_t     (*unloader)(crack_t *, screen_t *);
    sf::RectangleShape  box;

    sf::Texture         background;
    size_t              counter;
    sf::Clock           timer;
};

/*============================================================================*/

enum screens_index_t {
    SCREEN_MENU                             = 1,
    SCREEN_GAME                             = 2,
    SCREEN_VICTORY                          = 3,
    SCREEN_DEFEAT                           = 4,
    SCREEN_FAILED                           = 5,
    SCREEN_SUCCESS                          = 6,

    SCREEN_MAX                              = 7,
};

/*============================================================================*/

struct crack_t {
    screen_t            screens[SCREEN_MAX];
    sf::Vector2f        mouse;
    sf::RenderWindow    win;
    sf::Music           music;
    void               *objects_storage;
    void *            (*get_free_obj)(void *, object_types_t);
};

/*============================================================================*/

crack_state_t   object_ctor        (crack_t                *ctx,
                                    object_t               *obj,
                                    const object_info_t    *obj_info,
                                    object_types_t          type);

crack_state_t   run_screen         (crack_t                *ctx,
                                    screen_t               *screen);

crack_state_t   screen_ctor        (screen_t               *screen,
                                    const char             *music,
                                    const char             *texture,
                                    crack_state_t         (*updater )(crack_t *, screen_t *),
                                    crack_state_t         (*unloader)(crack_t *, screen_t *));

/*============================================================================*/

#define _RETURN_IF_ERROR(...) {                 \
    crack_state_t _error_code = (__VA_ARGS__);  \
    if(_error_code != CRACK_SUCCESS) {          \
        return _error_code;                     \
    }                                           \
}                                               \

/*============================================================================*/

#endif

/*============================================================================*/
