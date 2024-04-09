#include "Text.h"

Text::Text(const std::string& fontFile, unsigned int fontSize, const sf::Color& color, const std::string& content, float x, float y)
    : mPosition(x, y) {
    mFont.loadFromFile(fontFile);
    mText.setFont(mFont);
    mText.setCharacterSize(fontSize);
    mText.setFillColor(color);
    mText.setString(content);
    mText.setPosition(x, y);
}

void Text::draw(sf::RenderWindow& window) {
    window.draw(mText);
}

void Text::setPosition(float x, float y) {
    mPosition.x = x;
    mPosition.y = y;
    mText.setPosition(x, y);
}

void Text::setContent(const std::string& content) {
    mText.setString(content);
}