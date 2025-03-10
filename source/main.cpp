#include "crack.h"
#include "common.h"
#include "menu.h"
#include "objects.h"
#include "patch.h"


int main(void) {
    srand((unsigned int)time(NULL));
    objects_storage_t storage = {};
    crack_t ctx = {};
    ctx.objects_storage = (void *)&storage;
    ctx.get_free_obj = get_free_object;

    crack_context_ctor(&ctx);
    run_screen(&ctx, ctx.screens + SCREEN_MENU);
    return 0;
}
