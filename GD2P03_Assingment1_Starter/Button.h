/***
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2023 Media Design School
File Name : Button.h
Description : creates a sfml button with a function
Author : Theo Morris
Mail : theo.morris@mds.ac.nz
**/

#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class Button {
public:
    Button(const std::string& fontFile, const sf::Color& buttonColor, float x, float y, float width, float height, const std::string& text, const std::function<void()>& onClick);

    void draw(sf::RenderWindow& window);
    void handleEvent(sf::Event event);

private:
    sf::RectangleShape mButtonShape;
    sf::Text mText;
    sf::Font mFont;
    std::function<void()> mOnClick;
};