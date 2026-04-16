#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  Player.hpp
// ─────────────────────────────────────────────────────────────────────────────
#include <SFML/Graphics.hpp>
#include <vector>
#include "AnimData.hpp"

// Player animation / movement state
enum class PState { IDLE, RUN, JUMP, FALL, ATTACK, ATTACK2, HIT, DEAD };

class Player {
public:
    // ── World position (center of feet) ──────────────────────────────────────
    float x = 200.f, y = 556.f;
    float vx = 0.f,  vy = 0.f;
    bool  onGround    = false;
    bool  facingRight = true;

    // ── Stats (set per level via initStats) ───────────────────────────────────
    float maxHP = 100.f, hp = 100.f;
    float damage = 25.f;
    bool  isDead = false;      // true after death animation completes

    // ── Attack state ─────────────────────────────────────────────────────────
    bool          isAttacking  = false;
    bool          attackActive = false; // hitbox is "live" this frame
    sf::FloatRect attackBox;            // world-space hitbox

    // ── Invincibility frames ──────────────────────────────────────────────────
    float iTimer = 0.f;   // counts down from PLR_IFRAMES

    // ── Animation ────────────────────────────────────────────────────────────
    PState state    = PState::IDLE;
    int    frame    = 0;
    float  ftimer   = 0.f;
    bool   animDone = false;  // one-shot animation finished?

    sf::Sprite sprite;

    // ── Animation sets ────────────────────────────────────────────────────────
    AnimData aIdle, aRun, aJump, aFall, aAtk, aAtk2, aHit, aDeath;

    // ── Public interface ──────────────────────────────────────────────────────
    Player() = default;

    // Load all sprite frames from disk  (call once)
    bool loadAssets();

    // Set HP / damage based on level number [1-5]
    void initStats(int level);

    // Master update: input → physics → platform collision → anim
    void update(float dt,
                const std::vector<sf::FloatRect>& platforms,
                float levelLeft, float levelRight);

    // Apply damage (respects invincibility frames)
    void takeDamage(float dmg);

    // Axis-aligned bounding box for collision with enemies
    sf::FloatRect getBounds() const;

    // Draw sprite at screen coords (worldX - camX)
    void draw(sf::RenderWindow& win, float camX);

private:
    void handleInput(float dt);
    void applyPhysics(float dt,
                      const std::vector<sf::FloatRect>& platforms,
                      float lLeft, float lRight);
    void updateAttackBox();
    void updateAnim(float dt);
    void updateState();           // transition state after one-shot anims
    void setMoveState();          // pick IDLE / RUN / JUMP / FALL

    const AnimData& curAnim() const;
    static constexpr float SCALE = 2.2f;
};
