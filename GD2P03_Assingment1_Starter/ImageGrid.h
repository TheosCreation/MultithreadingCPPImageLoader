#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

class ImageGrid
{
public:
	ImageGrid(int imageSize);
	~ImageGrid();
	bool addTile(std::string filePath);
	bool addTexture(std::string filePath);
	void Draw(sf::RenderWindow& window);
private:
	std::vector<sf::RectangleShape> m_images;
	std::vector<sf::Texture> m_imageTextures;
	int m_imageSize = 300;
	int m_index = 0;
	int m_gridColumns = 3;
};

