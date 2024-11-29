#include "LavaLamp.h"
#include <cstdlib>

LavaLamp::LavaLamp(float width, float height) : width(width), height(height) {
    initParticles();
    initBackground();
}

void LavaLamp::initParticles() {
    int particleCount = 10;

    for (int i = 0; i < particleCount; ++i) {
        Particle p;
        float radius = static_cast<float>(rand() % 30 + 20); // Радиус от 20 до 50
        p.shape.setRadius(radius);
        p.shape.setOrigin(radius, radius);
        p.scale = 1.0f;
        p.scaleSpeed = static_cast<float>((rand() % 50 + 50) / 1000.0f); // От 0.05 до 0.1

        float x = static_cast<float>(rand() % static_cast<int>(width));
        float y = static_cast<float>(rand() % static_cast<int>(height));
        p.shape.setPosition(x, y);

        // Цвет частиц — оттенки синего
        sf::Color color(
                0,
                static_cast<sf::Uint8>(rand() % 26 + 230), // Зеленый компонент (230-255) для получения голубого оттенка
                255, // Синий компонент
                150  // Прозрачность
        );

        p.shape.setFillColor(color);

        // Случайная скорость
        p.velocity.x = static_cast<float>(rand() % 100 - 50); // От -50 до 50
        p.velocity.y = static_cast<float>(rand() % 100 - 50);

        particles.push_back(p);
    }
}

void LavaLamp::update(sf::Time elapsed) {
    for (auto& p : particles) {
        sf::Vector2f pos = p.shape.getPosition();
        sf::Vector2f vel = p.velocity;

        pos += vel * elapsed.asSeconds();

        // Отражение от стен
        if (pos.x - p.shape.getRadius() < 0) {
            pos.x = p.shape.getRadius();
            vel.x = -vel.x;
        }
        if (pos.x + p.shape.getRadius() > width) {
            pos.x = width - p.shape.getRadius();
            vel.x = -vel.x;
        }
        if (pos.y - p.shape.getRadius() < 0) {
            pos.y = p.shape.getRadius();
            vel.y = -vel.y;
        }
        if (pos.y + p.shape.getRadius() > height) {
            pos.y = height - p.shape.getRadius();
            vel.y = -vel.y;
        }

        // Обновление масштаба для эффекта пульсации
        p.scale += p.scaleSpeed * elapsed.asSeconds();
        if (p.scale > 1.2f || p.scale < 0.8f) {
            p.scaleSpeed = -p.scaleSpeed; // Инвертируем направление изменения масштаба
        }
        p.shape.setScale(p.scale, p.scale);

        p.shape.setPosition(pos);
        p.velocity = vel;
    }
}

void LavaLamp::draw(sf::RenderWindow& window) {
    // Рисуем градиентный фон
    window.draw(backgroundGradient);

    // Рисуем частицы
    for (const auto& p : particles) {
        window.draw(p.shape);
    }
}

void LavaLamp::initBackground() {
    backgroundGradient.setPrimitiveType(sf::Quads);
    backgroundGradient.resize(4);

    // Верхний цвет (красный)
    sf::Color topColor = sf::Color(255, 69, 0, 100);   // Оранжево-красный с прозрачностью (OrangeRed)
    // Нижний цвет (оранжевый)
    sf::Color bottomColor = sf::Color(255, 165, 0); // Оранжевый (Orange)

    // Верхний левый угол
    backgroundGradient[0].position = sf::Vector2f(0, 0);
    backgroundGradient[0].color = topColor;

    // Верхний правый угол
    backgroundGradient[1].position = sf::Vector2f(width, 0);
    backgroundGradient[1].color = topColor;

    // Нижний правый угол
    backgroundGradient[2].position = sf::Vector2f(width, height);
    backgroundGradient[2].color = bottomColor;

    // Нижний левый угол
    backgroundGradient[3].position = sf::Vector2f(0, height);
    backgroundGradient[3].color = bottomColor;
}

void LavaLamp::setSize(float width, float height) {
    this->width = width;
    this->height = height;

    // Обновляем позиции вершин градиента
    backgroundGradient[0].position = sf::Vector2f(0, 0);
    backgroundGradient[1].position = sf::Vector2f(width, 0);
    backgroundGradient[2].position = sf::Vector2f(width, height);
    backgroundGradient[3].position = sf::Vector2f(0, height);
}
