
//  Level.cpp  –  Procedural level generation

#include "Level.hpp"
#include "Constants.hpp"
#include <random>
#include <algorithm>
#include <iostream>

// ── Tile render 
static constexpr float TILE_W = 64.f;
static constexpr float TILE_H = 64.f;

// ── Static member definitions
sf::Texture Level::tileTex;
sf::Texture Level::doorTex;
bool        Level::texLoaded = false;

// ── Load shared textures 
bool Level::loadTextures()
{
    if (texLoaded) return true;
    bool ok = true;
    if (!tileTex.loadFromFile("tileset/tile.png"))
        std::cerr << "[ERROR] Cannot load tileset/tile.png\n", ok = false;
    if (!doorTex.loadFromFile("tileset/leveldoor.png"))
        std::cerr << "[ERROR] Cannot load tileset/leveldoor.png\n", ok = false;
    texLoaded = true;
    return ok;
}

// ── Helper: make a scaled tile sprite 
static sf::Sprite makeTile(float scaleX, float scaleY,
                            float wx, float wy)
{
    sf::Sprite s(Level::tileTex);
    s.setScale(scaleX, scaleY);
    s.setPosition(wx, wy);
    return s;
}

// ── Procedural generation 
void Level::generate(int level, unsigned seed)
{
    levelNum = level;
    platforms.clear();
    solidRects.clear();
    groundTiles.clear();
    enemySpawns.clear();

    std::mt19937 rng(seed ? seed : static_cast<unsigned>(level * 77777 + 12345));
    auto randInt = [&](int lo, int hi) -> int {
        return lo + static_cast<int>(rng() % static_cast<unsigned>(hi - lo + 1));
    };
    auto randF = [&](float lo, float hi) -> float {
        return lo + (hi - lo) * (static_cast<float>(rng() % 10000) / 10000.f);
    };

    // ── Tile scale factors (native → 64×64) 
    float natW  = static_cast<float>(tileTex.getSize().x);
    float natH  = static_cast<float>(tileTex.getSize().y);
    float scaleX = (natW > 0.f) ? TILE_W / natW : 1.f;
    float scaleY = (natH > 0.f) ? TILE_H / natH : 1.f;
    bool  hasTile = (natW > 0.f);

    // ── Ground solid rect
    // Extends 300 px downward so the player can never fall through
    solidRects.push_back({ 0.f, GROUND_Y, LEVEL_W, 300.f });

    // ── Ground visual tiles (one row along the walkable surface)
    if (hasTile) {
        int count = static_cast<int>(LEVEL_W / TILE_W) + 2;
        for (int i = 0; i < count; ++i)
            groundTiles.push_back(makeTile(scaleX, scaleY, i * TILE_W, GROUND_Y));
    }

    // ── Platforms (Removed to keep all tiles grounded) 
    // Since platforms are removed, the walkable area is just the ground section.

    // ── Enemy spawns on ground (spread evenly, skip player start area)
    int   groundCount = ENEMY_COUNTS[level - 1];
    float spacing     = (LEVEL_W - 1200.f) / std::max(1, groundCount);
    for (int i = 0; i < groundCount; ++i) {
        float ex = 700.f + i * spacing + randF(-60.f, 60.f);
        if (ex < LEVEL_W - 600.f)
            enemySpawns.push_back({ ex, GROUND_Y });
    }

    // ── Door 
    float doorX  = LEVEL_W - 430.f;
    float doorY  = GROUND_Y;   // door feet at ground level

    if (doorTex.getSize().x > 0) {
        float dScaleX = 90.f  / static_cast<float>(doorTex.getSize().x);
        float dScaleY = 110.f / static_cast<float>(doorTex.getSize().y);
        doorSprite.setTexture(doorTex);
        doorSprite.setScale(dScaleX, dScaleY);
        doorSprite.setPosition(doorX, doorY - 110.f);
    }
    // Interaction trigger zone
    doorRect = { doorX - 10.f, doorY - 115.f, 110.f, 115.f };
}

// ── Draw 
void Level::draw(sf::RenderWindow& win, float camX)
{
    // 1. Solid dirt fill across the whole bottom of the screen
    //    (no camera offset – it always covers the full screen width)
    sf::RectangleShape fill(sf::Vector2f(
        static_cast<float>(WIN_W),
        static_cast<float>(WIN_H) - GROUND_Y));
    fill.setPosition(0.f, GROUND_Y);
    fill.setFillColor(sf::Color(85, 60, 35));   // earthy brown dirt
    win.draw(fill);

    // Helper: is a world-space X on screen?
    auto onScreen = [&](float wx, float w) {
        float sx = wx - camX;
        return sx < WIN_W + w && sx + w > -w;
    };

    // 2. Ground tile row (world-space → screen via camX)
    for (auto& s : groundTiles) {
        float wx = s.getPosition().x;
        if (onScreen(wx, TILE_W)) {
            sf::Sprite tmp(s);
            tmp.setPosition(wx - camX, s.getPosition().y);
            win.draw(tmp);
        }
    }

    // 3. Platform tiles
    for (auto& plat : platforms) {
        for (auto& s : plat.tiles) {
            float wx = s.getPosition().x;
            if (onScreen(wx, TILE_W)) {
                sf::Sprite tmp(s);
                tmp.setPosition(wx - camX, s.getPosition().y);
                win.draw(tmp);
            }
        }
    }

    // 4. Exit door
    float dwx = doorSprite.getPosition().x;
    if (onScreen(dwx, 100.f)) {
        sf::Sprite tmpDoor(doorSprite);
        tmpDoor.setPosition(dwx - camX, doorSprite.getPosition().y);
        win.draw(tmpDoor);
    }
}
