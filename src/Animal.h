//
// Created by GP on 4/11/24.
//

#ifndef ANIMAL_H
#define ANIMAL_H
#include<string>
#include <SFML/Graphics.hpp>
#include <thread>
#include "GlobalVariables.h"


using std::string, std::thread, std::vector, std::unique_ptr, std::mutex, std::atomic, std::condition_variable, std::pair;
using sf::Texture, sf::RenderWindow, sf::Sprite;
using MapParameters::Cell, MapParameters::MAP_WIDTH, MapParameters::MAP_HEIGHT, MapParameters::TILE_SIZE, MapParameters::map;

/*namespace std {
    struct pair_hash {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            auto hash1 = std::hash<T1>()(p.first);
            auto hash2 = std::hash<T2>()(p.second);
            return hash1 ^ (hash2 << 1); // Combining the two hash values
        }
    };
}*/

typedef pair<int, int> position;

class Animal {
private:
    int id;
    string name;
    creatureType type{CARNIVORE};
    atomic<bool> isAlive = false;
    Texture texture;
    float positionX = 0;
    float positionY = 0;

    Sprite sprite;
    thread oldDeathThread;

    int energy = 100;

    thread energyThread;
    mutable mutex positionMutex;

    void oldDeath();

    mutex deathMutex;
    condition_variable deathCondition;

    int hungry = 0;

    thread hungryThread;

    void manageHungry();


    double heuristicDistance(position start, position final);

    vector<position> aStar(position start, position final);

    vector<position> currentPath;
    mutex pathMutex;

public:
    Animal(string name, creatureType type, const Texture& texture, float positionX, float positionY, float scale);
    ~Animal();
    void movement();
    inline static int animalNumber = 0;
    inline static int carnivoreNumber = 0;
    inline static int herbivoreNumber = 0;
    void draw(RenderWindow& window) const;
    bool getIsAlive() const;
    int getId() const;
    void setIsAlive(bool isAlive);

};

inline vector<unique_ptr<Animal>> animals;
inline thread reproduceThread;
extern void reproduce(const vector<animalStructure>& animalsReproduce);

#endif //ANIMAL_H
