#include "ImageTile.h"

void ImageTile::setTexture(sf::Texture* _texture)
{
	isEmpty = false;
	m_image.setTexture(_texture, true);
}
