#include "defeat.h"

enum defeat_object_t {
    DEFEAT_OBJ_BUTTON_AGAIN = 0,
    DEFEAT_OBJ_BUTTON_EXIT = 1,
};

static const char *DefeatMusicFile = "./styles/music/menu_music.mp3";//"./styles/music/defeat_music.mp3";
static const char *DefeatBackground = "./styles/img/defeat_background.png";

crack_state_t defeat_updater(crack_t *ctx, screen_t *screen);

static const button_info_t DefeatButtonAgainPrivate = {
    .texture_size           = sf::Vector2i(690, 370),
    .texture_focused        = sf::Vector2i(0, 370),
    .texture_unfocused      = sf::Vector2i(0, 0),
    .rel_pos                = sf::Vector2f(50 - 15, 75)
};

static const object_info_t DefeatButtonAgainPublic = {
    .on_mouse_click         = handle_button_again,
    .on_mouse_move          = default_button_move,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/defeat_button_again.png",
    .size                   = sf::Vector2f(69, 37) * 4.f,
    .object_private_info    = (const void *)&DefeatButtonAgainPrivate,
};

static const button_info_t DefeatButtonExitPrivate = {
    .texture_size           = sf::Vector2i(690, 370),
    .texture_focused        = sf::Vector2i(0, 370),
    .texture_unfocused      = sf::Vector2i(0, 0),
    .rel_pos                = sf::Vector2f(50 + 15, 75)
};

static const object_info_t DefeatButtonExitPublic = {
    .on_mouse_click         = handle_button_exit,
    .on_mouse_move          = default_button_move,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/menu_button_exit.png",
    .size                   = sf::Vector2f(69, 37) * 4.f,
    .object_private_info    = (const void *)&DefeatButtonExitPrivate,
};

crack_state_t create_defeat_objects(crack_t *ctx, object_type_t *object_types) {
    return CRACK_SUCCESS;
}

crack_state_t defeat_ctor(crack_t *ctx, object_type_t *objects_types, screen_t *screen) {
    screen->objects_num = 2;
    screen->updater = defeat_updater;
    screen->music.openFromFile(DefeatMusicFile);
    screen->unloader = NULL;
    screen->background.loadFromFile(DefeatBackground);
    screen->box.setSize(ScreenSize);
    screen->box.setPosition(sf::Vector2f(0, 0));
    screen->box.setTexture(&screen->background);
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + DEFEAT_OBJ_BUTTON_AGAIN,
                                    &DefeatButtonAgainPublic,
                                    objects_types[OBJECT_BUTTON]));
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + DEFEAT_OBJ_BUTTON_EXIT,
                                    &DefeatButtonExitPublic,
                                    objects_types[OBJECT_BUTTON]));
    return CRACK_SUCCESS;
}

crack_state_t defeat_updater(crack_t *ctx, screen_t *screen) {
    ctx->win.draw(screen->box);
    return CRACK_SUCCESS;
}
