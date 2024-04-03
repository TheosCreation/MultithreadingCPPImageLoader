#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

class ImageGrid
{
public:
	ImageGrid(int imageSize);
	~ImageGrid();
	void addTile();
	bool addTexture(std::string filePath);
	void RepositionTiles(int gridSize);
	void Draw(sf::RenderWindow& window);
private:
	std::vector<sf::RectangleShape> m_images;
	std::vector<sf::Texture> m_imageTextures;
	int m_imageSize = 200;
};

