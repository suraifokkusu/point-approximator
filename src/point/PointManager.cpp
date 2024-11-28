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
    }   else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::C) {
            clearPoints();
        } else if (event.key.code == sf::Keyboard::Num1) {
            currentType = ApproximationType::Linear;
            calculateApproximation(); // Добавили вызов
        } else if (event.key.code == sf::Keyboard::Num2) {
            currentType = ApproximationType::Parabolic;
            calculateApproximation(); // Добавили вызов
        } else if (event.key.code == sf::Keyboard::Num3) {
            currentType = ApproximationType::Logarithmic;
            calculateApproximation(); // Добавили вызов
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

    // Вызываем метод, который выберет нужный тип аппроксимации
    calculateApproximation();

}

void PointManager::draw(sf::RenderWindow& window) {
    // Рисуем точки
    for (const auto& point : points) {
        window.draw(point.shape);
        window.draw(point.label);
    }

    // Рисуем линию аппроксимации, если она рассчитана
    if (approximationLine.getVertexCount() > 1) {
        window.draw(approximationLine);
    }
}

void PointManager::removePoint(float x, float y) {
    points.erase(std::remove_if(points.begin(), points.end(),
                                [x, y](const Point& p) {
                                    return std::hypot(p.x - x, p.y - y) < 10; // удаление точки в радиусе 10 пикселей
                                }), points.end());
    calculateApproximation(); // Добавляем пересчет аппроксимации
}

void PointManager::clearPoints() {
    points.clear();
    approximationLine.clear();
    calculateApproximation(); // Добавляем пересчет аппроксимации
}

void PointManager::calculateApproximation() {
    approximationLine.clear();

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

    // Обновляем линию аппроксимации
    approximationLine.clear();
    approximationLine.setPrimitiveType(sf::Lines);

    sf::Vertex startPoint, endPoint;

    startPoint.position = sf::Vector2f(0, a * 0 + b);
    endPoint.position = sf::Vector2f(windowWidth, a * windowWidth + b);

    // Устанавливаем цвет линии в зелёный
    startPoint.color = sf::Color::Green;
    endPoint.color = sf::Color::Green;

    approximationLine.append(startPoint);
    approximationLine.append(endPoint);
}

void PointManager::calculateParabolicApproximation() {
    if (points.size() < 3) {
        approximationLine.clear();
        return; // Недостаточно точек для параболической аппроксимации
    }

    // Подготовка данных
    size_t n = points.size();
    double sumX = 0, sumX2 = 0, sumX3 = 0, sumX4 = 0;
    double sumY = 0, sumXY = 0, sumX2Y = 0;

    for (const auto& p : points) {
        double x = p.x;
        double y = p.y;
        sumX += x;
        sumX2 += x * x;
        sumX3 += x * x * x;
        sumX4 += x * x * x * x;
        sumY += y;
        sumXY += x * y;
        sumX2Y += x * x * y;
    }

    // Составление матрицы и вектора
    double matrix[3][4] = {
            { sumX4, sumX3, sumX2, sumX2Y },
            { sumX3, sumX2, sumX,  sumXY  },
            { sumX2, sumX,  static_cast<double>(n),     sumY   }
    };

    // Решение системы уравнений методом Гаусса
    for (int i = 0; i < 3; ++i) {
        // Нормализация ведущего коэффициента
        double pivot = matrix[i][i];
        if (pivot == 0) {
            approximationLine.clear();
            return; // Невозможно решить систему уравнений
        }
        for (int j = i; j < 4; ++j) {
            matrix[i][j] /= pivot;
        }
        // Обнуление элементов ниже ведущего
        for (int k = i + 1; k < 3; ++k) {
            double factor = matrix[k][i];
            for (int j = i; j < 4; ++j) {
                matrix[k][j] -= factor * matrix[i][j];
            }
        }
    }
    // Обратная подстановка
    c = matrix[2][3];
    b = matrix[1][3] - matrix[1][2] * c;
    a = matrix[0][3] - matrix[0][2] * c - matrix[0][1] * b;

    // Проверка на корректность значений
    if (std::isnan(a) || std::isnan(b) || std::isnan(c)) {
        approximationLine.clear();
        return; // Некорректные коэффициенты
    }

    // Обновление линии аппроксимации
    approximationLine.clear();
    approximationLine.setPrimitiveType(sf::LinesStrip);

    float step = windowWidth / 1000.0f; // Больше точек для плавности

    for (float x = 0; x <= windowWidth; x += step) {
        float y = static_cast<float>(a * x * x + b * x + c);
        if (y >= 0 && y <= windowHeight) {
            approximationLine.append(sf::Vertex(sf::Vector2f(x, y), sf::Color::Green));
        }
    }

    // Обновление текста уравнения
    //equationText.setString("y = " + std::to_string(a) + "x^2 + " + std::to_string(b) + "x + " + std::to_string(c));
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

void PointManager::drawColorDivision(sf::RenderWindow& window, sf::Time elapsed) {
    // Обновление фактора смешивания цветов
    colorBlend += blendDirection * elapsed.asSeconds();
    if (colorBlend >= 1.0f || colorBlend <= 0.0f) {
        blendDirection *= -1.0f;
        colorBlend = clamp(colorBlend, 0.0f, 1.0f);
    }

    // Смешивание цветов
    sf::Color blendedColor1 = blendColors(color1, color2, colorBlend);
    sf::Color blendedColor2 = blendColors(color2, color1, colorBlend);

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

    float step = 1.0f; // Шаг по оси X

    for (float x = 0; x <= windowWidth; x += step) {
        float y = static_cast<float>(a * x * x + b * x + c);

        // Проверяем границы по Y
        y = clamp(y, 0.0f, windowHeight);

        // Верхняя область
        upperArea.append(sf::Vertex(sf::Vector2f(x, 0), color1));
        upperArea.append(sf::Vertex(sf::Vector2f(x, y), color1));

        // Нижняя область
        lowerArea.append(sf::Vertex(sf::Vector2f(x, y), color2));
        lowerArea.append(sf::Vertex(sf::Vector2f(x, windowHeight), color2));
    }

    window.draw(upperArea);
    window.draw(lowerArea);
}

void PointManager::drawLogarithmicDivision(sf::RenderWindow& window) {
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

void PointManager::setWindowSize(float width, float height) {
    windowHeight = height;
    windowWidth = width;
}

sf::Color PointManager::blendColors(const sf::Color& c1, const sf::Color& c2, float blendFactor) {
    return sf::Color(
            static_cast<sf::Uint8>(c1.r + blendFactor * (c2.r - c1.r)),
            static_cast<sf::Uint8>(c1.g + blendFactor * (c2.g - c1.g)),
            static_cast<sf::Uint8>(c1.b + blendFactor * (c2.b - c1.b)),
            static_cast<sf::Uint8>(c1.a + blendFactor * (c2.a - c1.a))
    );
}
