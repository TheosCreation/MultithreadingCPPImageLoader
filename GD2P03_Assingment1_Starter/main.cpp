#define NOMINMAX
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Downloader.h"

std::mutex coutMutex;
std::mutex dataMutex;
std::chrono::steady_clock::time_point startTime;

void downloadAndLoadImage(const std::string& url, sf::Texture& texture, CDownloader& downloader) {
    std::string data;

    if (downloader.Download(url.c_str(), data)) {
        sf::Image image;
        if (image.loadFromMemory(data.c_str(), data.length())) {
            std::lock_guard<std::mutex> lock(dataMutex);
            texture.loadFromImage(image);
        }
        else {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cerr << "Failed to load image from memory: " << url << std::endl;
        }
    }
    else {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cerr << "Failed to download image: " << url << std::endl;
    }
}

void screenshot(const std::string& fileSaveLocation, sf::Window* window) {
    sf::Texture texture;
    texture.create(window->getSize().x, window->getSize().y);
    texture.update(*window);
    if (texture.copyToImage().saveToFile(fileSaveLocation)) {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "Screenshot saved to " << fileSaveLocation << std::endl;
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "GD2P03 Assignment 1");

    std::string data;
    //creates a downloader and initilises the global curl for the downloader, every tread has its own curl
    CDownloader downloader;
    downloader.Init();

    //downloads the list of images links
    if (!downloader.Download("https://raw.githubusercontent.com/MDS-HugoA/TechLev/main/ImgListSmall.txt", data)) {
        std::cerr << "Data failed to download";
        return -1;
    }

    //turns the images into seperate strings within a vector
    std::vector<std::string> urls;
    int oldPos = 0;
    while (oldPos < data.length()) {
        int pos = data.find('\n', oldPos);
        urls.emplace_back(data.substr(oldPos, pos - oldPos));
        oldPos = pos + 1;
    }

    std::vector<std::thread> threads;
    std::vector<sf::Texture> textures;
    textures.reserve(urls.size());

    startTime = std::chrono::steady_clock::now(); // Start the timer

    for (const auto& url : urls) {
        textures.emplace_back();
        threads.emplace_back(downloadAndLoadImage, std::cref(url), std::ref(textures.back()), std::ref(downloader));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto endTime = std::chrono::steady_clock::now(); // End the timer
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
                screenshot("combinedImage.png", &window);
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