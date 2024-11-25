#ifndef APP_H
#define APP_H

#include <SFML/Graphics.hpp>
#include "../grid/Grid.h"
#include "../point/PointManager.h"

class App {
public:
    App();
    void run();

private:
    sf::RenderWindow window;
    sf::Clock appClock; // заменили имя переменной
    Grid grid;
    PointManager pointManager;

    void processEvents();
    void render();
};

#endif // APP_H