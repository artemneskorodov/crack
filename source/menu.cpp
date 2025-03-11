/*============================================================================*/

#include "menu.h"
#include "common.h"
#include "patch.h"

/*============================================================================*/

enum menu_objects_indexes_t {
    MENU_OBJ_BUTTON_START                   = 0,
    MENU_OBJ_BUTTON_EXIT                    = 1,
    MENU_OBJ_INPUT_TEXT                     = 2,
};

/*============================================================================*/

static const char          *MenuBackground          = "./styles/img/menu_background.png";
static const char          *MenuMusicFile           = "./styles/music/menu_music.mp3";
static const sf::IntRect    MenuBckgRects[]   = {
    sf::IntRect(   0,    0, 3200, 2560),
    sf::IntRect(3200,    0, 3200, 2560),
    sf::IntRect(   0, 2560, 3200, 2560),
};
static const sf::Time       MenuBckgUpdateTime      = sf::milliseconds(500);

/*============================================================================*/

static crack_state_t handle_menu_button_start  (crack_t    *ctx,
                                                screen_t   *screen,
                                                object_t   *obj);

static crack_state_t menu_updater              (crack_t    *ctx,
                                                screen_t   *screen);

/*============================================================================*/

static const button_info_t MenuButtonStartPrivate = {
    .texture_size           = sf::Vector2i(690, 370),
    .texture_focused        = sf::Vector2i(0, 370),
    .texture_unfocused      = sf::Vector2i(0, 0),
    .rel_pos                = sf::Vector2f(50 + 15, 75)
};

static const object_info_t MenuButtonStartPublic = {
    .on_mouse_click         = handle_menu_button_start,
    .on_mouse_move          = button_move,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/menu_button_start.png",
    .size                   = sf::Vector2f(69, 37) * 4.f,
    .object_private_info    = (const void *)&MenuButtonStartPrivate,
};

/*============================================================================*/

static const button_info_t MenuButtonExitPrivate = {
    .texture_size           = sf::Vector2i(690, 370),
    .texture_focused        = sf::Vector2i(0, 370),
    .texture_unfocused      = sf::Vector2i(0, 0),
    .rel_pos                = sf::Vector2f(50 - 15, 75)
};

static const object_info_t MenuButtonExitPublic = {
    .on_mouse_click         = handle_button_exit,
    .on_mouse_move          = button_move,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/menu_button_exit.png",
    .size                   = sf::Vector2f(69, 37) * 4.f,
    .object_private_info    = (const void *)&MenuButtonExitPrivate,
};

/*============================================================================*/

static const textbox_info_t MenuInputTextPrivate = {
    .font_name              = "./styles/fonts/main_font.ttf",
    .text_sound             = "./styles/sounds/text_type.wav",
    .text_size              = 45,
    .capacity               = 56,
    .texture_size           = sf::Vector2i(3000, 370),
    .texture_focused        = sf::Vector2i(0, 370),
    .texture_unfocused      = sf::Vector2i(0, 0),
    .rel_pos                = sf::Vector2f(50, 50),
};

static const object_info_t MenuInputTextPublic = {
    .on_mouse_click         = textbox_click,
    .on_mouse_move          = textbox_move,
    .on_text_entered        = textbox_text,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/menu_input_box.png",
    .size                   = sf::Vector2f(300, 37) * 4.f,
    .object_private_info    = (const void *)&MenuInputTextPrivate,
};

/*============================================================================*/

crack_state_t menu_ctor(crack_t *ctx, screen_t *screen) {
    screen->objects_num = 3;
    _RETURN_IF_ERROR(screen_ctor   (screen,
                                    MenuMusicFile,
                                    MenuBackground,
                                    menu_updater,
                                    NULL))
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + MENU_OBJ_BUTTON_START,
                                    &MenuButtonStartPublic,
                                    OBJECT_BUTTON));
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + MENU_OBJ_BUTTON_EXIT,
                                    &MenuButtonExitPublic,
                                    OBJECT_BUTTON));
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + MENU_OBJ_INPUT_TEXT,
                                    &MenuInputTextPublic,
                                    OBJECT_TEXTBOX));
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t handle_menu_button_start(crack_t *ctx,
                                       screen_t *screen,
                                       object_t *obj) {
    /*------------------------------------------------------------------------*/
    /* Checking if button is pressed                                          */
    if(!is_button_pressed(ctx, obj)) {
        return CRACK_SUCCESS;
    }
    /*------------------------------------------------------------------------*/
    /* Stoppign menu music                                                    */
    screen->music.stop();
    /*------------------------------------------------------------------------*/
    /* Running other screens                                                  */
    while(true) {
        /*--------------------------------------------------------------------*/
        /* Running game                                                       */
        crack_state_t game_result =
            run_screen(ctx, ctx->screens + SCREEN_GAME);
        /*--------------------------------------------------------------------*/
        if(game_result == CRACK_GAME_WON) {
            crack_state_t victory_result =
                run_screen(ctx, ctx->screens + SCREEN_VICTORY);
            if(victory_result == CRACK_PLAY_AGAIN) {
                continue;
            }
            else if(victory_result == CRACK_GO_PATCH) {
                break;
            }
            else {
                return victory_result;
            }
        }
        else if(game_result == CRACK_PLAYER_LOST) {
            crack_state_t defeat_result =
                run_screen(ctx, ctx->screens + SCREEN_DEFEAT);
            if(defeat_result == CRACK_PLAY_AGAIN) {
                continue;
            }
            else {
                return defeat_result;
            }
        }
        else {
            return game_result;
        }
    }
    char *filename = get_textbox_buffer(&screen->objects[MENU_OBJ_INPUT_TEXT]);
    crack_state_t patch_result = patch(filename);
    if(patch_result == CRACK_NOT_SUPPORTED) {
        _RETURN_IF_ERROR(run_screen(ctx, ctx->screens + SCREEN_FAILED));
        return CRACK_SUCCESS;
    }
    else if(patch_result != CRACK_SUCCESS) {
        return patch_result;
    }
    _RETURN_IF_ERROR(run_screen(ctx, ctx->screens + SCREEN_SUCCESS));
    screen->music.play();

    return patch_result;
}

/*============================================================================*/

crack_state_t menu_updater(crack_t *ctx, screen_t *screen) {
    if(screen->timer.getElapsedTime() > MenuBckgUpdateTime) {
        screen->timer.restart();
        screen->counter = (screen->counter + 1) % (sizeof(MenuBckgRects) /
                                                   sizeof(MenuBckgRects[0]));
        screen->box.setTextureRect(MenuBckgRects[screen->counter]);
    }
    ctx->win.draw(screen->box);
    return CRACK_SUCCESS;
};

/*============================================================================*/
