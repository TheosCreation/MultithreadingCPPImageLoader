#include "Window.h"

Window::Window()
{
	m_windowPtr = std::make_shared<sf::RenderWindow>(sf::VideoMode(800, 600), "GD2P03 Assignment 1");
}

Window::~Window()
{
}

sf::Vector2i Window::getInnerSize()
{
	return sf::Vector2i();
}

sf::RenderWindow* Window::getWindow()
{
	return nullptr;
}

void Window::makeCurrentContext()
{
}

void Window::present(bool vsync)
{
}

bool Window::shouldClose()
{
	return false;
}
