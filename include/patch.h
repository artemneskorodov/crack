/*============================================================================*/

#ifndef PATCH_H
#define PATCH_H

/*============================================================================*/

#include "crack.h"
#include "common.h"

/*============================================================================*/

static const char      *NewPassword     = "SeSo";
static const size_t     NewPasswordLen  = 4;

/*============================================================================*/

crack_state_t       patch              (const char             *filename);

crack_state_t       patch_failed_ctor  (crack_t                *ctx,
                                        screen_t               *screen);

crack_state_t       patch_success_ctor (crack_t                *ctx,
                                        screen_t               *screen);

/*============================================================================*/

#endif

/*============================================================================*/
