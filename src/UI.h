//
// Created by GP on 27/10/24.
//

#ifndef UI_H
#define UI_H

#include <SFML/Graphics.hpp>

using sf::RenderWindow, sf::Font;

extern void  handleUserInput(RenderWindow& window);

extern void renderUI(RenderWindow& window);

extern void credits(RenderWindow& window);

extern void start(RenderWindow& window);

extern void settings (RenderWindow& window);

extern void paused(RenderWindow& window);

inline int screen = 0;

static Font font;

#endif //UI_H
