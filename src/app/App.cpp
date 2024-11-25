#include "App.h"

App::App() : window(sf::VideoMode(800, 600), "Point Approximator"), grid(), pointManager() {
    window.setFramerateLimit(60);
}

void App::run() {
    while (window.isOpen()) {
        processEvents();
        render();
    }
}

void App::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        pointManager.handleEvent(event, window);
    }
}

void App::render() {
    sf::Time elapsed = appClock.restart();
    window.clear();
    grid.draw(window); // рисуем сетку
    pointManager.drawColorDivision(window, elapsed); // рисуем разделение экрана
    pointManager.draw(window); // рисуем точки и линию
    window.display();
}