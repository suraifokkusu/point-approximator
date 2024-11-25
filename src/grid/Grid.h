#ifndef GRID_H
#define GRID_H

#include <SFML/Graphics.hpp>

class Grid {
public:
    Grid();
    void draw(sf::RenderWindow& window) const;

private:
    const int GRID_SIZE = 50; // Размер клеток
};

#endif // GRID_H