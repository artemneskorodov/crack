/*============================================================================*/

#ifndef OBJECTS_H
#define OBJECTS_H

/*============================================================================*/

#include "crack.h"

/*============================================================================*/

static const size_t         EnemiesNum          = 20;
static const size_t         BulletsNum          = 15;
static const size_t         MaxDecorationFrames = 10;
static const size_t         MaxPBarFrames       = 10;
static const size_t         MaxTextBufferSize   = 64;

/*============================================================================*/

struct button_t {
    sf::RectangleShape  box;
    sf::IntRect         focused;
    sf::IntRect         unfocused;
};

/*============================================================================*/

struct textbox_t {
    char                buffer[MaxTextBufferSize];
    size_t              position;
    size_t              capacity;
    sf::Text            text;
    sf::Font            font;
    sf::RectangleShape  box;
    sf::IntRect         focused;
    sf::IntRect         unfocused;
    sf::SoundBuffer     text_sound_buffer;
    sf::Sound           text_sound;
};

/*============================================================================*/

struct decoration_t {
    sf::RectangleShape  box;
    sf::Time            update_time;
    sf::Clock           timer;
    sf::IntRect         frames[MaxDecorationFrames];
    size_t              offsets_num;
    size_t              current_frame;
};

/*============================================================================*/

struct pbar_t {
    sf::RectangleShape  box;
    sf::IntRect         frames[MaxPBarFrames];
    size_t              frames_num;
    size_t              current_frame;
};

/*============================================================================*/

struct pool_t {
    int                 prev;
    int                 next;
};

/*============================================================================*/

struct player_t {
    sf::RectangleShape  box;
    sf::Vector2f        velocity;
    float               speed;
    sf::Vector2f        start_pos;
};

/*============================================================================*/

struct enemies_t {
    sf::RectangleShape  boxes[EnemiesNum];
    float               velocities[EnemiesNum];
    pool_t              pool[EnemiesNum];
    int                 active_head;
    int                 inactive_head;
    sf::IntRect         textures[3];
    float               spawn_rate;
    int                 hps[EnemiesNum];
    float               max_velocity;
    float               min_velocity;
    sf::Clock           last_hit[EnemiesNum];
    sf::Time            cool_down;
    sf::SoundBuffer     death_sound_buffer;
    sf::SoundBuffer     hit_sound_buffer;
    sf::Sound           death_sound;
    sf::Sound           hit_sound;
};

/*============================================================================*/

struct bullets_t {
    sf::RectangleShape  boxes[BulletsNum];
    float               velocity;
    sf::Clock           last_shot;
    sf::Time            shot_time;
    pool_t              pool[BulletsNum];
    int                 active_head;
    int                 inactive_head;
};

/*============================================================================*/

static const size_t PlayerObjectsNum        = 1;
static const size_t EnemiesObjectsNum       = 1;
static const size_t BulletsObjectsNum       = 1;
static const size_t ButtonObjectNum         = 9;
static const size_t TextboxObjectsNum       = 1;
static const size_t DecorationObjectsNum    = 1;
static const size_t PBarObjectsNum          = 1;

/*============================================================================*/

struct objects_storage_t {
    player_t            player_objs[PlayerObjectsNum];
    size_t              used_players;
    enemies_t           enemies_objs[EnemiesObjectsNum];
    size_t              used_enemies;
    bullets_t           bullets_objs[BulletsObjectsNum];
    size_t              used_bullets;
    button_t            button_objs[ButtonObjectNum];
    size_t              used_buttons;
    textbox_t           textbox_objs[TextboxObjectsNum];
    size_t              used_textboxs;
    decoration_t        decoration_objs[DecorationObjectsNum];
    size_t              used_decorations;
    pbar_t              pbar_objs[PBarObjectsNum];
    size_t              used_pbars;
};

/*============================================================================*/

void           *get_free_object        (void               *storage,
                                        object_types_t      object_type);

/*============================================================================*/

#endif

/*============================================================================*/
