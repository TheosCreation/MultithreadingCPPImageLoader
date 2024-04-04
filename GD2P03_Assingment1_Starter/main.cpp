#include <iostream>
#include <vector>
#include <condition_variable>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "ImageGrid.h"
#include "Downloader.h"

#include <future>

std::chrono::steady_clock::time_point startTime;

void downloadImageToFile(std::string url, CDownloader& downloader) {
    std::string filePath = "Images/" + url.substr(url.find_last_of('/') + 1);

    // Download Image to a file
    if (downloader.DownloadToFile(url.c_str(), filePath.c_str()))
    {
        std::cout << "image download success: " << url << std::endl;
        return;
    }

    // Failed to download or load the file
    std::cerr << "Failed to download image: " << url << std::endl;
}

//void LoadImageOnGrid(std::string url, ImageGrid* imageGrid) {
//    std::string filePath = "Images/" + url.substr(url.find_last_of('/') + 1);
//    std::ifstream file(filePath);
//    if (file.good()) {
//        if (imageGrid->setTileTexture()) {
//            std::cout << "Loaded from file: " << filePath << std::endl;
//            return;
//        }
//        if (!imageTextures[i]->loadFromFile(filePath)) {
//            std::cout << "Failed to load image: " << filePaths[i] << std::endl;
//        }
//        else {
//            grid.setTileTextures(&imageTextures[i]);
//        }
//    }
//
//    // Failed to download or load the file
//    std::cerr << "Failed to load Image: " << url << std::endl;
//}

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
    sf::RenderWindow window(sf::VideoMode(800, 600), "GD2P03 Assignment 1");


    std::string data;
    CDownloader downloader;
    downloader.Init();

    if (!downloader.Download("https://raw.githubusercontent.com/MDS-HugoA/TechLev/main/ImgListSmall.txt", data)) {
        std::cerr << "Data failed to download";
        return -1;
    }

    std::vector<std::string> urls = splitUrls(data);

    startTime = std::chrono::steady_clock::now();
    
    std::vector<std::future<void>> futures;
    for (const auto& url : urls) {
        futures.push_back(std::async(std::launch::async, downloadImageToFile, url, std::ref(downloader)));
    }
    for (auto& future : futures) {
        future.wait();
    }
    auto endTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    std::cout << "Total time taken to download images: " << elapsedTime << " milliseconds" << std::endl;
    
    startTime = std::chrono::steady_clock::now();
    
    int imageCount = urls.size();

    ImageGrid imagegrid(150, imageCount);
    std::vector<sf::Texture> imageTextures;

    for (int i = 0; i < imageCount; i++) {
        imageTextures.emplace_back();
    }

    //splits all the filepaths
    std::vector<std::string> filePaths;
    for (const auto& url : urls) {
        std::string filePath = "Images/" + url.substr(url.find_last_of('/') + 1);
        filePaths.push_back(filePath);
    }

    std::vector<std::future<void>> loadFutures;
    for (int i = 0; i < imageCount; i++) {
        std::cout << "img: " << filePaths[i] << std::endl;
        loadFutures.push_back(std::async(std::launch::async, [&, i]() {
            if (!imageTextures[i].loadFromFile(filePaths[i])) {
                std::cout << "Failed to load image: " << filePaths[i] << std::endl;
            }
            else {
                imagegrid.setTileTexture(&imageTextures[i]);
            }
            }));
    }

    //for (auto& future : loadFutures) {
    //    future.get();
    //}
    //for (auto& future : futures) {
    //    future.wait();
    //}
    
    endTime = std::chrono::steady_clock::now();
    elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    std::cout << "Total time taken to download and load images: " << elapsedTime << " milliseconds" << std::endl;

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
