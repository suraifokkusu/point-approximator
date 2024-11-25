#ifndef POINTMANAGER_H
#define POINTMANAGER_H

#include <SFML/Graphics.hpp>
#include <vector>

class PointManager {
public:
    enum class ApproximationType { Linear, Parabolic, Logarithmic };

    PointManager();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void drawColorDivision(sf::RenderWindow& window, sf::Time elapsed);

private:
    struct Point {
        float x, y;
        sf::CircleShape shape;
        sf::Text label;

        // Конструктор по умолчанию
        Point() : x(0), y(0), shape(5) {
            shape.setFillColor(sf::Color::Red);
        }

        // Основной конструктор
        Point(float x, float y, sf::Font& font)
                : x(x), y(y), shape(5) {
            shape.setPosition(x - 5, y - 5);
            shape.setFillColor(sf::Color::Red);

            label.setFont(font);
            label.setString("(" + std::to_string(static_cast<int>(x)) + ", " + std::to_string(static_cast<int>(y)) + ")");
            label.setCharacterSize(12);
            label.setFillColor(sf::Color::White);
            label.setPosition(x + 5, y + 5);
        }
    };

    sf::Font font; // шрифт для отображения текста
    std::vector<Point> points;
    sf::VertexArray approximationLine; // линия аппроксимации
    ApproximationType currentType = ApproximationType::Linear; // текущий тип аппроксимации

    float a = 0; // параметр наклона
    float b = 0; // параметр смещения
    float c = 0; // дополнительный параметр для параболической аппроксимации

    sf::Color color1 = sf::Color(70, 130, 180, 100); // первый цвет (стальной синий)
    sf::Color color2 = sf::Color(255, 127, 80, 100); // второй цвет (коралловый)
    float colorBlend = 0.0f; // степень смешивания цветов
    float blendDirection = 1.0f; // направление смешивания цветов

    void addPoint(float x, float y);
    void removePoint(float x, float y);
    void clearPoints();
    void calculateApproximation();
    void calculateLinearApproximation();
    void calculateParabolicApproximation();
    void calculateLogarithmicApproximation();

    void drawLinearDivision(sf::RenderWindow& window);
    void drawParabolicDivision(sf::RenderWindow& window);
    void drawLogarithmicDivision(sf::RenderWindow& window);
    sf::Color blendColors(const sf::Color& c1, const sf::Color& c2, float blendFactor);
};

#endif // POINTMANAGER_H