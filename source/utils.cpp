/*============================================================================*/

#include "utils.h"

/*============================================================================*/

sf::Vector2f real_pos(const sf::Vector2f &rel_pos,
                      const sf::Vector2f &screen_size,
                      const sf::Vector2f &size) {
    return sf::Vector2f(rel_pos.x * screen_size.x,
                        rel_pos.y * screen_size.y) * 0.01f - size * 0.5f;
}

/*============================================================================*/

bool is_object_focused(const sf::Vector2f &mouse,
                       const sf::RectangleShape &box) {
    const sf::Vector2f &pos = box.getPosition();
    const sf::Vector2f &size = box.getSize();
    if(pos.x < mouse.x && mouse.x < pos.x + size.x &&
       pos.y < mouse.y && mouse.y < pos.y + size.y) {
        return true;
    }
    return false;
}

/*============================================================================*/

bool box_intersect(const sf::Vector2f &tl1,
                   const sf::Vector2f &tr1,
                   const sf::Vector2f &bl1,
                   const sf::RectangleShape &box2) {
    const sf::Vector2f &tl2 = box2.getPosition();
    sf::Vector2f tr2 = tl2 + sf::Vector2f(box2.getSize().x, 0);
    sf::Vector2f bl2 = tl2 + sf::Vector2f(0, box2.getSize().y);

    if((((tl1.x < tl2.x) && (tl2.x < tr1.x)) ||
        ((tl1.x < tr2.x) && (tr2.x < tr1.x)) ||
        ((tl2.x < tl1.x) && (tl1.x < tr2.x)) ||
        ((tl2.x < tr1.x) && (tr1.x < tr2.x))) &&
       (((tl1.y < tl2.y) && (tl2.y < bl1.y)) ||
        ((tl1.y < bl2.y) && (bl2.y < bl1.y)) ||
        ((tl2.y < tl1.y) && (tl1.y < bl2.y)) ||
        ((tl2.y < bl1.y) && (bl1.y < bl2.y)))) {
        return true;
    }
    return false;
}

/*============================================================================*/

size_t file_size (FILE *file) {
    size_t old_pos = ftell(file);
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, old_pos, SEEK_SET);
    return size;
}

/*============================================================================*/
