#include "common.h"
#include "utils.h"
#include "menu.h"
#include "game.h"
#include "defeat.h"
#include "victory.h"

static const size_t TextOffset = 25;

static crack_state_t updater_button(crack_t *ctx, object_t *obj);
static crack_state_t constructor_button(crack_t *ctx, object_t *obj, const object_info_t *obj_info);
static crack_state_t destructor_button(crack_t *ctx, object_t *obj);
static crack_state_t updater_textbox(crack_t *ctx, object_t *obj);
static crack_state_t constructor_textbox(crack_t *ctx, object_t *obj, const object_info_t *obj_info);
static crack_state_t destructor_textbox(crack_t *ctx, object_t *obj);
static crack_state_t constructor_decoration(crack_t *ctx, object_t *obj, const object_info_t *obj_info);
static crack_state_t updater_decoration(crack_t *ctx, object_t *obj);
static crack_state_t destructor_decoration(crack_t *ctx, object_t *obj);
static crack_state_t constructor_pbar(crack_t *ctx, object_t *obj, const object_info_t *obj_info);
static crack_state_t updater_pbar(crack_t *ctx, object_t *obj);
static crack_state_t destructor_pbar(crack_t *ctx, object_t *obj);
static crack_state_t create_common_objects(crack_t *ctx, object_type_t *object_types);

crack_state_t create_common_objects(crack_t *ctx, object_type_t *object_types) {
    _RETURN_IF_ERROR(create_object(ctx,
                                   updater_button,
                                   constructor_button,
                                   destructor_button,
                                   object_types + OBJECT_BUTTON));

    _RETURN_IF_ERROR(create_object(ctx,
                                   updater_textbox,
                                   constructor_textbox,
                                   destructor_textbox,
                                   object_types + OBJECT_TEXTBOX));
    _RETURN_IF_ERROR(create_object(ctx,
                                   updater_decoration,
                                   constructor_decoration,
                                   destructor_decoration,
                                   object_types + OBJECT_DECORATION));
    _RETURN_IF_ERROR(create_object(ctx,
                                   updater_pbar,
                                   constructor_pbar,
                                   destructor_pbar,
                                   object_types + OBJECT_PBAR));
    return CRACK_SUCCESS;
}

crack_state_t updater_button(crack_t *ctx, object_t *obj) {
    button_t *button = (button_t *)obj->object_private;
    ctx->win.draw(button->box);
    return CRACK_SUCCESS;
}

crack_state_t constructor_button(crack_t *ctx, object_t *obj, const object_info_t *obj_info) {
    obj->object_private = ctx->get_free_obj(ctx->objects_storage, (int)OBJECT_BUTTON);
    button_t *button = (button_t *)obj->object_private;
    const button_info_t *info = (const button_info_t *)obj_info->object_private_info;

    sf::Vector2f pos = real_pos(info->rel_pos, ScreenSize, obj_info->size);
    button->box.setPosition(pos);
    button->box.setSize(obj_info->size);
    button->box.setTexture(&obj->texture);

    button->focused     = sf::IntRect(info->texture_focused.x,
                                      info->texture_focused.y,
                                      info->texture_size.x,
                                      info->texture_size.y);
    button->unfocused   = sf::IntRect(info->texture_unfocused.x,
                                      info->texture_unfocused.y,
                                      info->texture_size.x,
                                      info->texture_size.y);
    button->box.setTextureRect(button->unfocused);
    return CRACK_SUCCESS;
}

crack_state_t destructor_button(crack_t */*ctx*/, object_t */*obj*/) {
    return CRACK_SUCCESS;
}

crack_state_t updater_textbox(crack_t *ctx, object_t *obj) {
    textbox_t *textbox = (textbox_t *)obj->object_private;

    textbox->text.setString(textbox->buffer);

    ctx->win.draw(textbox->box);
    ctx->win.draw(textbox->text);
    return CRACK_SUCCESS;
}

crack_state_t constructor_textbox(crack_t *ctx, object_t *obj, const object_info_t *obj_info) {
    obj->object_private = ctx->get_free_obj(ctx->objects_storage, (int)OBJECT_TEXTBOX);
    textbox_t *textbox = (textbox_t *)obj->object_private;
    const textbox_info_t *info = (const textbox_info_t *)obj_info->object_private_info;

    textbox->buffer = (char *)calloc(info->capacity + 1, sizeof(char));
    if(textbox->buffer == NULL) {
        fprintf(stderr, "Error while allocating textbox buffer\n");
        return CRACK_MEMORY_ERROR;
    }
    textbox->capacity = info->capacity;

    sf::Vector2f box_pos  = real_pos(info->rel_pos, ScreenSize, obj_info->size);
    sf::Vector2f text_pos = box_pos +
                            sf::Vector2f(TextOffset, 0.5f * (obj_info->size.y - info->text_size));

    textbox->box.setPosition(box_pos);
    textbox->box.setSize(obj_info->size);
    textbox->box.setTexture(&obj->texture);

    if(!textbox->font.loadFromFile(info->font_name)) {
        fprintf(stderr, "Errro while loading '%s'\n", info->font_name);
        return CRACK_FONT_LOADING_ERROR;
    }
    textbox->text.setFont(textbox->font);
    textbox->text.setCharacterSize(info->text_size);
    textbox->text.setPosition(text_pos);
    textbox->focused    = sf::IntRect(info->texture_focused.x,
                                      info->texture_focused.y,
                                      info->texture_size.x,
                                      info->texture_size.y);
    textbox->unfocused  = sf::IntRect(info->texture_unfocused.x,
                                      info->texture_unfocused.y,
                                      info->texture_size.x,
                                      info->texture_size.y);
    textbox->box.setTextureRect(textbox->unfocused);
    textbox->text_sound_buffer.loadFromFile(info->text_sound);
    textbox->text_sound.setBuffer(textbox->text_sound_buffer);
    return CRACK_SUCCESS;
}

crack_state_t destructor_textbox(crack_t */*ctx*/, object_t *obj) {
    textbox_t *textbox = (textbox_t *)obj->object_private;
    free(textbox->buffer);
    return CRACK_SUCCESS;
}

crack_state_t constructor_decoration(crack_t *ctx, object_t *obj, const object_info_t *obj_info) {
    obj->object_private = ctx->get_free_obj(ctx->objects_storage, (int)OBJECT_DECORATION);
    decoration_t *decoration = (decoration_t *)obj->object_private;
    const decoration_info_t *info = (const decoration_info_t *)obj_info->object_private_info;

    decoration->offsets_num = info->texture_offsets_num;
    decoration->update_time = info->update_time;

    for(size_t i = 0; i < decoration->offsets_num; i++) {
        decoration->frames[i] = sf::IntRect(info->texture_offsets[i].x,
                                            info->texture_offsets[i].y,
                                            info->texture_size.x,
                                            info->texture_size.y);
    }
    decoration->box.setSize(obj_info->size);
    decoration->box.setPosition(real_pos(info->pos, ScreenSize, obj_info->size));
    decoration->box.setTexture(&obj->texture);
    decoration->box.setTextureRect(decoration->frames[0]);
    return CRACK_SUCCESS;
}

crack_state_t updater_decoration(crack_t *ctx, object_t *obj) {
    decoration_t *decoration = (decoration_t *)obj->object_private;
    if(decoration->timer.getElapsedTime() > decoration->update_time) {
        decoration->timer.restart();
        decoration->current_frame++;
        decoration->current_frame %= decoration->offsets_num;
        decoration->box.setTextureRect(decoration->frames[decoration->current_frame]);
    }
    ctx->win.draw(decoration->box);
    return CRACK_SUCCESS;
}

crack_state_t destructor_decoration(crack_t */*ctx*/, object_t */*obj*/) {
    return CRACK_SUCCESS;
}

crack_state_t constructor_pbar(crack_t *ctx, object_t *obj, const object_info_t *obj_info) {
    obj->object_private = ctx->get_free_obj(ctx->objects_storage, (int)OBJECT_PBAR);
    pbar_t *pbar = (pbar_t *)obj->object_private;
    const pbar_info_t *info = (const pbar_info_t *)obj_info->object_private_info;

    pbar->frames_num = info->frames_num;
    int frame_h = info->frame_size.y;
    for(size_t i = 0; i < info->frames_num; i++) {
        pbar->frames[i] = sf::IntRect(0, frame_h * (int)i, info->frame_size.x, info->frame_size.y);
    }

    pbar->box.setSize(obj_info->size);
    pbar->box.setPosition(real_pos(info->pos, ScreenSize, obj_info->size));
    pbar->box.setTexture(&obj->texture);
    pbar->box.setTextureRect(pbar->frames[0]);
    pbar->current_frame = 0;
    return CRACK_SUCCESS;
}

crack_state_t updater_pbar(crack_t *ctx, object_t *obj) {
    pbar_t *pbar = (pbar_t *)obj->object_private;
    ctx->win.draw(pbar->box);
    return CRACK_SUCCESS;
}

crack_state_t destructor_pbar(crack_t */*ctx*/, object_t */*obj*/) {
    return CRACK_SUCCESS;
}

char *get_textbox_buffer(object_t *obj) {
    textbox_t *textbox = (textbox_t *)obj->object_private;
    return textbox->buffer;
}

crack_state_t crack_context_ctor(crack_t *ctx, object_type_t *object_types) {
    ctx->win.create(WindowMode, WindowTitle);

    ctx->screens_num = 1;

    _RETURN_IF_ERROR(create_common_objects(ctx, object_types));
    _RETURN_IF_ERROR(create_game_objects(ctx, object_types));

    _RETURN_IF_ERROR(menu_ctor(ctx, object_types, ctx->screens + SCREEN_MENU));
    _RETURN_IF_ERROR(game_ctor(ctx, object_types, ctx->screens + SCREEN_GAME));
    _RETURN_IF_ERROR(defeat_ctor(ctx, object_types, ctx->screens + SCREEN_DEFEAT));
    _RETURN_IF_ERROR(victory_ctor(ctx, object_types, ctx->screens + SCREEN_VICTORY));

    return CRACK_SUCCESS;
}

crack_state_t default_button_move(crack_t *ctx, object_t *obj) {
    button_t *button = (button_t *)obj->object_private;
    if(is_object_focused(ctx->mouse, button->box)) {
        button->box.setTextureRect(button->focused);
    }
    else {
        button->box.setTextureRect(button->unfocused);
    }
    return CRACK_SUCCESS;
}

crack_state_t default_textbox_click(crack_t *ctx, screen_t */*screen*/, object_t *obj) {
    textbox_t *textbox = (textbox_t *)obj->object_private;
    if(is_object_focused(ctx->mouse, textbox->box)) {
        obj->waits_for_text = true;
    }
    else {
        obj->waits_for_text = false;
    }
    return CRACK_SUCCESS;
}

crack_state_t default_textbox_move(crack_t *ctx, object_t *obj) {
    textbox_t *textbox = (textbox_t *)obj->object_private;
    if(is_object_focused(ctx->mouse, textbox->box)) {
        textbox->box.setTextureRect(textbox->focused);
    }
    else {
        textbox->box.setTextureRect(textbox->unfocused);
    }
    return CRACK_SUCCESS;
}

crack_state_t default_textbox_text(crack_t */*ctx*/, object_t *obj, sf::Event &event) {
    textbox_t *textbox = (textbox_t *)obj->object_private;
    char key = (char)event.text.unicode;
    if(key == '\b') {
        if(textbox->position == 0) {
            return CRACK_SUCCESS;
        }
        textbox->buffer[--textbox->position] = '\0';
    }
    else {
        if(textbox->position == textbox->capacity) {
            return CRACK_SUCCESS;
        }
        textbox->buffer[textbox->position++] = key;
    }
    textbox->text_sound.play();
    return CRACK_SUCCESS;
}

bool is_button_pressed(crack_t *ctx, object_t *obj) {
    button_t *button = (button_t *)obj->object_private;
    return is_object_focused(ctx->mouse, button->box);
}

void *get_free_object(void *storage_ptr, int object) {
    objects_storage_t *storage = (objects_storage_t *)storage_ptr;
    all_objects_t obj = (all_objects_t)object;

    switch(obj) {
        case OBJECT_BUTTON: {
            if(storage->used_buttons == ButtonObjectNum) {
                fprintf(stderr, "All buttons are used\n");
                return NULL;
            }
            return &storage->button_objs[storage->used_buttons++];
        }
        case OBJECT_TEXTBOX: {
            if(storage->used_textboxs == TextboxObjectsNum) {
                fprintf(stderr, "All textboxs are used\n");
                return NULL;
            }
            return &storage->textbox_objs[storage->used_textboxs++];
        }
        case OBJECT_PLAYER: {
            if(storage->used_players == PlayerObjectsNum) {
                fprintf(stderr, "All players are used\n");
                return NULL;
            }
            return &storage->player_objs[storage->used_players++];
        }
        case OBJECT_ENEMIES: {
            if(storage->used_enemies == EnemiesObjectsNum) {
                fprintf(stderr, "All enemies are used\n");
                return NULL;
            }
            return &storage->enemies_objs[storage->used_enemies++];
        }
        case OBJECT_BULLETS: {
            if(storage->used_bullets == BulletsObjectsNum) {
                fprintf(stderr, "All bullets are used\n");
                return NULL;
            }
            return &storage->bullets_objs[storage->used_bullets++];
        }
        case OBJECT_DECORATION: {
            if(storage->used_decorations == DecorationObjectsNum) {
                fprintf(stderr, "All decorations are used\n");
                return NULL;
            }
            return &storage->decoration_objs[storage->used_decorations++];
        }
        case OBJECT_PBAR: {
            if(storage->used_pbars == PBarObjectsNum) {
                fprintf(stderr, "All progress bars are used\n");
                return NULL;
            }
            return &storage->pbar_objs[storage->used_pbars++];
        }
        case OBJECT_INVALID:
        default:
            fprintf(stderr, "Unknown object requested");
            return NULL;
    }
}

crack_state_t handle_button_exit(crack_t *ctx, screen_t *screen, object_t *obj) {
    if(is_button_pressed(ctx, obj)) {
        return CRACK_EXIT_SUCCESS;
    }
    return CRACK_SUCCESS;
}

crack_state_t handle_button_again(crack_t *ctx, screen_t *screen, object_t *obj) {
    if(is_button_pressed(ctx, obj)) {
        return CRACK_PLAY_AGAIN;
    }
    return CRACK_SUCCESS;
}
