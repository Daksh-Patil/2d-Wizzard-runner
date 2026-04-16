// ─────────────────────────────────────────────────────────────────────────────
//  ParallaxBG.cpp
// ─────────────────────────────────────────────────────────────────────────────
#include "ParallaxBG.hpp"
#include "Constants.hpp"
#include <iostream>

// ──────────────────────────────────────────────────────────────────────────────
bool BGLayer::load(const std::string& path, float scrollSpeed)
{
    if (!tex.loadFromFile(path)) {
        std::cerr << "[WARN] BG layer missing: " << path << '\n';
        return false;
    }
    speed = scrollSpeed;

    // Scale layer to fill the window height
    float scaleY = static_cast<float>(WIN_H) / tex.getSize().y;
    float scaleX = scaleY;

    float w = tex.getSize().x * scaleX;

    sp[0].setTexture(tex);
    sp[0].setScale(scaleX, scaleY);
    sp[0].setPosition(0.f, 0.f);

    sp[1].setTexture(tex);
    sp[1].setScale(scaleX, scaleY);
    sp[1].setPosition(w, 0.f);

    posX = 0.f;
    return true;
}

void BGLayer::update(float dx)
{
    // Move left proportionally to how much the camera moved
    posX -= dx * speed;

    float w = sp[0].getGlobalBounds().width;

    // Wrap: keep both sprites forming a seamless strip
    if (posX <= -w) posX += w;
    if (posX >  0.f) posX -= w;

    sp[0].setPosition(posX,     0.f);
    sp[1].setPosition(posX + w, 0.f);
}

void BGLayer::draw(sf::RenderWindow& w)
{
    w.draw(sp[0]);
    w.draw(sp[1]);
}

// ──────────────────────────────────────────────────────────────────────────────
bool ParallaxBG::load()
{
    // Back → front ordering; smaller speed = perceived to be farther away
    struct Info { const char* file; float speed; };
    static const Info LAYERS[] = {
        { "Desert Parallax by Cryptogene/9 Background.png", 0.04f },
        { "Desert Parallax by Cryptogene/8 Stars.png",      0.07f },
        { "Desert Parallax by Cryptogene/6 Sun.png",        0.10f },
        { "Desert Parallax by Cryptogene/5 Mountains.png",  0.16f },
        { "Desert Parallax by Cryptogene/4 Layer4.png",     0.22f },
        { "Desert Parallax by Cryptogene/3 Layer3.png",     0.28f },
        { "Desert Parallax by Cryptogene/2 Layer2.png",     0.35f },
        { "Desert Parallax by Cryptogene/1 Layer1.png",     0.42f },
    };

    layers_.clear();
    layers_.reserve(8); // MUST reserve before any emplace_back to prevent reallocation

    for (auto& info : LAYERS) {
        // emplace_back FIRST so the BGLayer lives at its final stable address,
        // THEN call load() so sp[0/1].setTexture(tex) points to that stable address.
        // If we did push_back(std::move(bl)) the texture would move, invalidating
        // the sprites' raw const sf::Texture* pointers → draws GL texture 0 (white).
        layers_.emplace_back();
        layers_.back().load(info.file, info.speed);
    }
    return true;
}

void ParallaxBG::update(float cameraDX)
{
    for (auto& l : layers_)
        l.update(cameraDX);
}

void ParallaxBG::draw(sf::RenderWindow& window)
{
    for (auto& l : layers_)
        l.draw(window);
}
