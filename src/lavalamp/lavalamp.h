#ifndef LAVALAMP_H
#define LAVALAMP_H

#include <SFML/Graphics.hpp>
#include <vector>

class LavaLamp {
public:
    LavaLamp(float width, float height);
    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window);

    void setSize(float width, float height);

private:
    struct Particle {
        sf::CircleShape shape;
        sf::Vector2f velocity;
        float scale;
        float scaleSpeed;
    };

    std::vector<Particle> particles;
    sf::VertexArray backgroundGradient;
    float width;
    float height;

    void initParticles();

    void initBackground();

};

#endif // LAVALAMP_H