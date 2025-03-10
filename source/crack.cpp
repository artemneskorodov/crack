#include "crack.h"
#include "common.h"

crack_state_t handle_mouse_moved(crack_t *ctx, screen_t *screen, sf::Event &event);
crack_state_t handle_mouse_pressed(crack_t *ctx, screen_t *screen);
crack_state_t handle_text_entered(crack_t *ctx, screen_t *screen, sf::Event &event);

crack_state_t object_ctor(crack_t *ctx, object_t *obj, const object_info_t *obj_info, object_types_t type) {
    obj->type = type;
    obj->texture.loadFromFile(obj_info->texture);

    obj->on_mouse_click      = obj_info->on_mouse_click;
    obj->on_mouse_move       = obj_info->on_mouse_move;
    obj->handle_buttons      = obj_info->handle_buttons;
    obj->on_text_entered     = obj_info->on_text_entered;

    _RETURN_IF_ERROR(ObjectTypesInfo[type].constructor(ctx, obj, obj_info));
    return CRACK_SUCCESS;
}

crack_state_t screen_ctor(screen_t         *screen,
                          const char       *music,
                          const char       *background,
                          crack_state_t   (*updater )(crack_t *, screen_t *),
                          crack_state_t   (*unloader)(crack_t *, screen_t *)) {
    screen->background.loadFromFile(background);
    screen->box.setPosition(sf::Vector2f(0, 0));
    screen->box.setSize(ScreenSize);
    screen->box.setTexture(&screen->background);
    screen->music.openFromFile(music);
    screen->updater = updater;
    screen->unloader = unloader;
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
            _END_SCREEN_IF_ERROR(ObjectTypesInfo[screen->objects[i].type].updater(ctx, screen->objects + i));
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

