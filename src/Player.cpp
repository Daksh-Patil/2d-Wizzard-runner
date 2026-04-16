// ─────────────────────────────────────────────────────────────────────────────
//  Player.cpp
// ─────────────────────────────────────────────────────────────────────────────
#include "Player.hpp"
#include "Constants.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cmath>

// ── Asset loading ─────────────────────────────────────────────────────────────
bool Player::loadAssets()
{
    bool ok = true;

    aIdle.frameTime = 0.12f;
    ok &= aIdle.load("main character/idle", "tile", 0, 7, 3);

    aRun.frameTime = 0.07f;
    ok &= aRun.load("main character/run", "tile", 0, 7, 3);

    aJump.frameTime = 0.1f;
    aJump.load("main character/jump", "tile", 0, 1, 3);
    if (aJump.empty()) aJump = aIdle;   // fallback

    aFall.frameTime = 0.1f;
    if (!aFall.load("main character/fall", "tile", 0, 3, 3))
        aFall = aJump;   // fall frames reuse jump if folder missing

    aAtk.frameTime = 0.065f;
    ok &= aAtk.load("main character/attack1", "tile", 0, 7, 3);

    aAtk2.frameTime = 0.065f;
    ok &= aAtk2.load("main character/attack2", "tile", 0, 7, 3);

    aHit.frameTime = 0.09f;
    aHit.load("main character/take hit", "tile", 0, 2, 3);
    if (aHit.empty()) aHit = aIdle;

    aDeath.frameTime = 0.1f;
    aDeath.load("main character/death", "tile", 0, 6, 3);
    if (aDeath.empty()) aDeath = aIdle;

    // Initialise sprite with first idle frame
    if (!aIdle.empty()) {
        sprite.setTexture(aIdle.get(0));
        auto lb = sprite.getLocalBounds();
        // The texture has exactly 83px of transparent empty space at the bottom.
        sprite.setOrigin(lb.width / 2.f, lb.height - 83.f);
        sprite.setScale(SCALE, SCALE);
    }
    return ok;
}

// ── Stats (called each time a new level starts) ───────────────────────────────
void Player::initStats(int level)
{
    float mult = 1.f;
    for (int i = 1; i < level; ++i) mult *= PLR_HP_MULT;
    maxHP  = PLR_BASE_HP * mult;
    hp     = maxHP;
    damage = PLR_DMG;
}

// ── Per-frame update entry point ──────────────────────────────────────────────
void Player::update(float dt,
                    const std::vector<sf::FloatRect>& platforms,
                    float levelLeft, float levelRight)
{
    if (isDead) return;

    // --- State transitions after one-shot anims ---
    updateState();

    // --- Input (blocked while attacking / hit / dead) ---
    if (state != PState::ATTACK && state != PState::ATTACK2 && state != PState::HIT)
        handleInput(dt);

    // --- Physics + collision ---
    applyPhysics(dt, platforms, levelLeft, levelRight);

    // --- Attack hitbox ---
    attackActive = false;
    if (state == PState::ATTACK || state == PState::ATTACK2) {
        // Active during middle frames (3-6 of 8-frame animation)
        if (frame >= 2 && frame <= 5) {
            attackActive = true;
            updateAttackBox();
        }
    }

    // --- Invincibility frames ---
    if (iTimer > 0.f) iTimer -= dt;

    // --- Animation advance ---
    updateAnim(dt);

    // --- Sprite update ---
    sprite.setTexture(curAnim().get(frame));
    auto lb = sprite.getLocalBounds();
    sprite.setOrigin(lb.width / 2.f, lb.height - 83.f); // subtract exact 83px bottom padding
    sprite.setScale(facingRight ? SCALE : -SCALE, SCALE);
}

// ── Input handling ────────────────────────────────────────────────────────────
void Player::handleInput(float dt)
{
    using K = sf::Keyboard;

    vx = 0.f;

    if (K::isKeyPressed(K::A) || K::isKeyPressed(K::Left)) {
        vx = -MOVE_SPEED;
        facingRight = false;
    }
    if (K::isKeyPressed(K::D) || K::isKeyPressed(K::Right)) {
        vx = MOVE_SPEED;
        facingRight = true;
    }

    // Jump – only from ground
    if ((K::isKeyPressed(K::Space) || K::isKeyPressed(K::Up) ||
         K::isKeyPressed(K::W)) && onGround) {
        vy = JUMP_VEL;
        onGround = false;
        state    = PState::JUMP;
        frame    = 0;
        ftimer   = 0.f;
    }

    // Attack 1 – Left Mouse
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (!isAttacking) {
            isAttacking = true;
            state   = PState::ATTACK;
            frame   = 0;
            ftimer  = 0.f;
            animDone = false;
        }
    }
    // Attack 2 – Right Mouse
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        if (!isAttacking) {
            isAttacking = true;
            state   = PState::ATTACK2;
            frame   = 0;
            ftimer  = 0.f;
            animDone = false;
        }
    }
}

// ── Physics + platform collision ──────────────────────────────────────────────
void Player::applyPhysics(float dt,
                           const std::vector<sf::FloatRect>& platforms,
                           float lLeft, float lRight)
{
    // Apply gravity
    vy += GRAVITY * dt;

    float newX = x + vx * dt;
    float newY = y + vy * dt;

    // --- Ground ---
    if (newY >= GROUND_Y) {
        newY     = GROUND_Y;
        vy       = 0.f;
        onGround = true;
    } else {
        onGround = false;
    }

    // --- Platforms (top-surface only) ---
    for (const auto& plat : platforms) {
        // Only land if approaching from above
        if (vy >= 0.f &&
            y <= plat.top &&
            newY  >= plat.top &&
            newX + PLR_HW > plat.left &&
            newX - PLR_HW < plat.left + plat.width)
        {
            newY     = plat.top;
            vy       = 0.f;
            onGround = true;
        }
    }

    // --- Horizontal bounds ---
    newX = std::max(lLeft  + PLR_HW, newX);
    newX = std::min(lRight - PLR_HW, newX);

    x = newX;
    y = newY;

    // Pick movement state when not in a one-shot anim
    if (state != PState::ATTACK && state != PState::ATTACK2 && state != PState::HIT && state != PState::DEAD)
        setMoveState();
}

// ── Attack hitbox ─────────────────────────────────────────────────────────────
void Player::updateAttackBox()
{
    float bx = facingRight ? x : x - PLR_REACH;
    attackBox = { bx, y - PLR_H, PLR_REACH, PLR_H };
}

// ── Damage ────────────────────────────────────────────────────────────────────
void Player::takeDamage(float dmg)
{
    if (iTimer > 0.f || isDead) return;
    hp -= dmg;
    if (hp <= 0.f) {
        hp    = 0.f;
        state = PState::DEAD;
        frame = 0; ftimer = 0.f; animDone = false;
        isAttacking   = false;
        attackActive  = false;
    } else {
        state  = PState::HIT;
        frame  = 0; ftimer = 0.f; animDone = false;
        iTimer = PLR_IFRAMES;
        isAttacking   = false;
        attackActive  = false;
    }
}

// ── AABB ──────────────────────────────────────────────────────────────────────
sf::FloatRect Player::getBounds() const
{
    return { x - PLR_HW, y - PLR_H, PLR_HW * 2.f, PLR_H };
}

// ── Draw ──────────────────────────────────────────────────────────────────────
void Player::draw(sf::RenderWindow& win, float camX)
{
    // Flash when invincible (alternate visibility)
    if (iTimer > 0.f && static_cast<int>(iTimer * 10.f) % 2 == 0) return;

    sprite.setPosition(x - camX, y);
    win.draw(sprite);
}

// ── Animation helpers ─────────────────────────────────────────────────────────
void Player::updateAnim(float dt)
{
    const AnimData& anim = curAnim();
    if (anim.empty()) return;

    ftimer += dt;
    if (ftimer >= anim.frameTime) {
        ftimer -= anim.frameTime;
        bool oneShot = (state == PState::ATTACK ||
                        state == PState::ATTACK2 ||
                        state == PState::HIT    ||
                        state == PState::DEAD);
        if (oneShot) {
            if (frame < anim.count() - 1)
                ++frame;
            else
                animDone = true;
        } else {
            frame = (frame + 1) % anim.count();
        }
    }
}

void Player::updateState()
{
    if ((state == PState::ATTACK || state == PState::ATTACK2) && animDone) {
        isAttacking = false;
        attackActive = false;
        animDone = false;
        frame = 0; ftimer = 0.f;
        setMoveState();
    }
    if (state == PState::HIT && animDone) {
        animDone = false;
        frame = 0; ftimer = 0.f;
        setMoveState();
    }
    if (state == PState::DEAD && animDone) {
        isDead = true;
    }
}

void Player::setMoveState()
{
    if (!onGround) {
        state = (vy < 0.f) ? PState::JUMP : PState::FALL;
    } else {
        state = (std::abs(vx) > 1.f) ? PState::RUN : PState::IDLE;
    }
}

const AnimData& Player::curAnim() const
{
    switch (state) {
        case PState::RUN:    return aRun;
        case PState::JUMP:   return aJump;
        case PState::FALL:   return aFall;
        case PState::ATTACK: return aAtk;
        case PState::ATTACK2:return aAtk2;
        case PState::HIT:    return aHit;
        case PState::DEAD:   return aDeath;
        default:             return aIdle;
    }
}
