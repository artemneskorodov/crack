#ifndef OBJECTS_H
#define OBJECTS_H

#include "crack.h"

static const size_t EnemiesNum              = 20;
static const size_t BulletsNum              = 15;
static const size_t MaxDecorationFrames     = 10;
static const size_t MaxPBarFrames           = 10;

struct button_t {
    sf::RectangleShape  box;
    sf::IntRect         focused;
    sf::IntRect         unfocused;
};

struct textbox_t {
    char               *buffer;
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

struct decoration_t {
    sf::RectangleShape  box;
    sf::Time            update_time;
    sf::Clock           timer;
    sf::IntRect         frames[MaxDecorationFrames];
    size_t              offsets_num;
    size_t              current_frame;
};

struct pbar_t {
    sf::RectangleShape  box;
    sf::IntRect         frames[MaxPBarFrames];
    size_t              frames_num;
    size_t              current_frame;
};

struct pool_t {
    int                 prev;
    int                 next;
};

struct player_t {
    sf::RectangleShape  box;
    sf::Vector2f        velocity;
    float               speed;
    sf::Vector2f        start_pos;
};

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

struct bullets_t {
    sf::RectangleShape  boxes[BulletsNum];
    float               velocity;
    sf::Clock           last_shot;
    sf::Time            shot_time;
    pool_t              pool[BulletsNum];
    int                 active_head;
    int                 inactive_head;
};

#endif
