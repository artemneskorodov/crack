#ifndef GAME_H
#define GAME_H

#include "objects.h"
#include "crack.h"
#include "common.h"

crack_state_t   create_game_objects    (crack_t            *ctx,
                                        object_type_t      *object_types);

crack_state_t   game_ctor              (crack_t            *ctx,
                                        object_type_t      *objects_types,
                                        screen_t           *screen);

#endif
