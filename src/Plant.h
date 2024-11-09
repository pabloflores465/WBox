#ifndef PLANT_H
#define PLANT_H

#include <thread>
#include <SFML/Graphics.hpp>

using sf::Sprite, sf::RenderWindow, sf::Vector2f, std::vector, std::unique_ptr, sf::FloatRect;
using std::mutex, std::thread;

class Plant {
public:
    Plant(float x, float y);
    ~Plant();
    void startLifeCycle();
    void draw(RenderWindow& window);
    bool checkCollision(const FloatRect& otherBounds);

    FloatRect getBounds() const;


private:
    Vector2f position;
    Sprite sprite;
    thread lifeCycleThread;
    bool isAlive;
    mutex spriteMutex;

    void lifeCycle() const;
};

inline vector<unique_ptr<Plant>> plants;

#endif //PLANT_H
