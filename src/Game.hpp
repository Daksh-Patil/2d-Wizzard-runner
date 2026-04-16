#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  Game.hpp  –  Top-level game manager / state machine
//
//  States:
//    MENU           → title screen
//    PLAYING        → active gameplay
//    LEVEL_DONE     → brief "LEVEL COMPLETE" banner (2 s)
//    GAME_OVER      → player died
//    VICTORY        → beat all 5 levels
// ─────────────────────────────────────────────────────────────────────────────
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

#include "Player.hpp"
#include "Enemy.hpp"
#include "Boss.hpp"
#include "Level.hpp"
#include "ParallaxBG.hpp"
#include "UI.hpp"

enum class GState { MENU, PLAYING, LEVEL_DONE, GAME_OVER, VICTORY };

class Game {
public:
    Game();
    void run();

private:
    // ── Core objects ─────────────────────────────────────────────────────────
    sf::RenderWindow window_;
    sf::Clock        clock_;
    GState           state_      = GState::MENU;
    int              levelNum_   = 1;   // 1-5
    float            stateTimer_ = 0.f; // used for LEVEL_DONE countdown
    float            camX_       = 0.f; // camera left edge in world space
    float            prevCamX_   = 0.f; // last frame camera position

    // ── Cheat System ─────────────────────────────────────────────────────────
    bool             cheatMenuActive_ = false;
    bool             infiniteHealth_  = false;

    // ── Game objects ─────────────────────────────────────────────────────────
    Player                   player_;
    std::vector<Enemy>       enemies_;
    Boss                     boss_;
    bool                     bossActive_ = false;
    Level                    level_;
    ParallaxBG               bg_;
    UI                       ui_;

    // ── Private methods ───────────────────────────────────────────────────────
    void loadLevel(int num);
    void spawnEnemies(int count, int lvl);

    void handleEvents();
    void update(float dt);
    void render();

    // State-specific updates
    void updateMenu(float dt);
    void updatePlaying(float dt);
    void updateLevelDone(float dt);
    void updateGameOver(float dt);

    // Helpers
    void updateCamera();
    void checkAttackHits();   // player attack vs enemies + boss
    void checkEnemyHits();    // enemy attacks vs player
    void checkDoorTrigger();
};
