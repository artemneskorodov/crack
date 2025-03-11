/*============================================================================*/

#ifndef UTILS_H
#define UTILS_H

/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

/*============================================================================*/

sf::Vector2f    real_pos               (const sf::Vector2f         &rel_pos,
                                        const sf::Vector2f         &screen_size,
                                        const sf::Vector2f         &size);

bool            is_object_focused      (const sf::Vector2f         &mouse,
                                        const sf::RectangleShape   &box);

bool            box_intersect          (const sf::Vector2f         &tl1,
                                        const sf::Vector2f         &tr1,
                                        const sf::Vector2f         &bl1,
                                        const sf::RectangleShape   &box2);

size_t          file_size              (FILE                       *file);

/*============================================================================*/

#endif

/*============================================================================*/
