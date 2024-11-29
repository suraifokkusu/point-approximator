#include "App.h"

App::App() : window(sf::VideoMode(800, 600), "Point Approximator", sf::Style::Default),
             grid(),
             pointManager(),
             lavaLamp(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)) {
    window.setFramerateLimit(60);
    pointManager.setWindowSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
    grid.setWindowSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
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
        else if (event.type == sf::Event::Resized) {
            // Обновляем вид в соответствии с новым размером окна
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));

            // Обновляем размеры в PointManager и LavaLamp
            pointManager.setWindowSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
            lavaLamp.setSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
            grid.setWindowSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
        }

        pointManager.handleEvent(event, window);
    }
}

void App::render() {
    sf::Time elapsed = appClock.restart();
    window.clear();

    if (pointManager.hasPoints()) {
        grid.draw(window);
        pointManager.drawColorDivision(window, elapsed);
        pointManager.draw(window);
    } else {
        lavaLamp.update(elapsed);
        lavaLamp.draw(window);
    }

    window.display();
}