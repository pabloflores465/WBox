#include "Obstacle.h"
#include <iostream>

using sf::Texture, sf::RenderWindow, sf::FloatRect;
using std::lock_guard, std::cerr, std::endl, std::cout;

Obstacle::Obstacle(float x, float y) : position(x, y) {
    static Texture obstacleTexture;
    static bool textureLoaded = false;
    if (!textureLoaded) {
        if (!obstacleTexture.loadFromFile("../public/obstacles/rock.png")) {
            cerr << "Error loading obstacle texture." << endl;
        }
        textureLoaded = true;
    }
    sprite.setTexture(obstacleTexture);
    sprite.setPosition(position);
    sprite.setScale(25.0f / static_cast<float>(obstacleTexture.getSize().x), 25.0f / static_cast<float>(obstacleTexture.getSize().y));
}

void Obstacle::draw(RenderWindow& window) {
    lock_guard lock(spriteMutex);
    window.draw(sprite);
}

bool Obstacle::checkCollision(const FloatRect& otherBounds) {
    lock_guard lock(spriteMutex);
    const bool collision = sprite.getGlobalBounds().intersects(otherBounds);
    cout << "collision detected" << endl;
    return collision;
}

FloatRect Obstacle::getBounds() const {
    return sprite.getGlobalBounds();
}
