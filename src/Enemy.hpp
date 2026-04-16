#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  Enemy.hpp  –  Skeleton enemy with simple AI
//
//  All skeleton enemies share ONE set of AnimData (static members) to avoid
//  reloading the same textures for each instance.
// ─────────────────────────────────────────────────────────────────────────────
#include <SFML/Graphics.hpp>
#include <vector>
#include "AnimData.hpp"

enum class EState { IDLE, WALK, ATTACK, HIT, DEAD };

class Enemy {
public:
    // ── World position (center of feet) ──────────────────────────────────────
    float  x, y;
    float  vx = 0.f;
    bool   facingRight = false;

    // ── Stats ────────────────────────────────────────────────────────────────
    float  maxHP, hp;
    float  damage;
    float  atkCooldown;      // dynamic cooldown
    bool   removed = false;  // mark for removal after death anim

    // ── Attack ────────────────────────────────────────────────────────────────
    float         atkTimer     = 0.f;   // cooldown counter
    bool          atkHitActive = false; // deal damage this frame?
    sf::FloatRect atkBox;               // world-space hitbox

    // ── Animation ─────────────────────────────────────────────────────────────
    EState state    = EState::IDLE;
    int    frame    = 0;
    float  ftimer   = 0.f;
    bool   animDone = false;

    sf::Sprite sprite;

    // ── Public interface ──────────────────────────────────────────────────────
    Enemy(float wx, float wy, float maxHp, float dmg, float atkCd);

    // Load shared textures once before spawning any enemies
    static bool loadSharedAssets();

    // Per-frame update: AI + physics + animation
    void update(float dt, float playerX, float playerY);

    // Apply damage (plays hit animation)
    void takeDamage(float dmg);

    // AABB for player collision / attack detection
    sf::FloatRect getBounds() const;

    void draw(sf::RenderWindow& win, float camX);

private:
    void updateAI(float dt, float playerX, float playerY);
    void updateAnim(float dt);
    const AnimData& curAnim() const;
    void  updateAtkBox();

    static constexpr float SCALE = 2.9f;

    // ── Shared (static) animations for ALL skeleton instances ─────────────────
    static AnimData sIdle, sWalk, sAtk, sHit, sDeath;
    static bool     sLoaded;
};
