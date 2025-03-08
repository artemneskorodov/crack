#include "crack.h"
#include "common.h"
#include "menu.h"
// #include "game.h"


int main(void) {
    srand((unsigned int)time(NULL));
    objects_storage_t storage = {};
    crack_t ctx = {};
    ctx.objects_storage = (void *)&storage;
    ctx.get_free_obj = get_free_object;

    object_type_t object_types[OBJECT_INVALID] = {};
    crack_context_ctor(&ctx, object_types);
    run_screen(&ctx, ctx.screens);
    return 0;
}
