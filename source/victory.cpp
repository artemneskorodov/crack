/*============================================================================*/

#include "victory.h"

/*============================================================================*/

enum victory_object_t {
    VICTORY_OBJ_BUTTON_EXIT                 = 0,
    VICTORY_OBJ_BUTTON_AGAIN                = 1,
    VICTORY_OBJ_BUTTON_PATCH                = 2,
};

/*============================================================================*/

static const char *VictoryBackground    = "./styles/img/victory_background.png";
static const char *VictoryMusicFile     = "./styles/music/game_music.mp3";

/*============================================================================*/

static crack_state_t handle_defeat_button_patch    (crack_t    *ctx,
                                                    screen_t   *screen,
                                                    object_t   *obj);

static crack_state_t victory_updater               (crack_t    *ctx,
                                                    screen_t   *screen);

/*============================================================================*/

static const button_info_t VictoryButtonPatchPrivate = {
    .texture_size           = sf::Vector2i(690, 370),
    .texture_focused        = sf::Vector2i(0, 370),
    .texture_unfocused      = sf::Vector2i(0, 0),
    .rel_pos                = sf::Vector2f(50 + 25, 75)
};

static const object_info_t VictoryButtonPatchPublic = {
    .on_mouse_click         = handle_defeat_button_patch,
    .on_mouse_move          = button_move,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/victory_button_patch.png",
    .size                   = sf::Vector2f(69, 37) * 4.f,
    .object_private_info    = (const void *)&VictoryButtonPatchPrivate,
};

/*============================================================================*/

static const button_info_t VictoryButtonExitPrivate = {
    .texture_size           = sf::Vector2i(690, 370),
    .texture_focused        = sf::Vector2i(0, 370),
    .texture_unfocused      = sf::Vector2i(0, 0),
    .rel_pos                = sf::Vector2f(50 - 25, 75)
};

static const object_info_t VictoryButtonExitPublic = {
    .on_mouse_click         = handle_button_exit,
    .on_mouse_move          = button_move,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/menu_button_exit.png",
    .size                   = sf::Vector2f(69, 37) * 4.f,
    .object_private_info    = (const void *)&VictoryButtonExitPrivate,
};

/*============================================================================*/

static const button_info_t VictoryButtonAgainPrivate = {
    .texture_size           = sf::Vector2i(690, 370),
    .texture_focused        = sf::Vector2i(0, 370),
    .texture_unfocused      = sf::Vector2i(0, 0),
    .rel_pos                = sf::Vector2f(50, 75)
};

static const object_info_t VictoryButtonAgainPublic = {
    .on_mouse_click         = handle_button_again,
    .on_mouse_move          = button_move,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/defeat_button_again.png",
    .size                   = sf::Vector2f(69, 37) * 4.f,
    .object_private_info    = (const void *)&VictoryButtonAgainPrivate,
};

/*============================================================================*/

crack_state_t victory_ctor(crack_t *ctx, screen_t *screen) {
    screen->objects_num = 3;
    _RETURN_IF_ERROR(screen_ctor   (screen,
                                    VictoryMusicFile,
                                    VictoryBackground,
                                    victory_updater,
                                    NULL));
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + VICTORY_OBJ_BUTTON_EXIT,
                                    &VictoryButtonExitPublic,
                                    OBJECT_BUTTON));
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + VICTORY_OBJ_BUTTON_AGAIN,
                                    &VictoryButtonAgainPublic,
                                    OBJECT_BUTTON));
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + VICTORY_OBJ_BUTTON_PATCH,
                                    &VictoryButtonPatchPublic,
                                    OBJECT_BUTTON));
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t handle_defeat_button_patch(crack_t   *ctx,
                                         screen_t  *screen,
                                         object_t  *obj) {
    if(is_button_pressed(ctx, obj)) {
        return CRACK_GO_PATCH;
    }
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t victory_updater(crack_t *ctx, screen_t *screen) {
    ctx->win.draw(screen->box);
    return CRACK_SUCCESS;
}

/*============================================================================*/
