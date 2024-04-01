#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

class ImageGrid
{
public:
	ImageGrid(int imageSize);
	~ImageGrid();
	void addTile(const sf::Texture& imageTexture);
	void RepositionTiles(int gridSize);
	void Draw(sf::RenderWindow& window);
private:
	std::vector<sf::RectangleShape> m_images;
	int m_imageSize = 200;
};

