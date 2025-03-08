#ifndef CRACK_H
#define CRACK_H

#include <stdio.h>
#include <stdlib.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

static const size_t MaxObjectTypesNum       = 10;
static const size_t ScreensNumber           = 5;
static const size_t MaxObjectsNumber        = 10;

struct crack_t;
struct screen_t;
struct object_t;
struct object_info_t;
struct sfml_obj_allocator_t;

typedef long int object_type_t;

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
};

typedef crack_state_t (*updater_t     )(crack_t *, object_t *);
typedef crack_state_t (*constructor_t )(crack_t *, object_t *, const object_info_t *);
typedef crack_state_t (*destructor_t  )(crack_t *, object_t *);

struct object_info_t {
    crack_state_t     (*on_mouse_click )(crack_t *, screen_t *, object_t *);
    crack_state_t     (*on_mouse_move  )(crack_t *, object_t *);
    crack_state_t     (*on_text_entered)(crack_t *, object_t *, sf::Event &);
    crack_state_t     (*handle_buttons )(crack_t *, object_t *);
    const char         *texture;
    sf::Vector2f        size;
    const void         *object_private_info;
};

struct object_t {
    object_type_t       type;
    sf::Texture         texture;
    crack_state_t     (*on_mouse_click )(crack_t *, screen_t *, object_t *);
    crack_state_t     (*on_mouse_move  )(crack_t *, object_t *);
    crack_state_t     (*on_text_entered)(crack_t *, object_t *, sf::Event &);
    crack_state_t     (*handle_buttons )(crack_t *, object_t *);
    bool                waits_for_text;
    void               *object_private;
};

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

struct crack_t {
    screen_t            screens[ScreensNumber];
    size_t              screens_num;
    updater_t           updater[MaxObjectTypesNum];
    constructor_t       constructor[MaxObjectTypesNum];
    destructor_t        destructor[MaxObjectTypesNum];
    size_t              objects_number;
    sf::Vector2f        mouse;
    sf::RenderWindow    win;
    sf::Music           music;
    sf::Sound           sound;//TODO
    void               *objects_storage;
    void *            (*get_free_obj)(void *, int);
};

crack_state_t   create_object      (crack_t                *ctx,
                                    updater_t               updater,
                                    constructor_t           constructor,
                                    destructor_t            destructor,
                                    object_type_t          *obj_type);

crack_state_t   object_ctor        (crack_t                *ctx,
                                    object_t               *obj,
                                    const object_info_t    *obj_info,
                                    object_type_t           type);

crack_state_t   run_screen         (crack_t                *ctx,
                                    screen_t               *screen);

#define _RETURN_IF_ERROR(...) {                 \
    crack_state_t _error_code = (__VA_ARGS__);  \
    if(_error_code != CRACK_SUCCESS) {          \
        return _error_code;                     \
    }                                           \
}                                               \

#endif
