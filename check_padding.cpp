#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::Image img;
    if (!img.loadFromFile("final boss/PNG files/idle/idle_1.png")) {
        std::cerr << "Fail\n"; return 1;
    }
    int bottom = -1;
    for (unsigned int y = img.getSize().y; y > 0; --y) {
        for (unsigned int x = 0; x < img.getSize().x; ++x) {
            if (img.getPixel(x, y - 1).a > 0) {
                bottom = y;
                break;
            }
        }
        if (bottom != -1) break;
    }
    std::cout << "Image height: " << img.getSize().y << "\n";
    std::cout << "Bottom-most non-transparent pixel: " << bottom << "\n";
    std::cout << "Padding: " << img.getSize().y - bottom << "\n";
    return 0;
}
