//
// Created by mnm on 25.11.2024.
//

#include "Grid.h"

Grid::Grid() {}

void Grid::draw(sf::RenderWindow& window) const {
    sf::Color gridColor(50, 50, 50); // Цвет линий сетки

    // Вертикальные линии
    for (float x = 0; x <= windowWidth; x += GRID_SIZE) {
        sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(x, 0), gridColor),
                sf::Vertex(sf::Vector2f(x, windowHeight), gridColor)
        };
        window.draw(line, 2, sf::Lines);
    }

    // Горизонтальные линии
    for (float y = 0; y <= windowHeight; y += GRID_SIZE) {
        sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(0, y), gridColor),
                sf::Vertex(sf::Vector2f(windowWidth, y), gridColor)
        };
        window.draw(line, 2, sf::Lines);
    }
}

void Grid::setWindowSize(float width, float height) {
    windowWidth = width;
    windowHeight = height;
}