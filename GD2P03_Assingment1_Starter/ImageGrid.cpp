#include "ImageGrid.h"
#include <iostream>

ImageGrid::ImageGrid(int imageSize, int gridSize)
{
    m_emptyTexure.loadFromFile("LoadingTile.png");
    m_imageSize = imageSize;
    m_gridSize = gridSize;

    for (int i = 0; i < m_gridSize; i++) {
        for (int j = 0; j < m_gridSize; j++) {
            m_tiles.emplace_back();
            int index = i * m_gridSize + j;
            // Set properties for tile
            m_tiles[index].m_image.setTexture(&m_emptyTexure);
            m_tiles[index].m_image.setPosition(sf::Vector2f(j * m_imageSize, i * m_imageSize));
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

void ImageGrid::scaleImages(float newScale, int zoomAmount, int page)
{
    m_gridSize = zoomAmount;
    m_imageSize = newScale;
    int pageOffset = (m_gridSize * m_gridSize) * page;
    for (int i = 0; i < m_gridSize; i++) {
        for (int j = 0; j < m_gridSize; j++) {
            int index = (i * m_gridSize + j) + pageOffset;
            m_tiles[index].m_image.setPosition(sf::Vector2f(j * m_imageSize, i * m_imageSize));
            m_tiles[index].m_image.setSize(sf::Vector2f(m_imageSize, m_imageSize));
        }
    }
}

void ImageGrid::draw(sf::RenderWindow& window, int page)
{
    int pageOffset = (m_gridSize * m_gridSize) * page;
    for (int i = 0; i < m_gridSize; i++) {
        for (int j = 0; j < m_gridSize; j++) {
            int index = i * m_gridSize + j + pageOffset;
            window.draw(m_tiles[index].m_image);
        }
    }
}
