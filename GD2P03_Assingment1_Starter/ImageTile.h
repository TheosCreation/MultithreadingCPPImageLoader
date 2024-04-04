#pragma once
#include <SFML/Graphics.hpp>

class ImageTile
{
public:
	sf::RectangleShape m_image;
	sf::Texture m_texture;
	void setTexture(sf::Texture* _texture);
	bool isEmpty = true;
};

