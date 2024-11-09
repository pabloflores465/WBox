//
// Created by GP on 4/11/24.
//

#include "Animal.h"
#include "Plant.h"
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <limits>

using std::this_thread::sleep_for, std::chrono::seconds, std::make_unique, std::cout, std::endl, std::lock_guard,
      std::unique_lock, std::priority_queue, std::vector, std::greater;

/*namespace std {
    template <class T1, class T2>
    struct hash<std::pair<T1, T2>> {
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            auto hash1 = std::hash<T1>{}(p.first);
            auto hash2 = std::hash<T2>{}(p.second);
            return hash1 ^ (hash2 << 1);
        }
    };
}*/

/*struct PositionHash {
    std::size_t operator()(const position& p) const noexcept {
        auto hash1 = std::hash<int>{}(p.first);
        auto hash2 = std::hash<int>{}(p.second);
        return hash1 ^ (hash2 << 1);
    }
};

struct PositionEqual {
    bool operator()(const position& lhs, const position& rhs) const noexcept {
        return lhs == rhs;
    }
};*/

Animal::Animal(string name, const creatureType type, const Texture& texture, const float positionX, const float positionY, const float scale) {
    this->id = animalNumber;
    this->name = std::move(name);
    this->type = type;
    this->isAlive = true;
    this->texture = texture;
    this->positionX = positionX;
    this->positionY = positionY;
    this->sprite.setTexture(texture);
    this->sprite.setPosition(positionX, positionY);
    this->sprite.setScale(scale, scale);
    this->energy = 100;
    this->hungry = 0;
    oldDeathThread = thread(&Animal::oldDeath, this);
    hungryThread = thread(&Animal::manageHungry, this);
    energyThread = thread(&Animal::movement, this);
    animalNumber++;
    if(type == CARNIVORE) {
        carnivoreNumber++;
    } else if (type == HERBIVORE) {
        herbivoreNumber++;
    }
    cout << "This animal: " << this->id << endl;
    cout << "Animal number: " << animalNumber << endl;
}

Animal::~Animal() {
    if (oldDeathThread.joinable()) {
        oldDeathThread.join();
        cout << "old dead finalized" << endl;
    }

    if (energyThread.joinable()) {
        energyThread.join();
        cout << "energy finalized" << endl;
    }

    if(hungryThread.joinable()) {
        hungryThread.join();
        cout << "hungry dead finalized" << endl;
    }
    if(type == CARNIVORE) {
        carnivoreNumber--;
    } else if (type == HERBIVORE) {
        herbivoreNumber--;
    }
};

void Animal::draw(RenderWindow &window) const {
    lock_guard lock(positionMutex);
    window.draw(this->sprite);
}

void Animal::oldDeath() {
    unique_lock lock(deathMutex);
    if (this->isAlive.load()) {
        if (deathCondition.wait_for(lock, seconds(120), [this]() { return !this->isAlive.load(); })) {
            return;
        }
        if (this->isAlive.exchange(false)) { // Cambia a false solo si es true
            deathCondition.notify_all();  // Notifica a otros hilos para salir
            cout << "DEAAAAAAAAD" << endl;
        }
    }
}

void reproduce(const vector<animalStructure>& animalsReproduce) {
    while(!animalsReproduce.empty()) {
        sleep_for(seconds(60));
        for (const auto& [name, type, texture, positionX, positionY, scale] : animalsReproduce) {
            animals.push_back(make_unique<Animal>(name, type, texture, positionX, positionY, scale));
        }
    }
}

void Animal::movement() {
    {
        std::lock_guard<std::mutex> posLock(positionMutex);
        // Obtener la posición actual
        position start = {
            static_cast<int>(this->positionX / TILE_SIZE),
            static_cast<int>(this->positionY / TILE_SIZE)
        };
        position final = { MAP_WIDTH / 2 - 1, MAP_HEIGHT / 2 - 1 };

        // Obtener el camino usando A*
        std::vector<position> path = aStar(start, final);

        // Almacenar el camino actual
        {
            std::lock_guard<std::mutex> pathLock(pathMutex);
            currentPath = std::move(path);
        }
    }
    std::unique_lock<std::mutex> lock(deathMutex);
    while (isAlive.load()) {
        // Esperar 1 segundo o hasta que el animal muera
        if (deathCondition.wait_for(lock, seconds(1), [this]() { return !isAlive.load(); })) {
            return;
        }

        if (energy >= 60) {


            // Mover un paso del camino
            std::lock_guard<std::mutex> pathLock(pathMutex);
            if (!currentPath.empty()) {
                auto nextPosition = currentPath.front();

                // Actualizar la posición
                this->positionX = static_cast<float>(nextPosition.first * TILE_SIZE);
                this->positionY = static_cast<float>(nextPosition.second * TILE_SIZE);
                sprite.setPosition(this->positionX, this->positionY); // Actualizar el sprite
                currentPath.erase(currentPath.begin());

                std::cout << "x: " << this->positionX << " y: " << this->positionY << std::endl;
            }

            energy--;
            std::cout << "La energía es: " << energy << std::endl;
        }
        else {
            if (energy < 90) {
                // Recargar energía
                energy++;
                std::cout << "La energía es: " << energy << std::endl;
            }
        }
    }
}

void Animal::manageHungry() {
    unique_lock lock(deathMutex);
    while(this->isAlive.load() && hungry < 60) {
        if(deathCondition.wait_for(lock, seconds(1),[this]() { return !this->isAlive.load(); })) {
            return;
        }
        hungry++;
    }
    if (this->isAlive.exchange(false)) { // Cambia a false solo si es true
        deathCondition.notify_all();  // Notifica a otros hilos para salir
        cout << "nooooooooooo" << endl;
    }
    cout<<"The hungry is: "<<hungry<<endl;
    cout<<"the animal died of hunger"<<endl;

}

/*void Animal::herbivoreMove() {

    for (const auto& plant : plants) {
        diference
        if (() && ())
    }
}*/

bool Animal::getIsAlive() const {
    return this->isAlive.load();
}

int Animal::getId() const {
    return this->id;
}
using std::cout;
void Animal::setIsAlive(const bool isAlive) {
    this->isAlive.store(isAlive);
}

double Animal::heuristicDistance(position start, position final) {
    const auto startX = start.first;
    const auto startY = start.second;
    const auto finalX = final.first;
    const auto finalY = final.second;
    return abs(startX - finalX) + abs(startY - finalY);
}

using std::unordered_map, std::hash, std::cout, std::equal_to, std::numeric_limits;
vector<position> Animal::aStar(position start, position final) {
    constexpr int rows = MAP_WIDTH;
    constexpr int cols = MAP_HEIGHT;

    // Initialize the grid: 0 represents walkable, 1 represents blocked
    // For simplicity, we'll initialize all cells as walkable. Modify as needed.
    vector<vector<int>> grid(rows, vector<int>(cols, 0));

    cout << "start (" << start.first << "," << start.second << ")" << endl;
    cout << "final (" << final.first << "," << final.second << ")" << endl;

    // Priority queue with pairs of (totalEstimatedCost, position)
    // Using a min-heap based on totalEstimatedCost
    priority_queue<pair<double, position>, vector<pair<double, position>>, std::greater<pair<double, position>>> possiblePaths;

    // Initialize 2D vectors for cameFrom, accumulatedCost, and totalEstimatedCost
    // Using -1 to indicate no previous position in cameFrom
    // Initialize accumulatedCost and totalEstimatedCost with infinity
    vector<vector<position>> cameFrom(rows, vector<position>(cols, {-1, -1}));
    vector<vector<double>> accumulatedCost(rows, vector<double>(cols, numeric_limits<double>::infinity()));
    vector<vector<double>> totalEstimatedCost(rows, vector<double>(cols, numeric_limits<double>::infinity()));

    // Initialize start position
    possiblePaths.emplace(0.0, start);
    accumulatedCost[start.first][start.second] = 0.0;
    totalEstimatedCost[start.first][start.second] = heuristicDistance(start, final);

    cout << "Accumulated cost at (" << start.first << ", " << start.second << "): "
         << accumulatedCost[start.first][start.second] << endl;

    // Directions: up, right, down, left
    vector<position> directions = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

    // To keep track of visited nodes
    vector<vector<bool>> closedSet(rows, vector<bool>(cols, false));

    while (!possiblePaths.empty()) {
        position currentPosition = possiblePaths.top().second;
        possiblePaths.pop();

        int currentX = currentPosition.first;
        int currentY = currentPosition.second;

        // Skip if already processed
        if (closedSet[currentX][currentY]) {
            continue;
        }
        closedSet[currentX][currentY] = true;

        // Check if the final position is reached
        if (currentPosition == final) {
            vector<position> path;
            position trace = final;
            while (trace != start) {
                path.push_back(trace);
                trace = cameFrom[trace.first][trace.second];
                if (trace.first == -1 && trace.second == -1) {
                    // No path found
                    return {};
                }
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            return path;
        }

        // Explore neighbors
        for (const auto& [dx, dy] : directions) {
            int neighborX = currentX + dx;
            int neighborY = currentY + dy;

            // Check boundaries
            if (neighborX < 0 || neighborX >= rows || neighborY < 0 || neighborY >= cols) {
                continue;
            }

            // Check if the cell is walkable
            if (grid[neighborX][neighborY] != 0) {
                continue;
            }

            double tentativeAccumulatedCost = accumulatedCost[currentX][currentY] + 1.0; // Assuming cost between neighbors is 1

            if (tentativeAccumulatedCost < accumulatedCost[neighborX][neighborY]) {
                cameFrom[neighborX][neighborY] = currentPosition;
                accumulatedCost[neighborX][neighborY] = tentativeAccumulatedCost;
                totalEstimatedCost[neighborX][neighborY] = tentativeAccumulatedCost + heuristicDistance({neighborX, neighborY}, final);
                possiblePaths.emplace(totalEstimatedCost[neighborX][neighborY], position{neighborX, neighborY});
            }
        }
    }

    // If no path is found
    return {};
}
/*vector<position> Animal::aStar(position start, position final) {

    constexpr int rows = MAP_WIDTH;
    constexpr int cols = MAP_HEIGHT;

    vector<vector<int>> grid;

    cout<<"start"<<"("<<start.first<<","<<start.second<<")"<<endl;
    cout<<"final"<<"("<<final.first<<","<<final.second<<")"<<endl;

    for(size_t row = 0; row < rows; row++) {
        for(size_t col = 0; col < cols; col++) {
            //const auto iterator = static_cast<vector<vector<int>>::difference_type>(row);
            grid.push_back({static_cast<int>(row), static_cast<int>(col)});
        }
    }

    //Define a priority queue that orders the queue based on a float value of priority, works as a vector
    priority_queue<pair<double, position>, vector<pair<double, position>>, greater<pair<double, position>>> possiblePaths;

    unordered_map<position, position, PositionHash, PositionEqual> cameFrom;
    unordered_map<position, double, PositionHash, PositionEqual> accumulatedCost;
    unordered_map<position, double, PositionHash, PositionEqual> totalEstimatedCost;

    possiblePaths.emplace(0.0, start);
    accumulatedCost[start] = 0.0;
    totalEstimatedCost[start] = heuristicDistance(start, final);

    std::cout << "Accumulated cost at (" << start.first << ", " << start.second << "): "
          << accumulatedCost[start] << std::endl;

    while (!possiblePaths.empty()) {
        position currentPosition = possiblePaths.top().second;
        possiblePaths.pop();

        if (currentPosition == final) {
            vector<position> path;
            while (cameFrom.find(currentPosition) != cameFrom.end()) {
                path.push_back(currentPosition);
                currentPosition = cameFrom[currentPosition];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            return path;
        }

        vector<position> neighbors = {{0,1}, {1,0}, {0,-1}, {-1,0}};

        //Defferentiating the function
        for(const auto& [dx,dy] : neighbors) {
            const auto currentX = currentPosition.first;
            const auto currentY = currentPosition.second;

            position neighbor = {currentX + dx, currentY + dy};

            const auto neighborX = neighbor.first;

            if(const auto neighborY = neighbor.second; neighborX >= 0 and neighborX < rows and neighborY >= 0 and neighborY < cols and grid[neighborX][neighborY] == 0) {
                if (const double possibleAccumulatedCost = accumulatedCost[currentPosition] + 1; accumulatedCost.find(neighbor) == accumulatedCost.end() or possibleAccumulatedCost < accumulatedCost[neighbor]) {
                    cameFrom[neighbor] = currentPosition;
                    accumulatedCost[neighbor] = possibleAccumulatedCost;
                    totalEstimatedCost[neighbor] = possibleAccumulatedCost + heuristicDistance(neighbor, final);
                    possiblePaths.emplace(totalEstimatedCost[neighbor], neighbor);
                }
            }
        }
    }

    return {};

}*/

/*using namespace std;

typedef pair<int, int> position; // Your typedef for position

struct pair_hash {
    std::size_t operator()(const position& p) const {
        // A simple but effective hash combination for pairs
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};


double Animal::heuristicDistance(const position& a, const position& b) {
    // Implement your heuristic function here (e.g., Manhattan distance)
    return abs(a.first - b.first) + abs(a.second - b.second);
}

vector<Animal::position> Animal::aStar(const position& start, const position& final) {
    constexpr int rows = MAP_WIDTH;  // Define MAP_WIDTH appropriately
    constexpr int cols = MAP_HEIGHT; // Define MAP_HEIGHT appropriately

    // Corrected grid initialization
    vector<vector<int>> grid(rows, vector<int>(cols, 0));

    cout << "start(" << start.first << "," << start.second << ")" << endl;
    cout << "final(" << final.first << "," << final.second << ")" << endl;

    // Define the priority queue
    priority_queue<pair<double, position>, vector<pair<double, position>>, greater<pair<double, position>>> possiblePaths;

    // Corrected unordered_map declarations
    unordered_map<position, position, std::pair_hash> cameFrom;
    unordered_map<position, double, std::pair_hash> accumulatedCost;
    unordered_map<position, double, std::pair_hash> totalEstimatedCost;

    possiblePaths.emplace(0.0, start);
    accumulatedCost.emplace(start, 0);
    totalEstimatedCost[start] = heuristicDistance(start, final);

    while (!possiblePaths.empty()) {
        position currentPosition = possiblePaths.top().second;
        possiblePaths.pop();

        if (currentPosition == final) {
            vector<position> path;
            while (currentPosition != start) {
                path.push_back(currentPosition);
                currentPosition = cameFrom[currentPosition];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            return path;
        }

        vector<position> directions = {{0,1}, {1,0}, {0,-1}, {-1,0}};

        for (const auto& dir : directions) {
            int newX = currentPosition.first + dir.first;
            int newY = currentPosition.second + dir.second;

            if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && grid[newX][newY] == 0) {
                position neighbor = {newX, newY};
                double newCost = accumulatedCost[currentPosition] + 1;

                if (accumulatedCost.find(neighbor) == accumulatedCost.end() || newCost < accumulatedCost[neighbor]) {
                    accumulatedCost[neighbor] = newCost;
                    double priority = newCost + heuristicDistance(neighbor, final);
                    totalEstimatedCost[neighbor] = priority;
                    possiblePaths.emplace(priority, neighbor);
                    cameFrom[neighbor] = currentPosition;
                }
            }
        }
    }

    return {}; // Return an empty path if no path is found
}*/

