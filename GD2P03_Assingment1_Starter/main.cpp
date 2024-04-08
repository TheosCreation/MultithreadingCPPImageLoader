#include <iostream>
#include <vector>
#include <condition_variable>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "ImageGrid.h"
#include "Downloader.h"

#include <future>
#include "CThreadPool.h"

std::chrono::steady_clock::time_point startTime;

// Function to split URLs from a string
std::vector<std::string> splitUrls(const std::string& _data) {
    std::vector<std::string> urls;
    size_t pos = 0;
    size_t oldPos = 0;
    while ((pos = _data.find('\n', oldPos)) != std::string::npos) {
        urls.push_back(_data.substr(oldPos, pos - oldPos));
        oldPos = pos + 1;
    }
    return urls;
}

// Function to take a screenshot of the current screen and save the to a file location
void screenshot(const std::string& fileSaveLocation, sf::Window* window) {
    sf::Texture texture;
    texture.create(window->getSize().x, window->getSize().y);
    texture.update(*window);
    if (texture.copyToImage().saveToFile(fileSaveLocation)) {
        std::cout << "Screenshot saved to " << fileSaveLocation << std::endl;
    }
}

int main() {
    startTime = std::chrono::steady_clock::now();

    int windowSize = 1260;
    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize), "GD2P03 Assignment 1");

    std::string data;
    CDownloader downloader;
    downloader.Init();

    if (!downloader.Download("https://raw.githubusercontent.com/MDS-HugoA/TechLev/main/ImgListLarge.txt", data)) {
        std::cerr << "Data failed to download";
        return -1;
    }

    // urls from the data downloaded and split
    std::vector<std::string> urls = splitUrls(data);
    // filepaths from the urls and split
    std::vector<std::string> filePaths;
    for (const auto& url : urls) {
        std::string filePath = "Images/" + url.substr(url.find_last_of('/') + 1);
        filePaths.push_back(filePath);
    }
    // number of images to download and load onto the grid
    int imageCount = urls.size();
    int gridSize = std::sqrt(imageCount);
    if (gridSize * gridSize < imageCount) {
        gridSize++;
    }

    ImageGrid imagegrid(windowSize / gridSize, gridSize);
    std::vector<sf::Texture> imageTextures;
    imageTextures.resize(imageCount);


    CThreadPool threadPool(std::thread::hardware_concurrency());
    std::vector<std::promise<void>> downloadPromises(imageCount);
    for (int i = 0; i < imageCount; i++) {
        downloadPromises[i] = std::promise<void>();
        threadPool.enqueue([&, i]() {
            std::string filePath = "Images/" + urls[i].substr(urls[i].find_last_of('/') + 1);
            if (downloader.DownloadToFile(urls[i].c_str(), filePath.c_str())) {
                std::cout << "image download success: " << urls[i] << std::endl;
                downloadPromises[i].set_value();
            }
            else {
                std::cerr << "Failed to download image: " << urls[i] << std::endl;
                downloadPromises[i].set_value();
            }
            });
        threadPool.enqueue([&, i]() {
            downloadPromises[i].get_future().wait();
            if (!imageTextures[i].loadFromFile(filePaths[i])) {
                std::cout << "Failed to load image: " << filePaths[i] << std::endl;
            }
            else {
                std::cout << "Image Loaded From: " << filePaths[i] << std::endl;
                imagegrid.setTileTexture(&imageTextures[i]);
            }
            });
    }

    auto endTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    std::cout << "Total Time taken to launch program: " << elapsedTime << " milliseconds" << std::endl;


    bool controlPressed = false;
    bool sPressed = false;
    bool scrolledUp = false;
    bool scrolledDown = false;
    //zooom factor is how many ros/columns are shown on screen
    int originalZoomFactor = 1;
    int maxZoomFactor = std::sqrt(imageCount);
    int zoomFactor = originalZoomFactor;

    int pageCount = 0;
    int maxPages = imageCount / (zoomFactor * zoomFactor);

    float imageSize = windowSize / zoomFactor;
    imagegrid.scaleImages(imageSize, originalZoomFactor, pageCount);

    while (window.isOpen()) {
        sf::Event winEvent;
        while (window.pollEvent(winEvent)) {
            switch (winEvent.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
                if (winEvent.mouseButton.button == sf::Mouse::Left)
                    pageCount++;
                else if (winEvent.mouseButton.button == sf::Mouse::Right)
                    pageCount--;
                break;
            case sf::Event::KeyPressed:
                if (winEvent.key.code == sf::Keyboard::Key::LControl || winEvent.key.code == sf::Keyboard::Key::RControl)
                    controlPressed = true;
                if (winEvent.key.code == sf::Keyboard::Key::S)
                    sPressed = true;
                break;
            case sf::Event::KeyReleased:
                if (winEvent.key.code == sf::Keyboard::Key::LControl || winEvent.key.code == sf::Keyboard::Key::RControl)
                    controlPressed = false;
                if (winEvent.key.code == sf::Keyboard::Key::S)
                    sPressed = false;
                break;
            case sf::Event::MouseWheelScrolled:
                if (controlPressed && winEvent.mouseWheelScroll.delta > 0)
                    scrolledUp = true;
                if (controlPressed && winEvent.mouseWheelScroll.delta < 0)
                    scrolledDown = true;
                break; 
            }
        }
        //input
        if (controlPressed && scrolledUp)
        {
            //zoom in
            zoomFactor--;
            if (zoomFactor < originalZoomFactor)
            {
                zoomFactor = originalZoomFactor;
            }
            else
            {
                imageSize = windowSize / zoomFactor;
            }
        }
        if (controlPressed && scrolledDown)
        {
            //zoom out
            zoomFactor++;
            if (zoomFactor > maxZoomFactor)
            {
                zoomFactor = maxZoomFactor;
            }
            else
            {
                imageSize = windowSize / zoomFactor;
            }
        }

        maxPages = imageCount / (zoomFactor * zoomFactor);

        if (pageCount < 0)
        {
            pageCount = 0;
        }
        if (pageCount > maxPages-1)
        {
            pageCount = maxPages-1;
        }

        imagegrid.scaleImages(imageSize, zoomFactor, pageCount);

        if (controlPressed && sPressed)
        {
            screenshot("screenshot.png", &window);
        }

        scrolledUp = false;
        scrolledDown = false;

        window.clear();

        imagegrid.draw(window, pageCount);

        window.display();
    }

    return 0;
}
