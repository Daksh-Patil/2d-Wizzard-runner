// ─────────────────────────────────────────────────────────────────────────────
//  Game.cpp  –  Top-level game manager
// ─────────────────────────────────────────────────────────────────────────────
#include "Game.hpp"
#include "Constants.hpp"
#include <algorithm>
#include <iostream>
#include <random>

// ── Constructor ───────────────────────────────────────────────────────────────
Game::Game()
    : window_(sf::VideoMode(WIN_W, WIN_H), "Kobold Warrior  –  5 Level Platformer",
              sf::Style::Titlebar | sf::Style::Close)
{
    window_.setFramerateLimit(60);
}

// ── Entry point ───────────────────────────────────────────────────────────────
void Game::run()
{
    // ── One-time loading ──────────────────────────────────────────────────────
    bg_.load();
    Level::loadTextures();
    Enemy::loadSharedAssets();
    player_.loadAssets();
    boss_.loadAssets();
    ui_.load();

    // ── Start at menu ─────────────────────────────────────────────────────────
    state_ = GState::MENU;

    while (window_.isOpen()) {
        float dt = clock_.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f; // cap at 20 fps equivalent

        handleEvents();
        update(dt);
        render();
    }
}

// ── Event processing ──────────────────────────────────────────────────────────
void Game::handleEvents()
{
    sf::Event ev;
    while (window_.pollEvent(ev)) {
        if (ev.type == sf::Event::Closed)
            window_.close();

        if (ev.type == sf::Event::KeyPressed) {
            if (ev.key.code == sf::Keyboard::Escape)
                window_.close();

            // ── Cheat Menu ───────────────────────────────────────────────────
            if (state_ == GState::PLAYING && ev.key.code == sf::Keyboard::X) {
                cheatMenuActive_ = !cheatMenuActive_;
                continue;
            }

            if (state_ == GState::PLAYING && cheatMenuActive_) {
                if (ev.key.code == sf::Keyboard::H) {
                    infiniteHealth_ = !infiniteHealth_;
                }
                else if (ev.key.code >= sf::Keyboard::Num1 && ev.key.code <= sf::Keyboard::Num5) {
                    levelNum_ = ev.key.code - sf::Keyboard::Num0;
                    loadLevel(levelNum_);
                    cheatMenuActive_ = false;
                }
                else if (ev.key.code >= sf::Keyboard::Numpad1 && ev.key.code <= sf::Keyboard::Numpad5) {
                    levelNum_ = ev.key.code - sf::Keyboard::Numpad0;
                    loadLevel(levelNum_);
                    cheatMenuActive_ = false;
                }
                continue; // Block standard inputs while cheat menu is open
            }
            // ─────────────────────────────────────────────────────────────────

            // Menu → start game
            if (state_ == GState::MENU &&
                (ev.key.code == sf::Keyboard::Enter ||
                 ev.key.code == sf::Keyboard::Space)) {
                levelNum_ = 1;
                loadLevel(levelNum_);
                state_ = GState::PLAYING;
            }

            // Game Over / Victory → back to menu
            if ((state_ == GState::GAME_OVER || state_ == GState::VICTORY) &&
                (ev.key.code == sf::Keyboard::Enter ||
                 ev.key.code == sf::Keyboard::Space ||
                 ev.key.code == sf::Keyboard::R)) {
                state_ = GState::MENU;
            }
        }
    }
}

// ── Master update dispatcher ──────────────────────────────────────────────────
void Game::update(float dt)
{
    switch (state_) {
        case GState::MENU:       updateMenu(dt);       break;
        case GState::PLAYING:    updatePlaying(dt);    break;
        case GState::LEVEL_DONE: updateLevelDone(dt);  break;
        case GState::GAME_OVER:  updateGameOver(dt);   break;
        case GState::VICTORY:    /* static screen */   break;
    }
}

// ── Menu ──────────────────────────────────────────────────────────────────────
void Game::updateMenu(float dt)
{
    // Parallax scrolls slowly in menu for aesthetics
    bg_.update(1.5f);
}

// ── Playing ───────────────────────────────────────────────────────────────────
void Game::updatePlaying(float dt)
{
    if (cheatMenuActive_) return; // Pause game logic while Cheat Menu is open

    if (infiniteHealth_) {
        player_.hp = player_.maxHP;
    }

    // --- Player ---
    std::vector<sf::FloatRect> solids = level_.getSolids();
    player_.update(dt, solids, 0.f, level_.width);

    if (player_.isDead) {
        state_ = GState::GAME_OVER;
        return;
    }

    // --- Enemies ---
    for (auto& e : enemies_)
        e.update(dt, player_.x, player_.y);

    // Remove dead enemies (after death animation)
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
                       [](const Enemy& e) { return e.removed; }),
        enemies_.end());

    // --- Boss (Level 5) ---
    if (bossActive_ && !boss_.defeated)
        boss_.update(dt, player_.x, player_.y);

    // --- Hit detection ---
    checkAttackHits();
    checkEnemyHits();

    // --- Door trigger ---
    checkDoorTrigger();

    // --- Camera ---
    updateCamera();
    float dx = camX_ - prevCamX_;
    prevCamX_ = camX_;
    bg_.update(dx);

    // --- Boss victory ---
    if (bossActive_ && boss_.defeated) {
        state_ = GState::VICTORY;
    }
}

// ── Level done (brief banner) ─────────────────────────────────────────────────
void Game::updateLevelDone(float dt)
{
    stateTimer_ -= dt;
    if (stateTimer_ <= 0.f) {
        levelNum_++;
        if (levelNum_ > 5) {
            state_ = GState::VICTORY;
        } else {
            loadLevel(levelNum_);
            state_ = GState::PLAYING;
        }
    }
}

void Game::updateGameOver(float /*dt*/)
{
    // Static screen waiting for Enter / R in handleEvents
}

// ── Load a level ──────────────────────────────────────────────────────────────
void Game::loadLevel(int num)
{
    level_.generate(num);

    // Reset player position (same HP carried over) at left of world
    player_.x = 200.f;
    player_.y = GROUND_Y;
    player_.vx = 0.f; player_.vy = 0.f;
    player_.onGround  = false;
    player_.isDead    = false;
    player_.isAttacking  = false;
    player_.attackActive = false;
    player_.iTimer    = 0.f;
    player_.state     = PState::IDLE;
    player_.frame     = 0;

    // First level: full HP. Later levels keep current HP for continuity.
    if (num == 1) player_.initStats(1);
    else          player_.initStats(num);  // re-scale maxHP; hp stays

    // Reset camera
    camX_ = prevCamX_ = 0.f;

    // Spawn enemies
    spawnEnemies(ENEMY_COUNTS[num - 1], num);

    // Boss on level 5
    bossActive_ = false;
    if (num == 5) {
        bossActive_ = true;
        boss_.initStats(player_.maxHP, player_.damage);
        boss_.spawn(LEVEL_W - 700.f, GROUND_Y);
    }
}

// ── Spawn enemies ─────────────────────────────────────────────────────────────
void Game::spawnEnemies(int count, int lvl)
{
    enemies_.clear();
    float mult = 1.f;
    for (int i = 1; i < lvl; ++i) mult *= ENM_HP_MULT;
    float hp  = ENM_BASE_HP * mult;
    float dmg = player_.maxHP * 0.08f;
    float atkCd = ENM_ATK_CD * std::pow(0.8f, lvl - 1);

    auto& spawns = level_.enemySpawns;

    // Use as many pre-calculated spawn points as available
    int n = std::min(count, static_cast<int>(spawns.size()));
    for (int i = 0; i < n; ++i)
        enemies_.emplace_back(spawns[i].x, spawns[i].y, hp, dmg, atkCd);

    // If more needed, spread remaining on the ground
    std::mt19937 rng(static_cast<unsigned>(lvl * 999));
    for (int i = n; i < count; ++i) {
        float ex = 800.f + static_cast<float>(rng() % static_cast<int>(LEVEL_W - 1200));
        enemies_.emplace_back(ex, GROUND_Y, hp, dmg, atkCd);
    }
}

// ── Camera ────────────────────────────────────────────────────────────────────
void Game::updateCamera()
{
    // Soft-track: centre player horizontally with a dead zone
    float targetX = player_.x - WIN_W / 2.f;
    targetX = std::max(0.f, std::min(LEVEL_W - WIN_W, targetX));
    // Smooth follow
    camX_ += (targetX - camX_) * 0.12f;
    camX_  = std::max(0.f, std::min(LEVEL_W - WIN_W, camX_));
}

// ── Player attack vs enemies / boss ──────────────────────────────────────────
void Game::checkAttackHits()
{
    if (!player_.attackActive) return;
    sf::FloatRect atk = player_.attackBox;

    for (auto& e : enemies_) {
        if (e.state == EState::DEAD) continue;
        if (atk.intersects(e.getBounds())) {
            e.takeDamage(player_.damage);
        }
    }

    if (bossActive_ && !boss_.isDead) {
        if (atk.intersects(boss_.getBounds())) {
            boss_.takeDamage(player_.damage);
        }
    }

    // Consume the hit so it doesn't trigger multiple times per swing
    player_.attackActive = false;
}

// ── Enemy attacks vs player ───────────────────────────────────────────────────
void Game::checkEnemyHits()
{
    sf::FloatRect pb = player_.getBounds();

    for (auto& e : enemies_) {
        if (!e.atkHitActive) continue;
        if (e.atkBox.intersects(pb))
            player_.takeDamage(e.damage);
    }

    if (bossActive_ && boss_.atkHitActive) {
        if (boss_.atkBox.intersects(pb))
            player_.takeDamage(boss_.damage);
    }
}

// ── Door trigger ──────────────────────────────────────────────────────────────
void Game::checkDoorTrigger()
{
    if (player_.getBounds().intersects(level_.doorRect)) {
        // Level 5 door only unlocks after boss is defeated
        if (levelNum_ == 5 && !boss_.defeated) return;

        stateTimer_ = 2.0f;
        state_      = GState::LEVEL_DONE;
    }
}

// ── Render ────────────────────────────────────────────────────────────────────
void Game::render()
{
    window_.clear(sf::Color(15, 15, 20));

    switch (state_) {

    // ── Menu ─────────────────────────────────────────────────────────────────
    case GState::MENU:
        bg_.draw(window_);
        ui_.drawMessage(window_,
                        "KOBOLD WARRIOR",
                        "Press ENTER or SPACE to start   |   WASD / Arrows to move   |   LMB / RMB to attack");
        break;

    // ── Playing ──────────────────────────────────────────────────────────────
    case GState::PLAYING:
        {
            bg_.draw(window_);
            level_.draw(window_, camX_);

            for (auto& e : enemies_) {
                e.draw(window_, camX_);
                ui_.drawEnemyHP(window_, e.x, e.y, e.hp, e.maxHP, camX_);
            }

            if (bossActive_ && !boss_.defeated)
                boss_.draw(window_, camX_);

            player_.draw(window_, camX_);

            // HUD
            ui_.drawPlayerHP(window_, player_.hp, player_.maxHP);
            ui_.drawLevelInfo(window_,
                              levelNum_,
                              static_cast<int>(enemies_.size()));
            if (bossActive_ && !boss_.defeated)
                ui_.drawBossHP(window_, boss_.hp, boss_.maxHP);

            // Door hint
            if (levelNum_ < 5) {
                // simple static banner
                float doorScreenX = level_.doorRect.left - camX_;
                if (doorScreenX > 0.f && doorScreenX < WIN_W - 60.f) {
                    // show nothing extra; the door sprite is visible
                }
            }

            if (cheatMenuActive_) {
                sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(WIN_W), static_cast<float>(WIN_H)));
                overlay.setFillColor(sf::Color(0, 0, 0, 180));
                window_.draw(overlay);

                std::string cheatTxt = "[H] Infinite Health (";
                cheatTxt += infiniteHealth_ ? "ON" : "OFF";
                cheatTxt += ")   |   [1-5] Jump to Level   |   [X] Close Menu";

                ui_.drawMessage(window_, "- CHEAT MENU -", cheatTxt);
            }
        }
        break;

    // ── Level done ────────────────────────────────────────────────────────────
    case GState::LEVEL_DONE:
        {
            bg_.draw(window_);
            level_.draw(window_, camX_);
            player_.draw(window_, camX_);
            float alpha = (stateTimer_ / 2.0f) * 255.f;
            ui_.drawFading(window_, "LEVEL COMPLETE!", alpha);
        }
        break;

    // ── Game Over ─────────────────────────────────────────────────────────────
    case GState::GAME_OVER:
        bg_.draw(window_);
        ui_.drawMessage(window_,
                        "GAME OVER",
                        "Press ENTER or R to return to menu");
        break;

    // ── Victory ───────────────────────────────────────────────────────────────
    case GState::VICTORY:
        bg_.draw(window_);
        ui_.drawMessage(window_,
                        "YOU WIN!",
                        "The Frost Guardian has fallen.  Press ENTER to play again.");
        break;
    }

    window_.display();
}
