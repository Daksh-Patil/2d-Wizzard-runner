#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  ParallaxBG.hpp  –  Multi-layer parallax scrolling background.
//
//  Each layer has a speed factor [0–1]. When the camera moves right by dx:
//    - far layers (small factor) barely move  →  depth illusion
//    - near layers (large factor) move faster
//  Two sprites per layer are tiled side-by-side for seamless looping.
// ─────────────────────────────────────────────────────────────────────────────
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// One scrolling layer
struct BGLayer {
    sf::Texture tex;
    sf::Sprite  sp[2];     // two copies for seamless loop
    float       speed = 0; // fraction of camera speed this layer uses
    float       posX  = 0; // current left edge X in screen space

    // Returns true if the texture loaded
    bool load(const std::string& path, float scrollSpeed);
    // dx = pixels the camera moved RIGHT this frame
    void update(float dx);
    void draw(sf::RenderWindow& w);
};

// Manages all background layers
class ParallaxBG {
public:
    // Load all desert parallax layers from disk
    bool load();
    // Call every frame with camera horizontal displacement
    void update(float cameraDX);
    void draw(sf::RenderWindow& window);

private:
    std::vector<BGLayer> layers_;
};
