#ifndef MENU_H
#define MENU_H

#include "objects.h"
#include "crack.h"

crack_state_t   menu_ctor              (crack_t            *ctx,
                                        object_type_t      *object_types,
                                        screen_t           *screen);

#endif
