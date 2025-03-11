/*============================================================================*/

#include "patch.h"
#include "utils.h"

/*============================================================================*/

static const char      *SupportedPrototype      = "./data/supported.com";
static const size_t     PasswordOffset          = 0x244;
static const char      *PatchSuccessBckg        = "./styles/img/patch_success_background.png";
static const char      *PatchSuccessMusicFile   = "./styles/music/menu_music.mp3";
static const char      *PatchFailedBckg         = "./styles/img/patch_failed_background.png";

/*============================================================================*/

static crack_state_t check_file_supported  (char       *buffer,
                                            size_t      size);

static crack_state_t change_file           (FILE       *file);

static crack_state_t patch_updater         (crack_t    *ctx,
                                            screen_t   *screen);

/*============================================================================*/

static const button_info_t PatchSuccessExitPrivate = {
    .texture_size           = sf::Vector2i(690, 370),
    .texture_focused        = sf::Vector2i(0, 370),
    .texture_unfocused      = sf::Vector2i(0, 0),
    .rel_pos                = sf::Vector2f(90, 50)
};

static const object_info_t PatchSuccessExitPublic = {
    .on_mouse_click         = handle_button_exit,
    .on_mouse_move          = button_move,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/menu_button_exit.png",
    .size                   = sf::Vector2f(69, 37) * 4.f,
    .object_private_info    = (const void *)&PatchSuccessExitPrivate,
};

/*============================================================================*/

static const button_info_t PatchFailedExitPrivate = {
    .texture_size           = sf::Vector2i(690, 370),
    .texture_focused        = sf::Vector2i(0, 370),
    .texture_unfocused      = sf::Vector2i(0, 0),
    .rel_pos                = sf::Vector2f(50, 75)
};

static const object_info_t PatchFailedExitPublic = {
    .on_mouse_click         = handle_button_exit,
    .on_mouse_move          = button_move,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/menu_button_exit.png",
    .size                   = sf::Vector2f(69, 37) * 4.f,
    .object_private_info    = (const void *)&PatchFailedExitPrivate,
};

/*============================================================================*/

crack_state_t patch(const char *filename) {
    FILE *file = fopen(filename, "r+b");
    if(file == NULL) {
        printf("%s", strerror(errno));
        return CRACK_NOT_SUPPORTED;
    }
    size_t size = file_size(file);
    char *buffer = (char *)calloc(2 * size, sizeof(char));
    if(buffer == NULL) {
        fclose(file);
        return CRACK_MEMORY_ERROR;
    }
    if(fread(buffer, sizeof(char), size, file) != size) {
        fclose(file);
        free(buffer);
        return CRACK_READING_FILE_ERROR;
    }

    crack_state_t error_code = check_file_supported(buffer, size);
    if(error_code != CRACK_SUCCESS) {
        fclose(file);
        free(buffer);
        return error_code;
    }
    free(buffer);

    error_code = change_file(file);
    if(error_code != CRACK_SUCCESS) {
        fclose(file);
        return error_code;
    }

    fclose(file);
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t check_file_supported(char *buffer, size_t size) {
    FILE *supported = fopen(SupportedPrototype, "rb");
    if(supported == NULL) {
        printf("%s", strerror(errno));
        return CRACK_NO_SUPPORTED_PROT;
    }
    size_t prototype_size = file_size(supported);
    if(prototype_size != size) {
        fprintf(stderr, "%lu %lu\n", prototype_size, size);
        fclose(supported);
        return CRACK_NOT_SUPPORTED;
    }
    if(fread(buffer + size, sizeof(char), size, supported) != size) {
        fclose(supported);
        return CRACK_READING_SUPPORTED_ERROR;
    }
    fclose(supported);
    for(size_t i = 0; i < size; i++) {
        if(buffer[i] != buffer[i + size]) {
            return CRACK_NOT_SUPPORTED;
        }
    }
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t change_file(FILE *file) {
    fseek(file, PasswordOffset, SEEK_SET);
    if(fwrite(NewPassword,
              sizeof(char),
              NewPasswordLen,
              file) != NewPasswordLen) {
        return CRACK_WRITING_FILE_ERROR;
    }
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t patch_failed_ctor(crack_t *ctx, screen_t *screen) {
    screen->objects_num = 1;
    _RETURN_IF_ERROR(screen_ctor   (screen,
                                    PatchSuccessMusicFile,
                                    PatchFailedBckg,
                                    patch_updater,
                                    NULL));
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + 0,
                                    &PatchFailedExitPublic,
                                    OBJECT_BUTTON));
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t patch_success_ctor(crack_t *ctx, screen_t *screen) {
    screen->objects_num = 1;
    _RETURN_IF_ERROR(screen_ctor   (screen,
                                    PatchSuccessMusicFile,
                                    PatchSuccessBckg,
                                    patch_updater,
                                    NULL));
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + 0,
                                    &PatchSuccessExitPublic,
                                    OBJECT_BUTTON));
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t patch_updater(crack_t *ctx, screen_t *screen) {
    ctx->win.draw(screen->box);
    return CRACK_SUCCESS;
}

/*============================================================================*/
