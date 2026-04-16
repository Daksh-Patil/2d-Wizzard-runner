#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  UI.hpp  –  HUD: health bars, level info, messages
// ─────────────────────────────────────────────────────────────────────────────
#include <SFML/Graphics.hpp>
#include <string>

class UI {
public:
    bool load();   // load font

    // Draw player HP bar (green→red gradient, bottom-left)
    void drawPlayerHP(sf::RenderWindow& win, float hp, float maxHP);

    // Draw boss HP bar across top of screen
    void drawBossHP(sf::RenderWindow& win, float hp, float maxHP);

    // Draw small HP bar above an enemy (world-space, converted to screen)
    void drawEnemyHP(sf::RenderWindow& win,
                     float worldX, float worldY,
                     float hp, float maxHP,
                     float camX);

    // Draw level indicator  "LEVEL X / 5"
    void drawLevelInfo(sf::RenderWindow& win, int level, int enemies);

    // Draw a centred message with optional sub-text
    void drawMessage(sf::RenderWindow& win,
                     const std::string& main,
                     const std::string& sub = "");

    // Animate a fading message (alpha 0-255, decreasing)
    void drawFading(sf::RenderWindow& win,
                    const std::string& text,
                    float alpha);

private:
    sf::Font     font_;
    bool         hasFont_ = false;

    void drawBar(sf::RenderWindow& win,
                 float x, float y, float w, float h,
                 float ratio,
                 sf::Color fill, sf::Color back);
};
