#include "crack.h"

crack_state_t handle_mouse_moved(crack_t *ctx, screen_t *screen, sf::Event &event);
crack_state_t handle_mouse_pressed(crack_t *ctx, screen_t *screen);
crack_state_t handle_text_entered(crack_t *ctx, screen_t *screen, sf::Event &event);

crack_state_t create_object(crack_t *ctx, updater_t updater, constructor_t constructor, destructor_t destructor, object_type_t *obj_type) {
    object_type_t type = (object_type_t)ctx->objects_number;
    ctx->objects_number++;

    if(ctx->constructor[type] != NULL || ctx->destructor[type] != NULL || ctx->updater[type] != NULL) {
        return CRACK_TYPE_IN_USE;
    }
    ctx->constructor[type] = constructor;
    ctx->updater[type] = updater;
    ctx->destructor[type] = destructor;

    *obj_type = type;
    return CRACK_SUCCESS;
}

crack_state_t object_ctor(crack_t *ctx, object_t *obj, const object_info_t *obj_info, object_type_t type) {
    obj->type = type;
    obj->texture.loadFromFile(obj_info->texture);

    obj->on_mouse_click      = obj_info->on_mouse_click;
    obj->on_mouse_move       = obj_info->on_mouse_move;
    obj->handle_buttons      = obj_info->handle_buttons;
    obj->on_text_entered     = obj_info->on_text_entered;

    _RETURN_IF_ERROR(ctx->constructor[type](ctx, obj, obj_info));
    return CRACK_SUCCESS;
}

#define _END_SCREEN_IF_ERROR(...) {                 \
    crack_state_t _error_code = (__VA_ARGS__);      \
    if(_error_code != CRACK_SUCCESS) {              \
        screen->music.stop();                       \
        if(screen->unloader != NULL) {              \
            screen->unloader(ctx, screen);          \
        }                                           \
        return _error_code;                         \
    }                                               \
}                                                   \

crack_state_t run_screen(crack_t *ctx, screen_t *screen) {
    screen->music.play();
    while(ctx->win.isOpen()) {
        sf::Event event;
        while(ctx->win.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                ctx->win.close();
                return CRACK_EXIT_SUCCESS;
            }
            else if(event.type == sf::Event::MouseMoved) {
                _END_SCREEN_IF_ERROR(handle_mouse_moved(ctx, screen, event));
            }
            else if(event.type == sf::Event::MouseButtonPressed) {
                _END_SCREEN_IF_ERROR(handle_mouse_pressed(ctx, screen));
            }
            else if(event.type == sf::Event::TextEntered) {
                _END_SCREEN_IF_ERROR(handle_text_entered(ctx, screen, event));
            }

            for(size_t i = 0; i < screen->objects_num; i++) {
                if(screen->objects[i].handle_buttons != NULL) {
                    _END_SCREEN_IF_ERROR(screen->objects[i].handle_buttons(ctx, &screen->objects[i]));
                }
            }
        }
        ctx->win.clear(sf::Color::Black);

        if(screen->updater != NULL) {
            _END_SCREEN_IF_ERROR(screen->updater(ctx, screen));
        }

        for(size_t i = 0; i < screen->objects_num; i++) {
            size_t updater = (size_t)screen->objects[i].type;
            _END_SCREEN_IF_ERROR(ctx->updater[updater](ctx, screen->objects + i));
        }

        ctx->win.display();

        if(screen->music.getStatus() == sf::Music::Stopped) {
            screen->music.setPlayingOffset(sf::seconds(0));
            screen->music.play();
        }
    }
    return CRACK_SUCCESS;
}

#undef _END_SCREEN_IF_ERROR

crack_state_t handle_mouse_moved(crack_t *ctx, screen_t *screen, sf::Event &event) {
    ctx->mouse.x = event.mouseMove.x;
    ctx->mouse.y = event.mouseMove.y;

    for(size_t i = 0; i < screen->objects_num; i++) {
        object_t *obj = &screen->objects[i];
        if(obj->on_mouse_move != NULL) {
            _RETURN_IF_ERROR(obj->on_mouse_move(ctx, obj));
        }
    }
    return CRACK_SUCCESS;
}

crack_state_t handle_mouse_pressed(crack_t *ctx, screen_t *screen) {
    for(size_t i = 0; i < screen->objects_num; i++) {
        object_t *obj = &screen->objects[i];
        if(obj->on_mouse_click != NULL) {
            _RETURN_IF_ERROR(obj->on_mouse_click(ctx, screen, obj));
        }
    }
    return CRACK_SUCCESS;
}

crack_state_t handle_text_entered(crack_t *ctx, screen_t *screen, sf::Event &event) {
    for(size_t i = 0; i < screen->objects_num; i++) {
        object_t *obj = &screen->objects[i];
        if(!obj->waits_for_text) {
            continue;
        }
        if(obj->on_text_entered != NULL) {
            _RETURN_IF_ERROR(obj->on_text_entered(ctx, obj, event));
        }
    }
    return CRACK_SUCCESS;
}

