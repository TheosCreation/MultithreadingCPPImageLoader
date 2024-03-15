#pragma once
#include <SFML/Graphics.hpp>

class Window
{
public:
	Window();
	~Window();

	sf::Vector2i getInnerSize();
	sf::RenderWindow* getWindow();
	void makeCurrentContext();
	void present(bool vsync);
	bool shouldClose();
private:
	std::shared_ptr<sf::RenderWindow> m_windowPtr;
	void* m_context = nullptr;
	sf::Vector2i m_size = sf::Vector2i(1024, 768);
};

