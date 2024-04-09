/***
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2023 Media Design School
File Name : ImageGrid.h
Description : creates a grid of images
Author : Theo Morris
Mail : theo.morris@mds.ac.nz
**/

#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "ImageTile.h"

class ImageGrid
{
public:
	ImageGrid(int imageSize, int gridSize);
	~ImageGrid();
	void setTileTexture(sf::Texture* _texture);
	void updateGrid(float newScale, int zoomAmount, int page);
	void draw(sf::RenderWindow& window, int page);
private:
	std::vector<ImageTile> m_tiles;
	int m_currentIndex = 0;
	int m_imageSize = 300;
	int m_gridSize = 3;

	sf::Texture m_emptyTexure;
};

