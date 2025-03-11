/*============================================================================*/

#ifndef GAME_H
#define GAME_H

/*============================================================================*/

#include "objects.h"
#include "crack.h"

/*============================================================================*/

crack_state_t   game_ctor              (crack_t            *ctx,
                                        screen_t           *screen);

crack_state_t player_constructor       (crack_t            *ctx,
                                        object_t           *obj,
                                        const object_info_t *obj_info);

crack_state_t player_updater           (crack_t            *ctx,
                                        object_t           *obj);

crack_state_t enemies_constructor      (crack_t            *ctx,
                                        object_t           *obj,
                                        const object_info_t *obj_info);

crack_state_t enemies_updater          (crack_t            *ctx,
                                        object_t           *obj);

crack_state_t bullets_constructor      (crack_t            *ctx,
                                        object_t           *obj,
                                        const object_info_t *obj_info);

crack_state_t bullets_updater          (crack_t            *ctx,
                                        object_t           *obj);

/*============================================================================*/

#endif

/*============================================================================*/
