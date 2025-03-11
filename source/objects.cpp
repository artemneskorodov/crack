/*============================================================================*/

#include "objects.h"

/*============================================================================*/

void *get_free_object(void *storage_ptr, object_types_t type){
    objects_storage_t *storage = (objects_storage_t *)storage_ptr;
    /*------------------------------------------------------------------------*/
    /* Returning pointers to structures depending on type                     */
    switch(type) {
        case OBJECT_BUTTON: {
            if(storage->used_buttons == ButtonObjectNum) {
                fprintf(stderr, "All buttons are used\n");
                return NULL;
            }
            return &storage->button_objs[storage->used_buttons++];
        }
        case OBJECT_TEXTBOX: {
            if(storage->used_textboxs == TextboxObjectsNum) {
                fprintf(stderr, "All textboxs are used\n");
                return NULL;
            }
            return &storage->textbox_objs[storage->used_textboxs++];
        }
        case OBJECT_PLAYER: {
            if(storage->used_players == PlayerObjectsNum) {
                fprintf(stderr, "All players are used\n");
                return NULL;
            }
            return &storage->player_objs[storage->used_players++];
        }
        case OBJECT_ENEMIES: {
            if(storage->used_enemies == EnemiesObjectsNum) {
                fprintf(stderr, "All enemies are used\n");
                return NULL;
            }
            return &storage->enemies_objs[storage->used_enemies++];
        }
        case OBJECT_BULLETS: {
            if(storage->used_bullets == BulletsObjectsNum) {
                fprintf(stderr, "All bullets are used\n");
                return NULL;
            }
            return &storage->bullets_objs[storage->used_bullets++];
        }
        case OBJECT_DECOR: {
            if(storage->used_decorations == DecorationObjectsNum) {
                fprintf(stderr, "All decorations are used\n");
                return NULL;
            }
            return &storage->decoration_objs[storage->used_decorations++];
        }
        case OBJECT_PBAR: {
            if(storage->used_pbars == PBarObjectsNum) {
                fprintf(stderr, "All progress bars are used\n");
                return NULL;
            }
            return &storage->pbar_objs[storage->used_pbars++];
        }
        case OBJECT_EMPTY:
        case OBJECT_MAX:
        default:
            fprintf(stderr, "Unknown object requested");
            return NULL;
    }
    /*------------------------------------------------------------------------*/
}

/*============================================================================*/
