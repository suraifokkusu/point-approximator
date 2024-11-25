//
// Created by mnm on 25.11.2024.
//

#include "Grid.h"

Grid::Grid() {}

void Grid::draw(sf::RenderWindow& window) const {
    sf::Color gridColor(50, 50, 50);
    sf::Vector2u size = window.getSize();

    // Вертикальные линии
    for (int x = 0; x < static_cast<int>(size.x); x += GRID_SIZE) {
        sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(x, 0), gridColor),
                sf::Vertex(sf::Vector2f(x, static_cast<float>(size.y)), gridColor)
        };
        window.draw(line, 2, sf::Lines);
    }

    // Горизонтальные линии
    for (int y = 0; y < static_cast<int>(size.y); y += GRID_SIZE) {
        sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(0, y), gridColor),
                sf::Vertex(sf::Vector2f(static_cast<float>(size.x), y), gridColor)
        };
        window.draw(line, 2, sf::Lines);
    }
}