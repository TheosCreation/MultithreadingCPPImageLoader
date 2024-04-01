#include "ImageGrid.h"

ImageGrid::ImageGrid(int imageSize)
{
    m_imageSize = imageSize;
}

ImageGrid::~ImageGrid()
{
}


void ImageGrid::addTile(const sf::Texture& imageTexture)
{
	// Add a new row to the vector
	sf::RectangleShape newImage;
	newImage.setTexture(&imageTexture);
	newImage.setSize(sf::Vector2f(m_imageSize, m_imageSize));
	m_images.push_back(newImage);
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
