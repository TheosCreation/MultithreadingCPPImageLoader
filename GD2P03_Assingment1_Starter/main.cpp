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
    if (downloader.DownloadToFile(url.c_str(), filePath.c_str())) return;

    // Failed to download or load the file
    std::cerr << "Failed to download image: " << url << std::endl;
}

void LoadImageOnGrid(std::string url, ImageGrid& imageGrid) {
    std::string filePath = "Images/" + url.substr(url.find_last_of('/') + 1);
    std::ifstream file(filePath);
    if (file.good()) {
        if (imageGrid.addTexture(filePath)) {
            std::cout << "Loaded from file: " << filePath << std::endl;
            return;
        }
    }

    // Failed to download or load the file
    std::cerr << "Failed to load Image: " << url << std::endl;
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

    ImageGrid imagegrid(100);

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

    //startTime = std::chrono::steady_clock::now();

    for (const auto& url : urls) {
        //LoadImageOnGrid(url, std::ref(imagegrid));
        futures.push_back(std::async(std::launch::deferred, LoadImageOnGrid, url, std::ref(imagegrid)));
    }
    for (auto& future : futures) {
        future.wait();
    }
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
