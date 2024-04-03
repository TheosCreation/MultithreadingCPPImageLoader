#include "ImageGrid.h"

ImageGrid::ImageGrid(int imageSize)
{
    m_imageSize = imageSize;
}

ImageGrid::~ImageGrid()
{
}


void ImageGrid::addTile()
{
	// Add a new row to the vector
	sf::RectangleShape newImage;
	newImage.setTexture(&m_imageTextures[0]);
	newImage.setSize(sf::Vector2f(m_imageSize, m_imageSize));
	m_images.push_back(newImage);
}

bool ImageGrid::addTexture(std::string filePath)
{
    m_imageTextures.emplace_back();
    if (m_imageTextures.back().loadFromFile(filePath))
    {
        return true;
    }
    m_imageTextures.pop_back();
    return false;
}

void ImageGrid::RepositionTiles(int gridSize)
{
    for (int i = 0; i < m_images.size(); i++)
    {
        // Calculate the row and column index for the current image
        int row = i / gridSize;
        int col = i % gridSize;

        // Calculate the position for the current image based on its row and column index
        float posX = col * m_imageSize;
        float posY = row * m_imageSize;

        // Set the position of the current image
        m_images[i].setPosition(posX, posY);
    }
}

void ImageGrid::Draw(sf::RenderWindow& window)
{
    for (auto& image : m_images) {
        window.draw(image);
    }
}
