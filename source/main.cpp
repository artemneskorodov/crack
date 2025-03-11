/*============================================================================*/

#include "crack.h"
#include "common.h"
#include "menu.h"
#include "objects.h"
#include "patch.h"

/*============================================================================*/

int main(void) {
    /*------------------------------------------------------------------------*/
    /* Updating random values                                                 */
    srand((unsigned int)time(NULL));
    /*------------------------------------------------------------------------*/
    /* Creating context structures                                            */
    objects_storage_t storage = {};
    crack_t ctx = {};
    /*------------------------------------------------------------------------*/
    /* Setting common context info                                            */
    ctx.objects_storage = (void *)&storage;
    ctx.get_free_obj = get_free_object;
    /*------------------------------------------------------------------------*/
    /* Calling context constructor                                            */
    if(crack_context_ctor(&ctx) != CRACK_SUCCESS) {
        return EXIT_FAILURE;
    }
    /*------------------------------------------------------------------------*/
    /* Returning retval of menu screen which calls other screens              */
    return (int)run_screen(&ctx, ctx.screens + SCREEN_MENU);
}

/*============================================================================*/
