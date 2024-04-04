#include "ImageGrid.h"
#include <iostream>

ImageGrid::ImageGrid(int imageSize, int gridSize)
{
    m_emptyTexure.loadFromFile("emptyTile.png");
    m_imageSize = imageSize;
    m_gridSize = gridSize;

    for (int i = 0; i < m_gridSize; i++) {
        for (int j = 0; j < m_gridSize; j++) {
            m_tiles.emplace_back();
            int index = i * m_gridSize + j; // Calculate the 1D index from 2D indices
            std::cout << "index: " << index << " x: " << j << " y: " << i << std::endl;
            // Set properties for tile
            m_tiles[index].m_image.setTexture(&m_emptyTexure);
            m_tiles[index].m_image.setPosition(sf::Vector2f(j * m_imageSize, i * m_imageSize)); // Set position
            m_tiles[index].m_image.setSize(sf::Vector2f(m_imageSize, m_imageSize));
        }
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
