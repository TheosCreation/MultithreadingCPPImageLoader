#include <iostream>
#include <fstream>
#include <vector>
#include <future>
#include <condition_variable>
#include <SFML/Graphics.hpp>
#include "ImageGrid.h"
#include "Downloader.h"
#include "CThreadPool.h"
#include "Button.h"
#include "Text.h"

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

// verify an image from files much faster than loading a sf::texture in memory and checking it
bool verifyImage(const std::string& filePath) {
    // open the file
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return false; // failed because most likely not exists
    }

    // reads the first few bytes to check to see if its a png or jpg
    const int HEADER_SIZE = 8;
    char header[HEADER_SIZE];
    file.read(header, HEADER_SIZE);

    if (file.gcount() < HEADER_SIZE) {
        return false; // file is too small or not an image format we recognize
    }

    return true; // file is likely a valid image so load it
}

// takes a screenshot of the current screen and save the to a file location alittle slow
void screenshot(const std::string& fileSaveLocation, sf::Window* window) {
    sf::Texture texture;
    texture.create(window->getSize().x, window->getSize().y);
    texture.update(*window);

    // Generate unique filenames
    std::string filename;
    int i = 0;
    do {
        filename = fileSaveLocation + std::to_string(i) + ".png";
        ++i;
    } while (std::ifstream(filename).good());

    // Save the screenshot to a file
    if (texture.copyToImage().saveToFile(filename)) {
        std::cout << "Screenshot saved to " << filename << std::endl;
    }
}

int main() {

    int windowSize = 1260;

    // creates a welcomepage and adds two buttons and title and subheading
    sf::RenderWindow settingsPage(sf::VideoMode(windowSize, windowSize), "Welcome");

    Text title("VCR_OSD_MONO_1.001.ttf", 60, sf::Color::White, "Welcome to the Image Viewer", 200, 50);
    Text listToLoad("VCR_OSD_MONO_1.001.ttf", 50, sf::Color::White, "Image List To Load", 400, 200);

    std::string listUrl;
    // will close the welcome window and set the list to load to the small list
    Button smallListButton("VCR_OSD_MONO_1.001.ttf", sf::Color::Red, 300, 300, 200, 50, "Small List", [&listUrl, &settingsPage]() {
        listUrl = "https://raw.githubusercontent.com/MDS-HugoA/TechLev/main/ImgListSmall.txt";
        settingsPage.close();
        });

    // will close the welcome window and set the list to load to the large list
    Button largeListButton("VCR_OSD_MONO_1.001.ttf", sf::Color::Red, 700, 300, 200, 50, "Large List", [&listUrl, &settingsPage]() {
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

    sf::RenderWindow imagegridWindow(sf::VideoMode(windowSize, windowSize), "Theos Image Viewer");

    std::string data;
    CDownloader downloader;
    downloader.Init();
    // downloads all the urls from the selected list on the data string
    if (!downloader.Download(listUrl.c_str(), data)) {
        std::cerr << "Data failed to download";
        return -1;
    }

    // urls are split from the data and saved in a vector
    std::vector<std::string> urls = splitUrls(data);

    // filepaths are split from the urls and saved in a vector
    std::vector<std::string> filePaths;
    for (const auto& url : urls) {
        std::string filePath = "Images/" + url.substr(url.find_last_of('/') + 1);
        filePaths.push_back(filePath);
    }

    // calculates the count images from the list of urls
    int imageCount = urls.size();

    // grid size is the number of columns or rows there are on the image grid
    int gridSize = std::sqrt(imageCount);
    if (gridSize * gridSize < imageCount) {
        gridSize++;
    }
    // will generate a empty image grid based on the number of images from the url list
    ImageGrid imagegrid(windowSize / gridSize, gridSize);

    // these are the textures of the images on the image grid
    std::vector<sf::Texture> imageTextures;
    imageTextures.resize(imageCount);

    // creates a thread pool to be used for downloading/verifying and loading images onto the image grid
    CThreadPool threadPool(std::thread::hardware_concurrency());

    // promise to wait till images are downloaded before loaded
    std::vector<std::promise<void>> downloadPromises(imageCount);
    for (int i = 0; i < imageCount; i++) {
        downloadPromises[i] = std::promise<void>();
        // downloading images tasks assigned to the threadpool
        threadPool.enqueue([&, i]() {
                if (verifyImage(filePaths[i])) {
                    std::cout << "Download skipped, image already exists in files: " << urls[i] << "\n";
                    downloadPromises[i].set_value();
                }
                else if (downloader.DownloadToFile(urls[i].c_str(), filePaths[i].c_str())) {
                    std::cout << "Image download success: " << urls[i] << "\n";
                    downloadPromises[i].set_value();
                }
                else {
                    std::cout << "Failed to download image: " << urls[i] << "\n";
                    downloadPromises[i].set_value();
                }
            });
        // loading images tasks assigned to the threadpool
        threadPool.enqueue([&, i]() {
                downloadPromises[i].get_future().wait();
                if (imageTextures[i].loadFromFile(filePaths[i])) {
                    std::cout << "Image Loaded From: " << filePaths[i] << "\n";
                    imagegrid.setTileTexture(&imageTextures[i]);
                }
            });
    }

    // input control checks
    bool controlPressed = false;
    bool sPressed = false;
    bool scrolledUp = false;
    bool scrolledDown = false;

    // zoom factor is how many ros/columns are shown on screen
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

        // zooms in and determines the number of pages allowed
        if (scrolledUp)
        {
            zoomFactor--;
            if (zoomFactor < minimumZoomFactor)
            {
                zoomFactor = minimumZoomFactor;
            }
            imageSize = windowSize / zoomFactor;
            maxPages = imageCount / (zoomFactor * zoomFactor);
        }

        // zooms out and determines the number of pages allowed
        if (scrolledDown)
        {
            zoomFactor++;
            if (zoomFactor > maxZoomFactor)
            {
                zoomFactor = maxZoomFactor;
            }
            imageSize = windowSize / zoomFactor;
            maxPages = imageCount / (zoomFactor * zoomFactor);
        }

        // ensure the user cannot go past the max and minimum page
        if (pageCount < 0)
        {
            pageCount = 0;
        }
        if (pageCount > maxPages-1)
        {
            pageCount = maxPages-1;
        }

        // resizes and positions the images based on page, zoom and image size
        imagegrid.updateGrid(imageSize, zoomFactor, pageCount);

        if (sPressed)
        {
            screenshot("Screenshots/screenshot", &imagegridWindow);
        }

        // clears input so there is no looping
        scrolledUp = false;
        scrolledDown = false;

        imagegridWindow.clear();

        // renders the grid
        imagegrid.draw(imagegridWindow, pageCount);

        imagegridWindow.display();
    }

    return 0;
}
