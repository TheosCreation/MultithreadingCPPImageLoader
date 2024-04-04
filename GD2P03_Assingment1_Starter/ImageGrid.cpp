#include "ImageGrid.h"
#include <iostream>

ImageGrid::ImageGrid(int imageSize, int gridSize)
{
    m_emptyTexure.loadFromFile("emptyTile.png");
    m_imageSize = imageSize;
    m_gridSize = gridSize;
	m_tiles.resize(m_gridSize);
    for (int i = 0; i < m_gridSize; i++) {

        for (int j = 0; j < m_gridSize; j++) {

        }
    }
    for (auto& tile : m_tiles) {
        tile.m_image.setTexture(&m_emptyTexure);
        tile.m_image.setPosition(m_imageSize * 1, m_imageSize * 1);
        tile.m_image.setSize(sf::Vector2f(m_imageSize, m_imageSize));
    }
}

ImageGrid::~ImageGrid()
{
}

void ImageGrid::setTileTexture(sf::Texture* _texture)
{
    m_tiles[m_currentIndex].setTexture(_texture);
    m_currentIndex++;
}

void ImageGrid::Draw(sf::RenderWindow& window)
{
    for (auto& tile : m_tiles) {
        window.draw(tile.m_image);
    }
}
