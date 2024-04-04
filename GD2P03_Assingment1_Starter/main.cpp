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

void downloadImageToFile(std::string url, CDownloader& downloader) {
    std::string filePath = "Images/" + url.substr(url.find_last_of('/') + 1);
    std::ifstream file(filePath);
    if (file.good()) {
        return;
    }
    // Download Image to a file
    if (downloader.DownloadToFile(url.c_str(), filePath.c_str()))
    {
        std::cout << "image download success: " << url << std::endl;
        return;
    }

    // Failed to download or load the file
    std::cerr << "Failed to download image: " << url << std::endl;
}

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

void screenshot(const std::string& fileSaveLocation, sf::Window* window) {
    sf::Texture texture;
    texture.create(window->getSize().x, window->getSize().y);
    texture.update(*window);
    if (texture.copyToImage().saveToFile(fileSaveLocation)) {
        std::cout << "Screenshot saved to " << fileSaveLocation << std::endl;
    }
}

int main() {
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
        gridSize++; // Increment gridSize if it's not a perfect square
    }

    int imageSize = windowSize / gridSize;

    ImageGrid imagegrid(imageSize, gridSize);
    std::vector<sf::Texture> imageTextures;
    imageTextures.resize(imageCount);

    startTime = std::chrono::steady_clock::now();

    std::vector<std::future<void>> downloadfutures;
    for (size_t i = 0; i < urls.size(); ++i) {
        downloadfutures.push_back(std::async(std::launch::async, downloadImageToFile, urls[i], std::ref(downloader)));
    }
    for (auto& future : downloadfutures) {
        future.wait();
    }

    auto endTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    std::cout << "Total time taken to download images: " << elapsedTime << " milliseconds" << std::endl;
    

    CThreadPool loaderThreadPool(std::thread::hardware_concurrency());
    for (int i = 0; i < imageCount; i++) {
        loaderThreadPool.enqueue([&, i]() {
            if (!imageTextures[i].loadFromFile(filePaths[i])) {
                std::cout << "Failed to load image: " << filePaths[i] << std::endl;
            }
            else {
                std::cout << "Image Loaded From: " << filePaths[i] << std::endl;
                imagegrid.setTileTexture(&imageTextures[i]);
            }
            });
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

        imagegrid.Draw(window);

        window.display();
    }

    return 0;
}
