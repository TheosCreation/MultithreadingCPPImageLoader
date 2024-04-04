#include "ImageGrid.h"
#include <iostream>

ImageGrid::ImageGrid(int imageSize)
{
    m_imageSize = imageSize;
}

ImageGrid::~ImageGrid()
{
}


void ImageGrid::addTile()
{
    // Calculate grid indices for the new tile
    int tileCount = (int)m_images.size();
    int sideLength = (int)sqrt(tileCount + 1); // Calculate the side length of the grid

    // Calculate row and column indices
    int rowIndex = tileCount / sideLength;
    int columnIndex = tileCount % sideLength;

    // Calculate position based on grid indices
    int xPosition = columnIndex * m_imageSize;
    int yPosition = rowIndex * m_imageSize;

	// Add a new row to the vector
	sf::RectangleShape newImage;
	newImage.setTexture(&m_imageTextures.back());
	newImage.setSize(sf::Vector2f(m_imageSize, m_imageSize));
    newImage.setPosition(sf::Vector2f(xPosition, yPosition));
	m_images.push_back(newImage);
}

bool ImageGrid::addTexture(std::string filePath)
{
    m_imageTextures.emplace_back();
    if (m_imageTextures.back().loadFromFile(filePath))
    {
        addTile();
        return true;
    }
    return false;
}

void ImageGrid::Draw(sf::RenderWindow& window)
{
    for (auto& image : m_images) {
        window.draw(image);
    }
}
