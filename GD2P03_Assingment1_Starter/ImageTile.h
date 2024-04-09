/***
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2023 Media Design School
File Name : ImageTile.h
Description : tile container for a rectangle image
Author : Theo Morris
Mail : theo.morris@mds.ac.nz
**/

#pragma once
#include <SFML/Graphics.hpp>

class ImageTile
{
public:
	sf::RectangleShape m_image;
	void setTexture(sf::Texture* _texture);
	bool isEmpty = true;
};