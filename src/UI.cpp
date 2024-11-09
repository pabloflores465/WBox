//
// Created by GP on 27/10/24.
//

#include "UI.h"
#include "Animal.h"
#include "Plant.h"
#include "Obstacle.h"
#include "GlobalVariables.h"
#include <SFML/Graphics.hpp>
#include <iostream>

using sf::RenderWindow, sf::Event, sf::View, sf::FloatRect, sf::Font, sf::Text, sf::Color, sf::Sprite, sf::Texture, sf::Vector2u;
using std::to_string, std::cerr, std::endl, std::cout;

void handleUserInput(RenderWindow& window) {
    Event event{};
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {

            for (size_t i = 0; i < animals.size();) {
                animals[i]->setIsAlive(false);
            }
            animals.clear();
            obstacles.clear();
            plants.clear();
            window.close();

        }
        else if (event.type == Event::Resized) {
            window.setView(View(FloatRect(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::P) {
                screen = 4;
            }
            else if (event.key.code == sf::Keyboard::C) {
                screen = 2;
            }else if (event.key.code == sf::Keyboard::Enter) {
                screen = 1;
            } else if (event.key.code == sf::Keyboard::S) {
                screen = 3;
            } else if (screen == 4 and (event.key.code == sf::Keyboard::Enter or event.key.code == sf::Keyboard::P)) {
                screen = 1;
            }
        }
    }
}

void start(RenderWindow& window) {
    Texture texture;
    if (!texture.loadFromFile("../public/buttons/button.png")) {
        cout << "Error al cargar el button" << endl;
    }

    Sprite sprite;
    sprite.setTexture(texture);

    // Obtenemos los límites del sprite
    FloatRect spriteBounds = sprite.getGlobalBounds();

    // Factor de escala
    sprite.setScale(0.2f, 0.2f);  // Ajusta el factor de escala como desees

    const Vector2u windowSize = window.getSize();

    // Centramos el sprite en la ventana
    sprite.setPosition(
        static_cast<float>(windowSize.x) / 2 - spriteBounds.width * 0.2f / 2,
        static_cast<float>(windowSize.y) / 2 - spriteBounds.height * 0.2f / 2
    );

    Texture background;

    if (!background.loadFromFile("../public/start_menu/start.jpeg")) {
        cout << "Error al cargar el button" << endl;
    }

    Sprite bg;

    bg.setTexture(background);
    bg.setScale(0.8f, 0.8f);
    bg.setPosition(0,0);

    window.draw(bg);
    window.draw(sprite);
}

void renderUI(RenderWindow& window) {
    static bool fontLoaded = false;
    if (!fontLoaded) {
        if (!font.loadFromFile("../public/fonts/PressStart2P-Regular.ttf")) {
            cerr << "Error loading font." << endl;
        }
        fontLoaded = true;
    }

    Text creatureCountText;
    creatureCountText.setFont(font);
    creatureCountText.setString("Plants: " + to_string(numberPlants) + "\nObstacles: " + to_string(numberObstacles) + "\nCarnivores: " + to_string(Animal::carnivoreNumber) + "\nHerbivores: " + to_string(Animal::herbivoreNumber));
    creatureCountText.setCharacterSize(24);
    creatureCountText.setFillColor(Color::White);
    creatureCountText.setPosition(10, 10);
    window.draw(creatureCountText);
}

void credits(RenderWindow& window) {
    static bool fontLoaded = false;
    if (!fontLoaded) {
        if (!font.loadFromFile("../public/fonts/PressStart2P-Regular.ttf")) {
            cerr << "Error loading font." << endl;
        }
        fontLoaded = true;
    }

    Text creatureCountText;
    creatureCountText.setFont(font);
    creatureCountText.setString("Made By \nPablo Flores\nNohel Estrada");
    creatureCountText.setCharacterSize(24);
    creatureCountText.setFillColor(Color::White);
    creatureCountText.setPosition(static_cast<float>(window.getSize().x)/2, static_cast<float>(window.getSize().y)/2);
    window.draw(creatureCountText);
}

void settings(RenderWindow& window) {
    static bool fontLoaded = false;
    if (!fontLoaded) {
        if (!font.loadFromFile("../public/fonts/PressStart2P-Regular.ttf")) {
            cerr << "Error loading font." << endl;
        }
        fontLoaded = true;
    }

    Text settingsText;
    settingsText.setFont(font);
    settingsText.setString("Settings: ");
    settingsText.setCharacterSize(24);
    settingsText.setPosition(static_cast<float>(window.getSize().x)/2, static_cast<float>(window.getSize().y)/2);
    window.draw(settingsText);

}

void paused(RenderWindow& window) {
    static bool fontLoaded = false;
    if (!fontLoaded) {
        if (!font.loadFromFile("../public/fonts/PressStart2P-Regular.ttf")) {
            cerr << "Error loading font." << endl;
        }
        fontLoaded = true;
    }

    Text pausedText;
    pausedText.setFont(font);
    pausedText.setString("Paused");
    pausedText.setCharacterSize(24);
    pausedText.setPosition(static_cast<float>(window.getSize().x)/2, static_cast<float>(window.getSize().y)/2);
    window.draw(pausedText);

}
