#ifndef VICTORY_H
#define VICTORY_H

#include "objects.h"
#include "crack.h"
#include "common.h"

crack_state_t   victory_ctor           (crack_t            *ctx,
                                        object_type_t      *objects_types,
                                        screen_t           *screen);

#endif
