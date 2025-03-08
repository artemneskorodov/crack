#include "menu.h"
#include "common.h"

enum menu_objects_indexes_t {
    MENU_OBJ_BUTTON_START = 0,
    MENU_OBJ_BUTTON_EXIT = 1,
    MENU_OBJ_INPUT_TEXT = 2,
};

/*
    PUBLIC
    .on_mouse_click         =
    .on_mouse_focus         =
    .on_mouse_unfocus       =
    .on_text_entered        =
    .on_mouse_missclick     =
    .handle_buttons         =
    .texture                =
    .size                   =
    .object_private_info    =
*/

static const char *MenuBackground = "./styles/img/menu_background.png";
static const char *MenuMusicFile = "./styles/music/menu_music.mp3";
static const sf::IntRect MenuBackgroundRects[] = {
    sf::IntRect(   0,    0, 3200, 2560),
    sf::IntRect(3200,    0, 3200, 2560),
    sf::IntRect(   0, 2560, 3200, 2560),
};
static const sf::Time MenuBackgroundUpdateTime = sf::milliseconds(500);

static crack_state_t handle_menu_button_start(crack_t *ctx, screen_t *screen, object_t *obj);
static crack_state_t menu_updater(crack_t *ctx, screen_t *screen);

/*============================================================================*/

static const button_info_t MenuButtonStartPrivate = {
    .texture_size           = sf::Vector2i(690, 370),
    .texture_focused        = sf::Vector2i(0, 370),
    .texture_unfocused      = sf::Vector2i(0, 0),
    .rel_pos                = sf::Vector2f(50 + 15, 75)
};

static const object_info_t MenuButtonStartPublic = {
    .on_mouse_click         = handle_menu_button_start,
    .on_mouse_move          = default_button_move,
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
    .on_mouse_move          = default_button_move,
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
    .text_size              = 60,
    .capacity               = 40,
    .texture_size           = sf::Vector2i(3000, 370),
    .texture_focused        = sf::Vector2i(0, 370),
    .texture_unfocused      = sf::Vector2i(0, 0),
    .rel_pos                = sf::Vector2f(50, 50),
};

static const object_info_t MenuInputTextPublic = {
    .on_mouse_click         = default_textbox_click,
    .on_mouse_move          = default_textbox_move,
    .on_text_entered        = default_textbox_text,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/menu_input_box.png",
    .size                   = sf::Vector2f(300, 37) * 4.f,
    .object_private_info    = (const void *)&MenuInputTextPrivate,
};

/*============================================================================*/

crack_state_t create_menu_objects(crack_t */*ctx*/, object_type_t */*object_types*/) {
    return CRACK_SUCCESS;
}

crack_state_t menu_ctor(crack_t *ctx, object_type_t *object_types, screen_t *screen) {
    screen->objects_num = 3;
    screen->updater = menu_updater;
    screen->music.openFromFile(MenuMusicFile);
    screen->unloader = NULL;
    screen->background.loadFromFile(MenuBackground);
    screen->box.setSize(ScreenSize);
    screen->box.setPosition(sf::Vector2f(0, 0));
    screen->box.setTexture(&screen->background);

    _RETURN_IF_ERROR(object_ctor(ctx,
                                 screen->objects + MENU_OBJ_BUTTON_START,
                                 &MenuButtonStartPublic,
                                 object_types[OBJECT_BUTTON]));
    _RETURN_IF_ERROR(object_ctor(ctx,
                                 screen->objects + MENU_OBJ_BUTTON_EXIT,
                                 &MenuButtonExitPublic,
                                 object_types[OBJECT_BUTTON]));
    _RETURN_IF_ERROR(object_ctor(ctx,
                                 screen->objects + MENU_OBJ_INPUT_TEXT,
                                 &MenuInputTextPublic,
                                 object_types[OBJECT_TEXTBOX]));
    return CRACK_SUCCESS;
}

crack_state_t handle_menu_button_start(crack_t *ctx, screen_t *screen, object_t *obj) {
    if(!is_button_pressed(ctx, obj)) {
        return CRACK_SUCCESS;
    }
    screen->music.stop();
    while(true) {
        crack_state_t game_result = run_screen(ctx, ctx->screens + SCREEN_GAME);
        if(game_result == CRACK_GAME_WON) {
            crack_state_t victory_result = run_screen(ctx, ctx->screens + SCREEN_VICTORY);
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
            crack_state_t defeat_result = run_screen(ctx, ctx->screens + SCREEN_DEFEAT);
            if(defeat_result == CRACK_PLAY_AGAIN) {
                continue;
            }
            else {
                return defeat_result;
            }
        }
    }
    crack_state_t patch_result = run_screen(ctx, ctx->screens + SCREEN_PATCH);
    screen->music.play();

    return patch_result;
}

crack_state_t menu_updater(crack_t *ctx, screen_t *screen) {
    if(screen->timer.getElapsedTime() > MenuBackgroundUpdateTime) {
        screen->timer.restart();
        screen->counter = (screen->counter + 1) % (sizeof(MenuBackgroundRects) / sizeof(MenuBackgroundRects[0]));
        screen->box.setTextureRect(MenuBackgroundRects[screen->counter]);
    }
    ctx->win.draw(screen->box);
    return CRACK_SUCCESS;
};
