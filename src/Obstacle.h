#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <thread>
#include <SFML/Graphics.hpp>

using sf::RenderWindow, sf::Vector2f, sf::Sprite, sf::FloatRect;
using std::vector, std::unique_ptr, std::mutex;

class Obstacle {
public:
    Obstacle(float x, float y);
    void draw(RenderWindow& window);
    bool checkCollision(const FloatRect& otherBounds);
    FloatRect getBounds() const;

private:
    Vector2f position;
    Sprite sprite;
    mutex spriteMutex;
};

inline vector<unique_ptr<Obstacle>> obstacles;

#endif //OBSTACLE_H
