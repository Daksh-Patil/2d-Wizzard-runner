#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  Level.hpp  –  Procedurally generated level data.
//
//  Generates platforms, places solid rects, sets up enemy spawn points, and
//  positions the exit door. Tileset texture is shared/static.
// ─────────────────────────────────────────────────────────────────────────────
#include <SFML/Graphics.hpp>
#include <vector>

// Represents one elevated platform (a row of tiles)
struct Platform {
    sf::FloatRect rect;              // world-space AABB (top = walkable surface)
    std::vector<sf::Sprite> tiles;   // visual tile sprites
};

class Level {
public:
    std::vector<Platform>      platforms;
    std::vector<sf::FloatRect> solidRects;   // ground + platforms (for collision)
    std::vector<sf::Sprite>    groundTiles;
    std::vector<sf::Vector2f>  enemySpawns;  // world-space spawn points

    sf::Sprite    doorSprite;
    sf::FloatRect doorRect;           // trigger zone for level transition
    float         width    = 7000.f;
    int           levelNum = 1;

    // Shared textures (static – loaded once)
    static sf::Texture tileTex, doorTex;
    static bool        texLoaded;

    // Must be called once before generating any level
    static bool loadTextures();

    // Procedurally generate a level
    // seed=0 → uses levelNum as seed
    void generate(int level, unsigned seed = 0);

    // Draw ground + platforms + door (enemies drawn by Game)
    void draw(sf::RenderWindow& win, float camX);

    const std::vector<sf::FloatRect>& getSolids() const { return solidRects; }
};
