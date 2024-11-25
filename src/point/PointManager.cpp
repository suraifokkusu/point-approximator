#include "PointManager.h"
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <iostream>

template <typename T>
T clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

PointManager::PointManager() : approximationLine(sf::LineStrip, 800) {
    if (!font.loadFromFile("arial.ttf")) {
        throw std::runtime_error("failed to load font from arial.ttf");
    }
}

void PointManager::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        if (event.mouseButton.button == sf::Mouse::Left) {
            addPoint(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
            calculateApproximation();
        } else if (event.mouseButton.button == sf::Mouse::Right) {
            removePoint(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
            calculateApproximation();
        }
    } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::C) {
            clearPoints(); // очистка всех точек
        } else if (event.key.code == sf::Keyboard::Num1) {
            currentType = ApproximationType::Linear;
            calculateApproximation();
        } else if (event.key.code == sf::Keyboard::Num2) {
            currentType = ApproximationType::Parabolic;
            calculateApproximation();
        } else if (event.key.code == sf::Keyboard::Num3) {
            currentType = ApproximationType::Logarithmic;
            calculateApproximation();
        }
    }
}

void PointManager::addPoint(float x, float y) {
    Point point;
    point.x = x;
    point.y = y;
    point.shape.setRadius(5);
    point.shape.setFillColor(sf::Color::Red);
    point.shape.setPosition(x - 5, y - 5);

    point.label.setFont(font); // Установка шрифта
    point.label.setCharacterSize(12);
    point.label.setFillColor(sf::Color::White);
    point.label.setString("(" + std::to_string(static_cast<int>(x)) + ", " + std::to_string(static_cast<int>(y)) + ")");
    point.label.setPosition(x + 10, y - 10);

    points.push_back(point);
    points.emplace_back(x, y, font); // Передаём координаты и шрифт
        if (points.size() >= 2) {
            calculateParabolicApproximation(); // Обновляем аппроксимацию
        }

}

void PointManager::draw(sf::RenderWindow& window) {
    // Рисуем точки
    for (const auto& point : points) {
        sf::CircleShape shape(5);
        shape.setPosition(point.x - 5, point.y - 5);
        shape.setFillColor(sf::Color::Red);
        window.draw(shape);
    }

    // Рисуем параболу, если она рассчитана
    if (approximationLine.getVertexCount() > 0) {
        window.draw(approximationLine);
    }
}
void PointManager::removePoint(float x, float y) {
    points.erase(std::remove_if(points.begin(), points.end(),
                                [x, y](const Point& p) {
                                    return std::hypot(p.x - x, p.y - y) < 10; // удаление точки в радиусе 10 пикселей
                                }), points.end());
}

void PointManager::clearPoints() {
    points.clear();
    approximationLine.clear();
}

void PointManager::calculateApproximation() {
    switch (currentType) {
        case ApproximationType::Linear:
            calculateLinearApproximation();
            break;
        case ApproximationType::Parabolic:
            calculateParabolicApproximation();
            break;
        case ApproximationType::Logarithmic:
            calculateLogarithmicApproximation();
            break;
    }
}

void PointManager::calculateLinearApproximation() {
    if (points.size() < 2) {
        a = 0;
        b = 0;
        return; // аппроксимация невозможна
    }

    float meanX = std::accumulate(points.begin(), points.end(), 0.0f,
                                  [](float sum, const Point& p) { return sum + p.x; }) / points.size();
    float meanY = std::accumulate(points.begin(), points.end(), 0.0f,
                                  [](float sum, const Point& p) { return sum + p.y; }) / points.size();

    float numerator = 0, denominator = 0;
    for (const auto& p : points) {
        numerator += (p.x - meanX) * (p.y - meanY);
        denominator += (p.x - meanX) * (p.x - meanX);
    }

    if (denominator == 0) {
        a = 0; // линия вертикальная
        b = meanY;
    } else {
        a = numerator / denominator;
        b = meanY - a * meanX;
    }
}

void PointManager::calculateParabolicApproximation() {
    if (points.size() < 2) return; // Нужны минимум две точки для аппроксимации

    // Координаты двух точек
    float x1 = points[0].x;
    float y1 = points[0].y;
    float x2 = points[1].x;
    float y2 = points[1].y;

    // Центр и ширина
    float centerX = (x1 + x2) / 2.0f;
    float halfWidth = std::abs(x2 - x1) / 2.0f;

    // Проверяем, чтобы ширина была больше 0
    if (halfWidth == 0) {
        std::cout << "Error: Points are vertically aligned. Cannot calculate parabolic approximation." << std::endl;
        approximationLine.clear();
        return;
    }

    // Высота параболы
    float height = std::abs(y2 - y1);

    // Кривизна параболы
    float a = -4.0f * height / (halfWidth * halfWidth);

    // Базовая линия (минимальная точка)
    float b = std::min(y1, y2);

    // Очистка линии
    approximationLine.clear();

    // Построение линии
    for (float x = centerX - halfWidth; x <= centerX + halfWidth; x += 1.0f) {
        float relativeX = x - centerX;
        float y = a * relativeX * relativeX + b;

        // Проверяем, чтобы y был в пределах экрана
        if (y >= 0 && y <= 600) {
            approximationLine.append(sf::Vertex(sf::Vector2f(x, y), sf::Color::Green));
        }
    }

    // Отладочный вывод
    std::cout << "a: " << a << ", b: " << b << ", centerX: " << centerX << ", halfWidth: " << halfWidth << std::endl;
    std::cout << "ApproximationLine size: " << approximationLine.getVertexCount() << std::endl;
}
void PointManager::calculateLogarithmicApproximation() {
    if (points.size() < 2) return; // аппроксимация невозможна для менее чем двух точек

    float sumLnX = 0, sumY = 0, sumLnX2 = 0, sumLnXY = 0;

    for (const auto& p : points) {
        if (p.x <= 0) continue; // логарифм только для положительных x
        float lnX = std::log(p.x);
        sumLnX += lnX;
        sumY += p.y;
        sumLnX2 += lnX * lnX;
        sumLnXY += lnX * p.y;
    }

    float n = points.size();
    float denom = n * sumLnX2 - sumLnX * sumLnX;

    if (denom == 0) return; // вырожденный случай

    a = (n * sumLnXY - sumLnX * sumY) / denom;
    b = (sumY * sumLnX2 - sumLnX * sumLnXY) / denom;

    approximationLine.clear();
    for (int x = 1; x <= 800; ++x) {
        float y = a * std::log(x) + b;
        approximationLine.append(sf::Vertex(sf::Vector2f(x, y), sf::Color::Green));
    }
}

sf::Color PointManager::blendColors(const sf::Color& c1, const sf::Color& c2, float blendFactor) {
    return sf::Color(
            static_cast<sf::Uint8>(c1.r + blendFactor * (c2.r - c1.r)),
            static_cast<sf::Uint8>(c1.g + blendFactor * (c2.g - c1.g)),
            static_cast<sf::Uint8>(c1.b + blendFactor * (c2.b - c1.b)),
            static_cast<sf::Uint8>(c1.a + blendFactor * (c2.a - c1.a))
    );
}

void PointManager::drawColorDivision(sf::RenderWindow& window, sf::Time elapsed) {
    if (points.empty()) {
        // Плавное увеличение или уменьшение blend-фактора
        colorBlend += blendDirection * elapsed.asSeconds() * 0.5f; // Скорость переливания
        if (colorBlend > 1.0f) {
            colorBlend = 1.0f;
            blendDirection = -1.0f; // Меняем направление
        } else if (colorBlend < 0.0f) {
            colorBlend = 0.0f;
            blendDirection = 1.0f; // Меняем направление
        }

        sf::Color blendedColor = blendColors(color1, color2, colorBlend);

        // Закрашиваем экран смешанным цветом
        sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
        background.setFillColor(blendedColor);
        window.draw(background);
        return;
    }

    // Если есть точки, используем разделение экрана
    switch (currentType) {
        case ApproximationType::Linear:
            drawLinearDivision(window);
            break;
        case ApproximationType::Parabolic:
            drawParabolicDivision(window);
            break;
        case ApproximationType::Logarithmic:
            drawLogarithmicDivision(window);
            break;
    }
}

void PointManager::drawLinearDivision(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::VertexArray upperArea(sf::TrianglesStrip);
    sf::VertexArray lowerArea(sf::TrianglesStrip);

    for (unsigned int x = 0; x <= windowSize.x; ++x) {
        float lineY = a * x + b;
        upperArea.append(sf::Vertex(sf::Vector2f(x, 0), color1));
        upperArea.append(sf::Vertex(sf::Vector2f(x, std::min(lineY, static_cast<float>(windowSize.y))), color1));

        lowerArea.append(sf::Vertex(sf::Vector2f(x, std::max(lineY, 0.0f)), color2));
        lowerArea.append(sf::Vertex(sf::Vector2f(x, windowSize.y), color2));
    }

    window.draw(upperArea);
    window.draw(lowerArea);
}

void PointManager::drawParabolicDivision(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();

    sf::VertexArray upperArea(sf::TrianglesStrip);
    sf::VertexArray lowerArea(sf::TrianglesStrip);

    // Определяем границы параболы
    float x1 = points[0].x;
    float y1 = points[0].y;
    float x2 = points[1].x;
    float y2 = points[1].y;

    float centerX = (x1 + x2) / 2.0f;
    float halfWidth = std::abs(x2 - x1) / 2.0f;
    float height = std::abs(y2 - y1);
    float a = -4.0f * height / (halfWidth * halfWidth);
    float b = std::min(y1, y2);

    for (unsigned int x = 0; x <= windowSize.x; ++x) {
        float relativeX = x - centerX;
        float lineY = a * relativeX * relativeX + b;

        if (lineY < 0) lineY = 0;
        if (lineY > windowSize.y) lineY = static_cast<float>(windowSize.y);

        // Верхняя часть
        upperArea.append(sf::Vertex(sf::Vector2f(x, 0), color1));
        upperArea.append(sf::Vertex(sf::Vector2f(x, lineY), color1));

        // Нижняя часть
        lowerArea.append(sf::Vertex(sf::Vector2f(x, lineY), color2));
        lowerArea.append(sf::Vertex(sf::Vector2f(x, windowSize.y), color2));
    }

    window.draw(upperArea);
    window.draw(lowerArea);
}void PointManager::drawLogarithmicDivision(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::VertexArray upperArea(sf::TrianglesStrip);
    sf::VertexArray lowerArea(sf::TrianglesStrip);

    for (unsigned int x = 1; x <= windowSize.x; ++x) {
        float lineY = a * std::log(x) + b;
        upperArea.append(sf::Vertex(sf::Vector2f(x, 0), color1));
        upperArea.append(sf::Vertex(sf::Vector2f(x, std::min(lineY, static_cast<float>(windowSize.y))), color1));

        lowerArea.append(sf::Vertex(sf::Vector2f(x, std::max(lineY, 0.0f)), color2));
        lowerArea.append(sf::Vertex(sf::Vector2f(x, windowSize.y), color2));
    }

    window.draw(upperArea);
    window.draw(lowerArea);
}