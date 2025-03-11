/*============================================================================*/

#include "game.h"
#include "utils.h"
#include "objects.h"
#include "common.h"

/*============================================================================*/

enum game_objects_t {
    GAME_OBJ_PLAYER                         = 0,
    GAME_OBJ_ENEMIES                        = 1,
    GAME_OBJ_BULLETS                        = 2,
    GAME_OBJ_COIN                           = 3,
    GAME_OBJ_PBAR                           = 4,
};

/*============================================================================*/

struct player_info_t {
    sf::Vector2f        start_position;
    float               velocity;
};

/*============================================================================*/

struct enemies_info_t {
    float               min_velocity;
    float               max_velocity;
    float               spawn_rate;
    sf::Vector2i        textures[3];
    sf::Vector2i        texture_size;
    sf::Time            cool_down;
    const char         *death_sound;
    const char         *hit_sound;
};

/*============================================================================*/

struct bullets_info_t {
    float               velocity;
    sf::Time            shot_time;
};

/*============================================================================*/

static const float          DeltaTime           = 0.05f;
static const char          *GameMusicFile       = "./styles/music/game_music.mp3";
static const char          *GameBackground      = "./styles/img/game_background.png";
static const sf::Time       GameBckgUpdateTime  = sf::milliseconds(500);
static const sf::IntRect    GameBckgRects[]     = {
    sf::IntRect(   0,        0, 3200, 2560),
    sf::IntRect(3200,        0, 3200, 2560),
    sf::IntRect(   0,     2560, 3200, 2560),
    sf::IntRect(3200,     2560, 3200, 2560),
    sf::IntRect(   0, 2 * 2560, 3200, 2560),
};

/*============================================================================*/

static crack_state_t player_buttons_handler            (crack_t            *ctx,
                                                        object_t           *obj);

static crack_state_t game_updater                      (crack_t            *ctx,
                                                        screen_t           *screen);

static crack_state_t check_bullets_enemies_collisions  (crack_t            *ctx,
                                                        screen_t           *screen,
                                                        bullets_t          *bullets,
                                                        enemies_t          *enemies,
                                                        pbar_t             *pbar);

static crack_state_t check_player_enemies_collisions   (crack_t            *ctx,
                                                        screen_t           *screen,
                                                        player_t           *player,
                                                        enemies_t          *enemies);

static crack_state_t check_bullets_timer               (crack_t            *ctx,
                                                        screen_t           *screen,
                                                        bullets_t          *bullets,
                                                        player_t           *player);

static crack_state_t set_active                        (pool_t             *pool,
                                                        int                 elem,
                                                        int                *active,
                                                        int                *inactive);

static crack_state_t set_inactive                      (pool_t             *pool,
                                                        int                 elem,
                                                        int                *active,
                                                        int                *inactive);

static crack_state_t reset_pool                        (pool_t             *pool,
                                                        size_t              size,
                                                        int                *active,
                                                        int                *inactive);

static crack_state_t check_bullet_collisions           (crack_t            *ctx,
                                                        screen_t           *screen,
                                                        enemies_t          *enemies,
                                                        const sf::Vector2f &bullet_tl,
                                                        const sf::Vector2f &bullet_tr,
                                                        const sf::Vector2f &bullet_bl,
                                                        pbar_t             *pbar);

static crack_state_t unload_game                       (crack_t            *ctx,
                                                        screen_t           *screen);

/*============================================================================*/

static const player_info_t GamePlayerPrivate = {
    .start_position         = sf::Vector2f(50, 50),
    .velocity               = 7.f * DeltaTime,
};

static const object_info_t GamePlayerPublic = {
    .on_mouse_click         = NULL,
    .on_mouse_move          = NULL,
    .on_text_entered        = NULL,
    .handle_buttons         = player_buttons_handler,
    .texture                = "./styles/img/game_player.png",
    .size                   = sf::Vector2f(23, 20) * 4.f,
    .object_private_info    = (const void *)&GamePlayerPrivate,
};

/*============================================================================*/

static const enemies_info_t GameEnemiesPrivate = {
    .min_velocity           = 3.f * DeltaTime,
    .max_velocity           = 6.f * DeltaTime,
    .spawn_rate             = 0.07f,
    .texture_size           = sf::Vector2i(220, 440),
    .textures               = { sf::Vector2i(  0, 440),
                                sf::Vector2i(220,   0),
                                sf::Vector2i(  0,   0)},
    .cool_down              = sf::milliseconds(100),
    .death_sound            = "./styles/sounds/enemy_death.wav",
    .hit_sound              = "./styles/sounds/enemy_hit.wav"
};

static const object_info_t GameEnemiesPublic = {
    .on_mouse_click         = NULL,
    .on_mouse_move          = NULL,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/game_enemy.png",
    .size                   = sf::Vector2f(1, 2) * 35.f,
    .object_private_info    = (const void *)&GameEnemiesPrivate,
};

/*============================================================================*/

static const bullets_info_t GameBulletsPrivate = {
    .velocity               = 20 * DeltaTime,
    .shot_time              = sf::milliseconds(300),
};

static const object_info_t GameBulletsPublic = {
    .on_mouse_click         = NULL,
    .on_mouse_move          = NULL,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/game_bullet.png",
    .size                   = sf::Vector2f(1, 1) * 5.f,
    .object_private_info    = (const void *)&GameBulletsPrivate,
};

/*============================================================================*/

static const decoration_info_t GameCoinPrivate = {
    .pos                    = sf::Vector2f(97, 3),
    .update_time            = sf::milliseconds(80),
    .texture_offsets_num    = 4,
    .texture_offsets        = { sf::Vector2i(  0,   0),
                                sf::Vector2i(420,   0),
                                sf::Vector2i(  0, 420),
                                sf::Vector2i(420, 420)},
    .texture_size           = sf::Vector2i(420, 420),
};

static const object_info_t GameCoinPublic = {
    .on_mouse_click         = NULL,
    .on_mouse_move          = NULL,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/game_coin.png",
    .size                   = sf::Vector2f(1, 1) * 49.f,
    .object_private_info    = (const void *)&GameCoinPrivate,
};

/*============================================================================*/

static const pbar_info_t GamePBarPrivate = {
    .frame_size = sf::Vector2i(640, 140),
    .frames_num = 10,
    .pos = sf::Vector2f(85, 3),
};

static const object_info_t GamePBarPublic = {
    .on_mouse_click         = NULL,
    .on_mouse_move          = NULL,
    .on_text_entered        = NULL,
    .handle_buttons         = NULL,
    .texture                = "./styles/img/game_progress_bar.png",
    .size                   = sf::Vector2f(32, 7) * 7.f,
    .object_private_info    = (const void *)&GamePBarPrivate,
};

/*============================================================================*/

crack_state_t game_ctor(crack_t *ctx, screen_t *screen) {
    screen->objects_num = 5;
    _RETURN_IF_ERROR(screen_ctor   (screen,
                                    GameMusicFile,
                                    GameBackground,
                                    game_updater,
                                    unload_game));

    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + GAME_OBJ_PLAYER,
                                    &GamePlayerPublic,
                                    OBJECT_PLAYER));
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + GAME_OBJ_ENEMIES,
                                    &GameEnemiesPublic,
                                    OBJECT_ENEMIES));
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + GAME_OBJ_BULLETS,
                                    &GameBulletsPublic,
                                    OBJECT_BULLETS));
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + GAME_OBJ_COIN,
                                    &GameCoinPublic,
                                    OBJECT_DECOR));
    _RETURN_IF_ERROR(object_ctor   (ctx,
                                    screen->objects + GAME_OBJ_PBAR,
                                    &GamePBarPublic,
                                    OBJECT_PBAR));
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t game_updater(crack_t *ctx, screen_t *screen) {
    /*------------------------------------------------------------------------*/
    /* Updating background                                                    */
    if(screen->timer.getElapsedTime() > GameBckgUpdateTime) {
        screen->timer.restart();
        screen->counter = (screen->counter + 1) % (sizeof(GameBckgRects) /
                                                   sizeof(GameBckgRects[0]));
        screen->box.setTextureRect(GameBckgRects[screen->counter]);
    }
    ctx->win.draw(screen->box);
    /*------------------------------------------------------------------------*/
    /* Defining structures pointers to use easy                               */
    object_t *player_obj  = screen->objects + GAME_OBJ_PLAYER;
    object_t *enemies_obj = screen->objects + GAME_OBJ_ENEMIES;
    object_t *bullets_obj = screen->objects + GAME_OBJ_BULLETS;
    object_t *pbar_obj    = screen->objects + GAME_OBJ_PBAR;
    player_t  *player  = (player_t  *)player_obj ->object_private;
    enemies_t *enemies = (enemies_t *)enemies_obj->object_private;
    bullets_t *bullets = (bullets_t *)bullets_obj->object_private;
    pbar_t    *pbar    = (pbar_t    *)pbar_obj   ->object_private;
    /*------------------------------------------------------------------------*/
    /* Calling game events checkers                                           */
    _RETURN_IF_ERROR(check_bullets_enemies_collisions(ctx,
                                                      screen,
                                                      bullets,
                                                      enemies,
                                                      pbar));
    _RETURN_IF_ERROR(check_player_enemies_collisions (ctx,
                                                      screen,
                                                      player,
                                                      enemies));
    _RETURN_IF_ERROR(check_bullets_timer             (ctx,
                                                      screen,
                                                      bullets,
                                                      player));
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t check_bullets_timer(crack_t      *ctx,
                                  screen_t     *screen,
                                  bullets_t    *bullets,
                                  player_t     *player) {
    /*------------------------------------------------------------------------*/
    /* Checking if bullet can be shoot                                        */
    if(bullets->last_shot.getElapsedTime() < bullets->shot_time) {
        return CRACK_SUCCESS;
    }
    /*------------------------------------------------------------------------*/
    /* Restarting timer                                                       */
    bullets->last_shot.restart();
    /*------------------------------------------------------------------------*/
    /* Getting new bullet index, checking if there are free bulelts           */
    int new_bullet = bullets->inactive_head;
    if(new_bullet == -1) {
        return CRACK_SUCCESS;
    }
    /*------------------------------------------------------------------------*/
    /* Setting bullet active                                                  */
    _RETURN_IF_ERROR(set_active(bullets->pool,
                                new_bullet,
                                &bullets->active_head,
                                &bullets->inactive_head));
    /*------------------------------------------------------------------------*/
    /* Setting bullet position to players center                              */
    sf::RectangleShape &bullet_box = bullets->boxes[new_bullet];
    sf::Vector2f pos = player->box.getPosition();
    pos.x += (player->box.getSize().x - bullet_box.getSize().x) * 0.5f;
    bullet_box.setPosition(pos);
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t check_bullets_enemies_collisions(crack_t     *ctx,
                                               screen_t    *screen,
                                               bullets_t   *bullets,
                                               enemies_t   *enemies,
                                               pbar_t      *pbar) {
    /*------------------------------------------------------------------------*/
    /* Getting head of linked list with active bullets                        */
    int bullet = bullets->active_head;
    /*------------------------------------------------------------------------*/
    while(bullet != -1) {
        /*--------------------------------------------------------------------*/
        /* Gettinng next bullet index                                         */
        int bullet_next = bullets->pool[bullet].next;
        /*--------------------------------------------------------------------*/
        /* Getting bullet rect                                                */
        sf::RectangleShape &bullet_box = bullets->boxes[bullet];
        /*--------------------------------------------------------------------*/
        /* Calculating bullet top_left, top_right and bottom_left             */
        const sf::Vector2f &tl = bullet_box.getPosition();
        sf::Vector2f tr = tl + sf::Vector2f(bullet_box.getSize().x, 0);
        sf::Vector2f bl = tl + sf::Vector2f(0, bullet_box.getSize().y);
        /*--------------------------------------------------------------------*/
        /* Checking collision of this bullet with all enemies                 */
        _RETURN_IF_ERROR(check_bullet_collisions(ctx,
                                                 screen,
                                                 enemies,
                                                 tl,
                                                 tr,
                                                 bl,
                                                 pbar));
        /*--------------------------------------------------------------------*/
        /* Setting index to next                                              */
        bullet = bullet_next;
    }
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t check_bullet_collisions(crack_t              *ctx,
                                      screen_t             *screen,
                                      enemies_t            *enemies,
                                      const sf::Vector2f   &bullet_tl,
                                      const sf::Vector2f   &bullet_tr,
                                      const sf::Vector2f   &bullet_bl,
                                      pbar_t               *pbar) {
    /*------------------------------------------------------------------------*/
    /* Getting head of linked list with active enemies                        */
    int enemy = enemies->active_head;
    /*------------------------------------------------------------------------*/
    while(enemy != -1) {
        /*--------------------------------------------------------------------*/
        /* Getting next enemy index                                           */
        int next = enemies->pool[enemy].next;
        /*--------------------------------------------------------------------*/
        /* Checking if enemy timer and rect                                   */
        sf::Clock &last_hit = enemies->last_hit[enemy];
        sf::RectangleShape &enemy_box = enemies->boxes[enemy];
        /*--------------------------------------------------------------------*/
        /* Checking if timer ellapsed and bullet intersects with enemy rect   */
        if(last_hit.getElapsedTime() > (enemies->cool_down) &&
           box_intersect(bullet_tl, bullet_tr, bullet_bl, enemy_box)) {
            /*----------------------------------------------------------------*/
            /* If enemy is dead after hit                                     */
            if(enemies->hps[enemy] == 1) {
                /*------------------------------------------------------------*/
                /* Setting enemy inactive                                     */
                _RETURN_IF_ERROR(set_inactive(enemies->pool,
                                              enemy,
                                              &enemies->active_head,
                                              &enemies->inactive_head));
                /*------------------------------------------------------------*/
                /* Updating score                                             */
                pbar->current_frame++;
                if(pbar->current_frame == pbar->frames_num) {
                    return CRACK_GAME_WON;
                }
                pbar->box.setTextureRect(pbar->frames[pbar->current_frame]);
                /*------------------------------------------------------------*/
                /* Playing sound                                              */
                enemies->death_sound.play();
                /*------------------------------------------------------------*/
            }
            /* If enemy is alive after hit                                    */
            else {
                /*------------------------------------------------------------*/
                /* Updating health points and texture                         */
                enemies->hps[enemy]--;
                int hp = enemies->hps[enemy];
                enemy_box.setTextureRect(enemies->textures[hp - 1]);
                /*------------------------------------------------------------*/
                /* Playing sound                                              */
                enemies->hit_sound.play();
                /*------------------------------------------------------------*/
            }
            /*----------------------------------------------------------------*/
            /* Restaring cool down timer                                      */
            last_hit.restart();
        }
        /*--------------------------------------------------------------------*/
        /* Moving index to next enemy                                         */
        enemy = next;
    }
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t check_player_enemies_collisions(crack_t      */*ctx*/,
                                              screen_t     */*screen*/,
                                              player_t     *player,
                                              enemies_t    *enemies) {
    /*------------------------------------------------------------------------*/
    /* Getting players top_left, top_right and bottom_left                    */
    const sf::Vector2f &tl = player->box.getPosition();
    sf::Vector2f tr = tl + sf::Vector2f(player->box.getSize().x, 0);
    sf::Vector2f bl = tl + sf::Vector2f(0, player->box.getSize().y);
    /*------------------------------------------------------------------------*/
    /* Getting head of linked list with active enemies                        */
    int enemy = enemies->active_head;
    /*------------------------------------------------------------------------*/
    /* Running through active enemies                                         */
    while(enemy != -1) {
        /*--------------------------------------------------------------------*/
        /* Getting next enemy index                                           */
        int next = enemies->pool[enemy].next;
        /*--------------------------------------------------------------------*/
        /* Getting enemy box                                                  */
        sf::RectangleShape &enemy_box = enemies->boxes[enemy];
        /*--------------------------------------------------------------------*/
        /* Checking if intersects with enemy                                  */
        if(box_intersect(tl, tr, bl, enemy_box)) {
            return CRACK_PLAYER_LOST;
        }
        /*--------------------------------------------------------------------*/
        /* Moving index to next enemy                                         */
        enemy = next;
    }
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t player_constructor(crack_t               *ctx,
                                 object_t              *obj,
                                 const object_info_t   *obj_info) {
    /*------------------------------------------------------------------------*/
    /* Getting static structure from storage                                  */
    obj->object_private = ctx->get_free_obj(ctx->objects_storage,
                                            OBJECT_PLAYER);
    /*------------------------------------------------------------------------*/
    /* Defining structures for easy use of them                               */
    player_t *player = (player_t *)obj->object_private;
    const player_info_t *info = (const player_info_t *)
                                obj_info->object_private_info;
    /*------------------------------------------------------------------------*/
    /* Calculating box size and setting box info                              */
    sf::Vector2f pos = real_pos(info->start_position,
                                ScreenSize,
                                obj_info->size);
    player->box.setSize    (obj_info->size);
    player->box.setPosition(pos           );
    player->box.setTexture (&obj->texture );
    /*------------------------------------------------------------------------*/
    /* Setting players info and init velocity to zeros                        */
    player->velocity = sf::Vector2f(0, 0);
    player->speed = info->velocity;
    /*------------------------------------------------------------------------*/
    /* Start position will be needed when reinitializing the game             */
    player->start_pos = player->box.getPosition();
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t player_updater(crack_t *ctx, object_t *obj) {
    /*------------------------------------------------------------------------*/
    /* Calculating new position                                               */
    player_t *player = (player_t *)obj->object_private;
    sf::Vector2f new_pos = player->box.getPosition() + player->velocity;
    /*------------------------------------------------------------------------*/
    /* Checking for left and right borders collisions                         */
    if(new_pos.x < 0) {
        new_pos.x = 0;
    }
    else if(new_pos.x > ScreenSize.x - player->box.getSize().x) {
        new_pos.x = ScreenSize.x - player->box.getSize().x;
    }
    /*------------------------------------------------------------------------*/
    /* Checking for top and bottom borders collisions                         */
    if(new_pos.y < 0) {
        new_pos.y = 0;
    }
    else if(new_pos.y > ScreenSize.y - player->box.getSize().y) {
        new_pos.y = ScreenSize.y - player->box.getSize().y;
    }
    /*------------------------------------------------------------------------*/
    /* Updating box position and drawing it on window                         */
    player->box.setPosition(new_pos);
    ctx->win.draw(player->box);
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t enemies_constructor(crack_t             *ctx,
                                  object_t            *obj,
                                  const object_info_t *obj_info) {
    /*------------------------------------------------------------------------*/
    /* Getting static structure from storage                                  */
    obj->object_private = ctx->get_free_obj(ctx->objects_storage,
                                            OBJECT_ENEMIES);
    /*------------------------------------------------------------------------*/
    /* Defining structures for easy use of them                               */
    enemies_t *enemies = (enemies_t *)obj->object_private;
    const enemies_info_t *info = (const enemies_info_t *)
                                 obj_info->object_private_info;
    /*------------------------------------------------------------------------*/
    /* Creating rects of texture with different hps                           */
    for(size_t i = 0; i < 3; i++) {
        sf::IntRect &rect = enemies->textures[i];
        rect = sf::IntRect(info->textures[i].x,
                           info->textures[i].y,
                           info->texture_size.x,
                           info->texture_size.y);
    }
    /*------------------------------------------------------------------------*/
    /* Setting eveny enemy box info                                           */
    for(size_t i = 0; i < EnemiesNum; i++) {
        sf::RectangleShape &box = enemies->boxes[i];
        box.setSize       (obj_info->size          );
        box.setTexture    (&obj->texture           );
        box.setTextureRect(enemies->textures[3 - 1]);
    }
    /*------------------------------------------------------------------------*/
    /* Resetting enemies pool                                                 */
    _RETURN_IF_ERROR(reset_pool(enemies->pool,
                                EnemiesNum,
                                &enemies->active_head,
                                &enemies->inactive_head));
    /*------------------------------------------------------------------------*/
    /* Setting enemies spawn info                                             */
    enemies->spawn_rate    = info->spawn_rate;
    enemies->max_velocity  = info->max_velocity;
    enemies->min_velocity  = info->min_velocity;
    enemies->cool_down     = info->cool_down;
    /*------------------------------------------------------------------------*/
    /* Loading and setting sounds                                             */
    enemies->death_sound_buffer.loadFromFile(info->death_sound);
    enemies->hit_sound_buffer  .loadFromFile(info->hit_sound  );
    enemies->death_sound.setBuffer(enemies->death_sound_buffer);
    enemies->hit_sound  .setBuffer(enemies->hit_sound_buffer  );
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t enemies_updater(crack_t *ctx, object_t *obj) {
    /*------------------------------------------------------------------------*/
    /* Getting head of active enemies                                         */
    enemies_t *enemies = (enemies_t *)obj->object_private;
    int elem = enemies->active_head;
    /*------------------------------------------------------------------------*/
    /* Running through enemies                                                */
    while(elem != -1) {
        /*--------------------------------------------------------------------*/
        /* Getting next enemy index                                           */
        int next = enemies->pool[elem].next;
        /*--------------------------------------------------------------------*/
        /* Getting enemy box and enemy position                               */
        sf::RectangleShape &box = enemies->boxes[elem];
        sf::Vector2f new_pos = box.getPosition();
        /*--------------------------------------------------------------------*/
        /* Calculating new position                                           */
        new_pos += sf::Vector2f(0, enemies->velocities[elem]);
        /*--------------------------------------------------------------------*/
        /* Setting enemy inactive if bottom border was reached                */
        if(new_pos.y > ScreenSize.y) {
            _RETURN_IF_ERROR(set_inactive(enemies->pool,
                                          elem,
                                          &enemies->active_head,
                                          &enemies->inactive_head));
        }
        /*--------------------------------------------------------------------*/
        /* Updating position and drawing if bottom border was not reached     */
        else {
            box.setPosition(new_pos);
            ctx->win.draw(box);
        }
        /*--------------------------------------------------------------------*/
        /* Moving enemy index to next in linked list                          */
        elem = next;
    }
    /*------------------------------------------------------------------------*/
    /* Getting random value for spawn                                         */
    float rand_val = (float)(rand() % 10000) * 0.01f;
    /*------------------------------------------------------------------------*/
    /* Creating new enemy if random value as percents is less than spawn rate */
    if(enemies->spawn_rate > rand_val) {
        /*--------------------------------------------------------------------*/
        /* Getting enemy inactive head                                        */
        int new_elem = enemies->inactive_head;
        /*--------------------------------------------------------------------*/
        /* Checking if there are inactive enemies                             */
        if(new_elem != -1) {
            /*----------------------------------------------------------------*/
            /* Setting enemy active                                           */
            _RETURN_IF_ERROR(set_active(enemies->pool,
                                        new_elem,
                                        &enemies->active_head,
                                        &enemies->inactive_head));
            /*----------------------------------------------------------------*/
            /* Creating random position                                       */
            sf::Vector2f pos = sf::Vector2f(0, 0);
            pos.x = (float)((unsigned int)rand() % WindowMode.width);
            /*----------------------------------------------------------------*/
            /* Setting position to box                                        */
            sf::RectangleShape &box = enemies->boxes[new_elem];
            box.setPosition(pos - box.getSize());
            /*----------------------------------------------------------------*/
            /* Calculating velocity random value                              */
            float velocity = enemies->min_velocity;
            float max_velocity = enemies->max_velocity;
            velocity += (rand() % 100) * 0.01f * (max_velocity - velocity);
            enemies->velocities[new_elem] = velocity;
            /*----------------------------------------------------------------*/
            /* Setting initial health point and texture                       */
            enemies->hps[new_elem] = 3;
            box.setTextureRect(enemies->textures[3 - 1]);
            /*----------------------------------------------------------------*/
            /* Drawing on window                                              */
            ctx->win.draw(box);
        }
        /*--------------------------------------------------------------------*/
    }
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t bullets_constructor(crack_t              *ctx,
                                  object_t             *obj,
                                  const object_info_t  *obj_info) {
    /*------------------------------------------------------------------------*/
    /* Getting static structure from storage                                  */
    obj->object_private = ctx->get_free_obj(ctx->objects_storage,
                                            OBJECT_BULLETS);
    /*------------------------------------------------------------------------*/
    /* Defining structures for easy use of them                               */
    bullets_t *bullets = (bullets_t *)obj->object_private;
    const bullets_info_t *info = (const bullets_info_t *)
                                 obj_info->object_private_info;
    /*------------------------------------------------------------------------*/
    /* Setting bullets box info                                               */
    for(size_t i = 0; i < BulletsNum; i++) {
        sf::RectangleShape &box = bullets->boxes[i];
        box.setSize(obj_info->size);
        box.setTexture(&obj->texture);
    }
    /*------------------------------------------------------------------------*/
    /* Resetting bullets pool list                                            */
    _RETURN_IF_ERROR(reset_pool(bullets->pool,
                                BulletsNum,
                                &bullets->active_head,
                                &bullets->inactive_head));
    /*------------------------------------------------------------------------*/
    /* Setting velocity and shot time info                                    */
    bullets->velocity      = info->velocity;
    bullets->shot_time     = info->shot_time;
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t bullets_updater(crack_t *ctx, object_t *obj) {
    /*------------------------------------------------------------------------*/
    /* Getting head of active bullets lists                                   */
    bullets_t *bullets = (bullets_t *)obj->object_private;
    int elem = bullets->active_head;
    /*------------------------------------------------------------------------*/
    /* Running through active bullets                                         */
    while(elem != -1) {
        /*--------------------------------------------------------------------*/
        /* Getting next bulelt index                                          */
        int next = bullets->pool[elem].next;
        /*--------------------------------------------------------------------*/
        /* Calculating new bullet position                                    */
        sf::RectangleShape &box = bullets->boxes[elem];
        sf::Vector2f new_pos = box.getPosition();
        new_pos -= sf::Vector2f(0, bullets->velocity);
        /*--------------------------------------------------------------------*/
        /* If top border was reached setting bullet inactive                  */
        if(new_pos.y < 0 - box.getSize().y) {
            _RETURN_IF_ERROR(set_inactive(bullets->pool,
                                          elem,
                                          &bullets->active_head,
                                          &bullets->inactive_head));
        }
        /*--------------------------------------------------------------------*/
        /* Drawing box if border was not reached                              */
        else {
            box.setPosition(new_pos);
            ctx->win.draw(box);
        }
        /*--------------------------------------------------------------------*/
        /* Moving bullet index to next                                        */
        elem = next;
    }
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t set_inactive(pool_t *pool, int elem, int *active, int *inactive) {
    /*------------------------------------------------------------------------*/
    /* Setting next for prev                                                  */
    if(pool[elem].prev != -1) {
        pool[pool[elem].prev].next = pool[elem].next;
    }
    else {
        *active = pool[elem].next;
    }
    /*------------------------------------------------------------------------*/
    /* Setting prev for next                                                  */
    if(pool[elem].next != -1) {
        pool[pool[elem].next].prev = pool[elem].prev;
    }
    /*------------------------------------------------------------------------*/
    /* Updating old inactive head element                                     */
    if(*inactive != -1) {
        pool[*inactive].prev = elem;
    }
    /*------------------------------------------------------------------------*/
    /* Updating element connections                                           */
    pool[elem].prev = -1;
    pool[elem].next = (*inactive);
    /*------------------------------------------------------------------------*/
    /* Updating inactive head                                                 */
    *inactive = elem;
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t set_active(pool_t *pool, int elem, int *active, int *inactive) {
    /*------------------------------------------------------------------------*/
    /* Setting next for prev                                                  */
    if(pool[elem].prev != -1) {
        pool[pool[elem].prev].next = pool[elem].next;
    }
    else {
        *inactive = pool[elem].next;
    }
    /*------------------------------------------------------------------------*/
    /* Setting prev for next                                                  */
    if(pool[elem].next != -1) {
        pool[pool[elem].next].prev = pool[elem].prev;
    }
    /*------------------------------------------------------------------------*/
    /* Updating old active head element                                       */
    if(*active != -1) {
        pool[*active].prev = elem;
    }
    /*------------------------------------------------------------------------*/
    /* Updating elements connections                                          */
    pool[elem].prev = -1;
    pool[elem].next = (*active);
    /*------------------------------------------------------------------------*/
    /* Updating active head                                                   */
    *active = elem;
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t player_buttons_handler(crack_t */*ctx*/, object_t *obj) {
    /*------------------------------------------------------------------------*/
    /* Getting which buttons are pressed                                      */
    player_t *player = (player_t *)obj->object_private;
    bool left  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left );
    bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    bool down  = sf::Keyboard::isKeyPressed(sf::Keyboard::Down );
    bool up    = sf::Keyboard::isKeyPressed(sf::Keyboard::Up   );
    /*------------------------------------------------------------------------*/
    /* Calculating multypliers for speed coordinates                          */
    int mult_x = (int)right - (int)left;
    int mult_y = (int)down  - (int)up;
    /*------------------------------------------------------------------------*/
    /* If two coordinates are involved dividing by sqrt(2)                    */
    if((left != right) && (down != up)) {
        player->velocity = sf::Vector2f(player->speed * mult_x,
                                        player->speed * mult_y) / sqrtf(2);
    }
    /*------------------------------------------------------------------------*/
    /* Just setting speed coordinates if only one speed coordinate is not 0   */
    else {
        player->velocity = sf::Vector2f(player->speed * mult_x,
                                        player->speed * mult_y);
    }
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t unload_game(crack_t *ctx, screen_t *screen) {
    /*------------------------------------------------------------------------*/
    /* Defining structures pointers of obects to use them easy                */
    object_t *player_obj  = screen->objects + GAME_OBJ_PLAYER;
    object_t *enemies_obj = screen->objects + GAME_OBJ_ENEMIES;
    object_t *bullets_obj = screen->objects + GAME_OBJ_BULLETS;
    object_t *pbar_obj    = screen->objects + GAME_OBJ_PBAR;
    player_t  *player  = (player_t  *)player_obj ->object_private;
    enemies_t *enemies = (enemies_t *)enemies_obj->object_private;
    bullets_t *bullets = (bullets_t *)bullets_obj->object_private;
    pbar_t    *pbar    = (pbar_t    *)pbar_obj   ->object_private;
    /*------------------------------------------------------------------------*/
    /* Setting player position to initial                                     */
    player->box.setPosition(player->start_pos);
    /*------------------------------------------------------------------------*/
    /* Resetting enemies pool                                                 */
    _RETURN_IF_ERROR(reset_pool(enemies->pool,
                                EnemiesNum,
                                &enemies->active_head,
                                &enemies->inactive_head));
    /*------------------------------------------------------------------------*/
    /* Resetting bullets pool                                                 */
    _RETURN_IF_ERROR(reset_pool(bullets->pool,
                                BulletsNum,
                                &bullets->active_head,
                                &bullets->inactive_head));
    /*------------------------------------------------------------------------*/
    /* Resetting progress bar                                                 */
    pbar->current_frame = 0;
    pbar->box.setTextureRect(pbar->frames[0]);
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/

crack_state_t reset_pool(pool_t *pool,
                         size_t size,
                         int *active,
                         int *inactive) {
    /*------------------------------------------------------------------------*/
    /* Setting every element next to its actual next                          */
    for(size_t i = 0; i < size; i++) {
        if(i + 1 != size) {
            pool[i].next = (int)i + 1;
        }
        else {
            pool[i].next = -1;
        }
        pool[i].prev = (int)i - 1;
    }
    /*------------------------------------------------------------------------*/
    /* Setting all list to inactive                                           */
    *active   = -1;
    *inactive =  0;
    /*------------------------------------------------------------------------*/
    return CRACK_SUCCESS;
}

/*============================================================================*/
