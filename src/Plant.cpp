//
// Created by GP on 27/10/24.
//

#include "Plant.h"
#include <chrono>
#include <thread>
#include <iostream>

using std::cerr, std::cout, std::endl, std::this_thread::sleep_for, std::chrono::seconds, std::lock_guard, std::thread;
using sf::RenderWindow, sf::Texture, sf::FloatRect;

Plant::Plant(const float x, const float y) : position(x, y), isAlive(true) {
    static Texture plantTexture;
    static bool textureLoaded = false;
    if (not textureLoaded) {
        if (not plantTexture.loadFromFile("../public/plants/plant.png")) {
            cerr << "Error loading plant texture." << endl;
        }
        textureLoaded = true;
    }
    sprite.setTexture(plantTexture);
    sprite.setPosition(position);
    sprite.setScale(50.0f / static_cast<float>(plantTexture.getSize().x), 50.0f / static_cast<float>(plantTexture.getSize().y));
}

Plant::~Plant() {
    isAlive = false;
    if (lifeCycleThread.joinable()) {
        lifeCycleThread.join();
    }
}

void Plant::startLifeCycle() {
    lifeCycleThread = thread(&Plant::lifeCycle, this);
}

void Plant::lifeCycle() const {
    while (isAlive) {
        sleep_for(seconds(5));
    }
}

void Plant::draw(RenderWindow& window) {
    lock_guard lock(spriteMutex);
    window.draw(sprite);
}

bool Plant::checkCollision(const FloatRect& otherBounds) {
    lock_guard lock(spriteMutex);
    const bool collision = sprite.getGlobalBounds().intersects(otherBounds);
    cout << "collision detected" << endl;
    return collision;
}

FloatRect Plant::getBounds() const {
    return sprite.getGlobalBounds();
}
