// ─────────────────────────────────────────────────────────────────────────────
//  UI.cpp  –  HUD rendering
// ─────────────────────────────────────────────────────────────────────────────
#include "UI.hpp"
#include "Constants.hpp"
#include <algorithm>
#include <string>
#include <sstream>

// ── Load font ─────────────────────────────────────────────────────────────────
bool UI::load()
{
    hasFont_ = font_.loadFromFile(SYS_FONT);
    return hasFont_;
}

// ── Internal bar helper ───────────────────────────────────────────────────────
void UI::drawBar(sf::RenderWindow& win,
                 float x, float y, float w, float h,
                 float ratio,
                 sf::Color fill, sf::Color back)
{
    // Background
    sf::RectangleShape bg({ w, h });
    bg.setPosition(x, y);
    bg.setFillColor(back);
    bg.setOutlineThickness(2.f);
    bg.setOutlineColor(sf::Color(20, 20, 20, 220));
    win.draw(bg);

    // Fill
    if (ratio > 0.f) {
        sf::RectangleShape bar({ w * std::clamp(ratio, 0.f, 1.f), h });
        bar.setPosition(x, y);
        bar.setFillColor(fill);
        win.draw(bar);
    }
}

// ── Player HP bar (bottom-left) ───────────────────────────────────────────────
void UI::drawPlayerHP(sf::RenderWindow& win, float hp, float maxHP)
{
    float ratio = (maxHP > 0.f) ? hp / maxHP : 0.f;

    // Color gradient: green → yellow → red
    sf::Color fill =
        ratio > 0.6f ? sf::Color(50, 220, 80)   :   // green
        ratio > 0.3f ? sf::Color(230, 210, 30)   :   // yellow
                       sf::Color(230, 50,  30);       // red

    float bx = 20.f, by = static_cast<float>(WIN_H) - 36.f;
    float bw = 220.f, bh = 20.f;

    drawBar(win, bx, by, bw, bh, ratio, fill, sf::Color(40, 40, 40, 200));

    if (hasFont_) {
        sf::Text t;
        t.setFont(font_);
        t.setCharacterSize(14);
        t.setFillColor(sf::Color::White);
        std::ostringstream ss;
        ss << "HP  " << static_cast<int>(hp) << " / " << static_cast<int>(maxHP);
        t.setString(ss.str());
        t.setPosition(bx + 4.f, by + 2.f);
        win.draw(t);
    }
}

// ── Boss HP bar (full-width, top of screen) ───────────────────────────────────
void UI::drawBossHP(sf::RenderWindow& win, float hp, float maxHP)
{
    float ratio = (maxHP > 0.f) ? hp / maxHP : 0.f;
    float bx = 40.f, by = 12.f;
    float bw = static_cast<float>(WIN_W) - 80.f, bh = 22.f;

    drawBar(win, bx, by, bw, bh, ratio, sf::Color(180, 40, 200), sf::Color(40, 20, 50, 220));

    if (hasFont_) {
        sf::Text t;
        t.setFont(font_);
        t.setCharacterSize(14);
        t.setFillColor(sf::Color(255, 220, 255));
        std::ostringstream ss;
        ss << "FROST GUARDIAN  " << static_cast<int>(std::max(0.f, hp))
           << " / " << static_cast<int>(maxHP);
        t.setString(ss.str());
        // Centre the text inside the bar
        auto lb  = t.getLocalBounds();
        t.setPosition(bx + (bw - lb.width) / 2.f, by + 3.f);
        win.draw(t);
    }
}

// ── Small HP bar above enemy ──────────────────────────────────────────────────
void UI::drawEnemyHP(sf::RenderWindow& win,
                     float worldX, float worldY,
                     float hp, float maxHP,
                     float camX)
{
    if (maxHP <= 0.f || hp >= maxHP) return; // only show if damaged
    float ratio = hp / maxHP;
    float sx = worldX - camX - 25.f;
    float sy = worldY - 80.f;   // above head
    drawBar(win, sx, sy, 50.f, 6.f, ratio,
            sf::Color(220, 50, 50), sf::Color(60, 20, 20, 200));
}

// ── Level indicator (top-right) ───────────────────────────────────────────────
void UI::drawLevelInfo(sf::RenderWindow& win, int level, int enemies)
{
    if (!hasFont_) return;
    sf::Text t;
    t.setFont(font_);
    t.setCharacterSize(18);
    t.setFillColor(sf::Color(255, 230, 100));

    std::ostringstream ss;
    ss << "LEVEL " << level << " / 5";
    t.setString(ss.str());
    auto lb = t.getLocalBounds();
    t.setPosition(static_cast<float>(WIN_W) - lb.width - 20.f, 10.f);
    win.draw(t);

    // Enemy count below
    sf::Text t2;
    t2.setFont(font_);
    t2.setCharacterSize(14);
    t2.setFillColor(sf::Color(200, 200, 200));
    ss.str("");
    ss << "Enemies: " << enemies;
    t2.setString(ss.str());
    auto lb2 = t2.getLocalBounds();
    t2.setPosition(static_cast<float>(WIN_W) - lb2.width - 20.f, 34.f);
    win.draw(t2);
}

// ── Centred message ───────────────────────────────────────────────────────────
void UI::drawMessage(sf::RenderWindow& win,
                     const std::string& main,
                     const std::string& sub)
{
    if (!hasFont_) return;

    // Semi-transparent overlay
    sf::RectangleShape overlay({ static_cast<float>(WIN_W), static_cast<float>(WIN_H) });
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    win.draw(overlay);

    // Main text
    sf::Text t;
    t.setFont(font_);
    t.setCharacterSize(52);
    t.setFillColor(sf::Color(255, 220, 80));
    t.setString(main);
    auto lb = t.getLocalBounds();
    t.setOrigin(lb.width / 2.f, lb.height / 2.f);
    t.setPosition(WIN_W / 2.f, WIN_H / 2.f - 30.f);
    win.draw(t);

    // Sub text
    if (!sub.empty()) {
        sf::Text s;
        s.setFont(font_);
        s.setCharacterSize(24);
        s.setFillColor(sf::Color(220, 220, 220));
        s.setString(sub);
        auto lb2 = s.getLocalBounds();
        s.setOrigin(lb2.width / 2.f, lb2.height / 2.f);
        s.setPosition(WIN_W / 2.f, WIN_H / 2.f + 40.f);
        win.draw(s);
    }
}

// ── Fading message ────────────────────────────────────────────────────────────
void UI::drawFading(sf::RenderWindow& win, const std::string& text, float alpha)
{
    if (!hasFont_) return;
    sf::Text t;
    t.setFont(font_);
    t.setCharacterSize(44);
    sf::Uint8 a = static_cast<sf::Uint8>(std::clamp(alpha, 0.f, 255.f));
    t.setFillColor(sf::Color(100, 255, 100, a));
    t.setString(text);
    auto lb = t.getLocalBounds();
    t.setOrigin(lb.width / 2.f, lb.height / 2.f);
    t.setPosition(WIN_W / 2.f, WIN_H / 2.f);
    win.draw(t);
}
