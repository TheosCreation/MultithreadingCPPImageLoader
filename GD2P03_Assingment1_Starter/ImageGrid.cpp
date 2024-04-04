#include "ImageGrid.h"
#include <iostream>

ImageGrid::ImageGrid(int imageSize)
{
    m_imageSize = imageSize;
}

ImageGrid::~ImageGrid()
{
}


bool ImageGrid::addTile(std::string filePath)
{
    if (addTexture(filePath))
    {
        sf::RectangleShape newImage;
        newImage.setTexture(&m_imageTextures.back()); 
        newImage.setSize(sf::Vector2f(m_imageSize, m_imageSize));

        // Calculate grid position based on index
        int row = m_index / m_gridColumns;
        int col = m_index % m_gridColumns;

        // Set position based on grid
        float xPos = col * m_imageSize;
        float yPos = row * m_imageSize;
        newImage.setPosition(sf::Vector2f(xPos, yPos));

        // Add new tile to the grid
        m_images.push_back(newImage);
        m_index++;
        return true;
    }
    return false;
}

bool ImageGrid::addTexture(std::string filePath)
{
    // Add a new texture to the end of the vector and try to load it
    sf::Texture newTexture;
    if (newTexture.loadFromFile(filePath))
    {
        m_imageTextures.push_back(newTexture);
        return true;
    }
    // If loading the texture fails, return false
    return false;
}

void ImageGrid::Draw(sf::RenderWindow& window)
{
    for (auto& image : m_images) {
        window.draw(image);
    }
}
