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
	void Draw(sf::RenderWindow& window);
private:
	std::vector<ImageTile> m_tiles;
	int m_currentIndex = 0;
	int m_imageSize = 300;
	int m_gridSize = 3;


	sf::Texture m_emptyTexure;
};

