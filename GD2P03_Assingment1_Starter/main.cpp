#include <iostream>
#include <fstream>
#include <vector>
#include <future>
#include <condition_variable>
#include <SFML/Graphics.hpp>
#include "ImageGrid.h"
#include "Downloader.h"
#include "CThreadPool.h"
#include <sstream>
#include "Button.h"
#include "Text.h"

std::chrono::steady_clock::time_point startTime;

// splits URLs from a string
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

// takes a screenshot of the current screen and save the to a file location
void screenshot(const std::string& fileSaveLocation, sf::Window* window) {
    sf::Texture texture;
    texture.create(window->getSize().x, window->getSize().y);
    texture.update(*window);

    // Generate unique filenames
    std::string filename;
    int i = 0;
    do {
        std::ostringstream oss;
        oss << fileSaveLocation << i << ".png";
        filename = oss.str();
        ++i;
    } while (std::ifstream(filename).good());

    // Save the screenshot to a file
    if (texture.copyToImage().saveToFile(filename)) {
        std::cout << "Screenshot saved to " << filename << std::endl;
    }
}

int main() {

    int windowSize = 1260;

    sf::RenderWindow settingsPage(sf::VideoMode(windowSize, windowSize), "Settings Page");

    Text title("VCR_OSD_MONO_1.001.ttf", 100, sf::Color::White, "Settings", 250, 50);
    Text listToLoad("VCR_OSD_MONO_1.001.ttf", 50, sf::Color::White, "List To Load", 250, 200);

    std::string listUrl;
    Button smallListButton("VCR_OSD_MONO_1.001.ttf", sf::Color::Red, 150, 300, 200, 50, "Small", [&listUrl, &settingsPage]() {
        listUrl = "https://raw.githubusercontent.com/MDS-HugoA/TechLev/main/ImgListSmall.txt";
        settingsPage.close();
        });
    Button largeListButton("VCR_OSD_MONO_1.001.ttf", sf::Color::Red, 500, 300, 200, 50, "Large", [&listUrl, &settingsPage]() {
        listUrl = "https://raw.githubusercontent.com/MDS-HugoA/TechLev/main/ImgListLarge.txt";
        settingsPage.close();
        });

    while (settingsPage.isOpen()) {
        sf::Event winEvent;
        while (settingsPage.pollEvent(winEvent)) {
            smallListButton.handleEvent(winEvent);
            largeListButton.handleEvent(winEvent);
            switch (winEvent.type)
            {
            case sf::Event::Closed:
                settingsPage.close();
                return 0;
            }
        }

        settingsPage.clear();

        smallListButton.draw(settingsPage);
        largeListButton.draw(settingsPage);
        title.draw(settingsPage);
        listToLoad.draw(settingsPage);

        settingsPage.display();
    }

    startTime = std::chrono::steady_clock::now();
    sf::RenderWindow imagegridWindow(sf::VideoMode(windowSize, windowSize), "Theos Image Viewer");

    std::string data;
    CDownloader downloader;
    downloader.Init();

    if (!downloader.Download(listUrl.c_str(), data)) {
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
    //promise to wait till images are downloaded before loaded
    std::vector<std::promise<void>> downloadPromises(imageCount);
    for (int i = 0; i < imageCount; i++) {
        downloadPromises[i] = std::promise<void>();
        //downloading images tasks assigned to the threadpool
        threadPool.enqueue([&, i]() {
                std::string filePath = "Images/" + urls[i].substr(urls[i].find_last_of('/') + 1);
                std::ifstream fileStream(filePath);
                if (fileStream.good()) {
                    std::cout << "Download skipped, image already exitst in files: " << urls[i] << "\n";
                    downloadPromises[i].set_value();
                }
                else if (downloader.DownloadToFile(urls[i].c_str(), filePath.c_str())) {
                    std::cout << "Image download success: " << urls[i] << "\n";
                    downloadPromises[i].set_value();
                }
                else {
                    std::cout << "Failed to download image: " << urls[i] << "\n";
                    downloadPromises[i].set_value();
                }
                fileStream.close();
            });
        //loading images tasks assigned to the threadpool
        threadPool.enqueue([&, i]() {
                downloadPromises[i].get_future().wait();
                if (imageTextures[i].loadFromFile(filePaths[i])) {
                    std::cout << "Image Loaded From: " << filePaths[i] << "\n";
                    imagegrid.setTileTexture(&imageTextures[i]);
                }
                else {
                    std::cout << "Failed to load image: " << filePaths[i] << "\n";
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
    int minimumZoomFactor = 1;
    int maxZoomFactor = std::sqrt(imageCount);
    int zoomFactor = 3;

    int pageCount = 0;
    int maxPages = imageCount / (zoomFactor * zoomFactor);

    float imageSize = windowSize / zoomFactor;
    imagegrid.updateGrid(imageSize, zoomFactor, pageCount);

    while (imagegridWindow.isOpen()) {
        sf::Event winEvent;
        while (imagegridWindow.pollEvent(winEvent)) {
            switch (winEvent.type)
            {
            case sf::Event::Closed:
                imagegridWindow.close();
                break;
            case sf::Event::MouseButtonPressed:
                if (winEvent.mouseButton.button == sf::Mouse::Left)
                    pageCount++;
                else if (winEvent.mouseButton.button == sf::Mouse::Right)
                    pageCount--;
                break;
            case sf::Event::KeyPressed:
                if (winEvent.key.code == sf::Keyboard::Key::S)
                    sPressed = true;
                break;
            case sf::Event::KeyReleased:
                if (winEvent.key.code == sf::Keyboard::Key::S)
                    sPressed = false;
                break;
            case sf::Event::MouseWheelScrolled:
                if (winEvent.mouseWheelScroll.delta > 0)
                    scrolledUp = true;
                if (winEvent.mouseWheelScroll.delta < 0)
                    scrolledDown = true;
                break; 
            }
        }
        //input
        if (scrolledUp)
        {
            //zoom in
            zoomFactor--;
            if (zoomFactor < minimumZoomFactor)
            {
                zoomFactor = minimumZoomFactor;
            }
            imageSize = windowSize / zoomFactor;
        }
        if (scrolledDown)
        {
            //zoom out
            zoomFactor++;
            if (zoomFactor > maxZoomFactor)
            {
                zoomFactor = maxZoomFactor;
            }
            imageSize = windowSize / zoomFactor;
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

        imagegrid.updateGrid(imageSize, zoomFactor, pageCount);

        if (sPressed)
        {
            screenshot("Screenshots/screenshot", &imagegridWindow);
        }

        scrolledUp = false;
        scrolledDown = false;

        imagegridWindow.clear();

        imagegrid.draw(imagegridWindow, pageCount);

        imagegridWindow.display();
    }

    return 0;
}
