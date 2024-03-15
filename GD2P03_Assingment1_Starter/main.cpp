#define NOMINMAX
#include "Downloader.h"
#include <SFML/Graphics.hpp>
#include <vector>

void screenshot(std::string fileSaveLocation, sf::Window* window)
{
	//saves the current window to an image
	sf::Texture texture;
	texture.create(window->getSize().x, window->getSize().y);
	texture.update(*window);
	if (texture.copyToImage().saveToFile(fileSaveLocation))
	{
		std::cout << "screenshot saved to " << fileSaveLocation << std::endl;
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "GD2P03 Assignment 1");

	CDownloader downloader;
	downloader.Init();

	std::string data = "";
	if (downloader.Download(
		"https://raw.githubusercontent.com/MDS-HugoA/TechLev/main/ImgListSmall.txt",
		data))
	{
		std::cout << data << "\n";
		printf("data length: %zd\n", data.length());
	}
	else
	{
		printf("data failed to download");
	}


	//split the urls
	size_t pos = 0;
	size_t oldPos = 0;
	std::vector<std::string> urls;
	while (pos != std::string::npos)
	{
		pos = data.find('\n', oldPos);
		if (oldPos < data.length())
		{
			urls.push_back(data.substr(oldPos, pos - oldPos));
			printf("url [%zd] : %s\n", urls.size() - 1, urls[urls.size() - 1].c_str());
			oldPos = pos + 1;
		}
	} 

	sf::RectangleShape imageArray[4];
	sf::Texture textArray[4];
	int count = 0;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {


			data = "";
			downloader.Download(urls[count].c_str(), data);//download the image

			if (!textArray[count].loadFromMemory(data.c_str(), data.length())) {
				return -1;
			}

			imageArray[count].setTexture(&textArray[count], false);
			imageArray[count].setSize(sf::Vector2f(200, 200));
			imageArray[count].setPosition(200 * i, 200 * j);


			count++;
		}
	}

	while (window.isOpen())
	{
		sf::Event winEvent;
		while (window.pollEvent(winEvent))
		{
			if (winEvent.type == sf::Event::Closed)
			{
				screenshot("combinedImage.png", &window);
				window.close();
			}
		}

		window.clear();
		for (int i = 0; i < 4; i++) {

			window.draw(imageArray[i]);
		}
		window.display();
	}

	curl_global_cleanup();

	return 0;
}

