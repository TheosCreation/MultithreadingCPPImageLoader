#include <iostream>
#include <vector>
#include <condition_variable>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "Downloader.h"
#include "CThreadPool.h"

std::chrono::steady_clock::time_point startTime;

void downloadAndLoadImage(const std::string& url, sf::Texture& texture, CDownloader& downloader) {
    std::string filePath = "Images/" + url.substr(url.find_last_of('/') + 1);
    std::ifstream file(filePath);
    
    if (file.good()) {
        if (texture.loadFromFile(filePath)) {
            std::cout << "Loaded from file: " << filePath << std::endl;
            return;
        }
    }
    // Failed to load the file will download instead
    if (downloader.DownloadToFile(url.c_str(), filePath.c_str())) {
        if (texture.loadFromFile(filePath)) {
            std::cout << "Downloaded and loaded: " << url << std::endl;
            return;
        }
    }

    // Failed to download or load the file
    std::cerr << "Failed to download image: " << url << std::endl;
}

void screenshot(const std::string& fileSaveLocation, sf::Window* window) {
    sf::Texture texture;
    texture.create(window->getSize().x, window->getSize().y);
    texture.update(*window);
    if (texture.copyToImage().saveToFile(fileSaveLocation)) {
        std::cout << "Screenshot saved to " << fileSaveLocation << std::endl;
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "GD2P03 Assignment 1");

    std::string data;
    CDownloader downloader;
    downloader.Init();

    if (!downloader.Download("https://raw.githubusercontent.com/MDS-HugoA/TechLev/main/ImgListSmall.txt", data)) {
        std::cerr << "Data failed to download";
        return -1;
    }

    std::vector<std::string> urls;
    int oldPos = 0;
    while (oldPos < data.length()) {
        int pos = data.find('\n', oldPos);
        urls.emplace_back(data.substr(oldPos, pos - oldPos));
        oldPos = pos + 1;
    }

    startTime = std::chrono::steady_clock::now();

    std::vector<sf::Texture> textures(urls.size());

    CThreadPool pool(std::thread::hardware_concurrency());

    for (size_t i = 0; i < urls.size(); ++i) {
        pool.enqueue([i, &urls, &textures, &downloader] {
            downloadAndLoadImage(urls[i], textures[i], downloader);
            });
    }

    // Wait for all tasks to complete
    pool.getFuture().get();

    auto endTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    std::cout << "Total time taken to load images: " << elapsedTime << " milliseconds" << std::endl;


    sf::RectangleShape imageArray[9];
    int count = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            imageArray[count].setTexture(&textures[count], false);
            imageArray[count].setSize(sf::Vector2f(200, 200));
            imageArray[count].setPosition(200 * i, 200 * j);
            count++;
        }
    }

    while (window.isOpen()) {
        sf::Event winEvent;
        while (window.pollEvent(winEvent)) {
            if (winEvent.type == sf::Event::Closed) {
                screenshot("Images/combinedImage.png", &window);
                window.close();
            }
        }

        window.clear();
        for (const auto& images : imageArray)
        {
            window.draw(images);
        }
        window.display();
    }

    return 0;
}
