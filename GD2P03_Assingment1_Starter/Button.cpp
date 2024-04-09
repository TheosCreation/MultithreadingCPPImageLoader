#include "Button.h"

Button::Button(const std::string& fontFile, const sf::Color& buttonColor, float x, float y, float width, float height, const std::string& text, const std::function<void()>& onClick)
    : mOnClick(onClick) {
    mButtonShape.setPosition(x, y);
    mButtonShape.setSize(sf::Vector2f(width, height));
    mButtonShape.setFillColor(buttonColor);

    mFont.loadFromFile(fontFile);
    mText.setFont(mFont);
    mText.setString(text);
    mText.setCharacterSize(24);
    mText.setFillColor(sf::Color::White);

    // Center text within the button
    sf::FloatRect textBounds = mText.getLocalBounds();
    mText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    mText.setPosition(x + width / 2.0f, y + height / 2.0f);
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(mButtonShape);
    window.draw(mText);
}

void Button::handleEvent(sf::Event event) {
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
        if (mButtonShape.getGlobalBounds().contains(mousePos)) {
            mOnClick();
        }
    }
}