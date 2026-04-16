#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  Boss.hpp  –  Final boss (Level 5 only)
//
//  Frost Guardian sprite: individual PNG frames in final boss/PNG files/
//  HP = 2 × player maxHP at level 5; same damage as player
// ─────────────────────────────────────────────────────────────────────────────
#include <SFML/Graphics.hpp>
#include "AnimData.hpp"

enum class BossState { IDLE, WALK, ATTACK, HIT, DEAD };

class Boss {
public:
    float x, y;
    float vx = 0.f;
    bool  facingRight = false;

    float maxHP, hp;
    float damage;
    bool  isDead   = false;
    bool  defeated = false;  // true after death animation completes

    // Attack
    float         atkTimer     = 0.f;
    bool          atkHitActive = false;
    sf::FloatRect atkBox;

    // Animation
    BossState state    = BossState::IDLE;
    int       frame    = 0;
    float     ftimer   = 0.f;
    bool      animDone = false;

    sf::Sprite sprite;

    // Animations
    AnimData aIdle, aWalk, aAtk, aHit, aDeath;

    // ── Public interface ──────────────────────────────────────────────────────
    Boss() = default;

    // Load Frost Guardian frames
    bool loadAssets();

    // Set HP = 2 * playerMaxHP, damage = playerDamage
    void initStats(float playerMaxHP, float playerDamage);

    // Spawn at given world position
    void spawn(float wx, float wy);

    void update(float dt, float playerX, float playerY);
    void takeDamage(float dmg);

    sf::FloatRect getBounds() const;

    void draw(sf::RenderWindow& win, float camX);

private:
    void updateAI(float dt, float playerX, float playerY);
    void updateAnim(float dt);
    const AnimData& curAnim() const;
    void  updateAtkBox();

    static constexpr float SCALE = 4.0f;
};
