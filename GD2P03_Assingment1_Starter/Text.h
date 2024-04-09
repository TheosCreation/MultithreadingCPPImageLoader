/***
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2023 Media Design School
File Name : Text.h
Description : creates text useful for ui
Author : Theo Morris
Mail : theo.morris@mds.ac.nz
**/

#pragma once

#include <SFML/Graphics.hpp>

class Text {
public:
    Text(const std::string& fontFile, unsigned int fontSize, const sf::Color& color, const std::string& content, float x, float y);

    void draw(sf::RenderWindow& window);
    void setPosition(float x, float y);
    void setContent(const std::string& content);

private:
    sf::Font mFont;
    sf::Text mText;
    sf::Vector2f mPosition;
};