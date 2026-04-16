// ─────────────────────────────────────────────────────────────────────────────
//  Enemy.cpp  –  Skeleton enemy implementation
// ─────────────────────────────────────────────────────────────────────────────
#include "Enemy.hpp"
#include "Constants.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

// ── Static member definitions ─────────────────────────────────────────────────
AnimData Enemy::sIdle;
AnimData Enemy::sWalk;
AnimData Enemy::sAtk;
AnimData Enemy::sHit;
AnimData Enemy::sDeath;
bool     Enemy::sLoaded = false;

// ── Load shared textures (call once before spawning any enemies) ──────────────
bool Enemy::loadSharedAssets()
{
    if (sLoaded) return true;

    sIdle.frameTime = 0.12f;
    sIdle.load("skeleton animation/idle skeleton", "tile", 0, 7, 3);

    // Walk uses non-padded naming: tile0.png … tile9.png
    sWalk.frameTime = 0.08f;
    sWalk.load("skeleton animation/skeleton walk", "tile", 0, 9, 0);

    sAtk.frameTime = 0.07f;
    sAtk.load("skeleton animation/attack1 skeleton", "tile", 0, 9, 3);

    sHit.frameTime = 0.09f;
    sHit.load("skeleton animation/hurt skeleton", "tile", 0, 4, 3);

    sDeath.frameTime = 0.09f;
    sDeath.load("skeleton animation/die skeleton", "tile", 0, 12, 3);

    if (sIdle.empty())
        std::cerr << "[ERROR] Could not load skeleton idle frames\n";

    sLoaded = true;
    return !sIdle.empty();
}

// ── Constructor ───────────────────────────────────────────────────────────────
Enemy::Enemy(float wx, float wy, float maxHp, float dmg, float atkCd)
    : x(wx), y(wy), maxHP(maxHp), hp(maxHp), damage(dmg), atkCooldown(atkCd)
{
    if (!sIdle.empty()) {
        sprite.setTexture(sIdle.get(0));
        auto lb = sprite.getLocalBounds();
        sprite.setOrigin(lb.width / 2.f, lb.height);
        sprite.setScale(SCALE, SCALE);
    }
}

// ── Per-frame update ──────────────────────────────────────────────────────────
void Enemy::update(float dt, float playerX, float playerY)
{
    if (state == EState::DEAD) {
        updateAnim(dt);
        sprite.setTexture(curAnim().get(frame));
        auto lb = sprite.getLocalBounds();
        sprite.setOrigin(lb.width / 2.f, lb.height);
        sprite.setScale(facingRight ? SCALE : -SCALE, SCALE);

        if (animDone) removed = true;
        return;
    }

    atkTimer += dt;
    updateAI(dt, playerX, playerY);
    updateAnim(dt);

    // Update sprite
    sprite.setTexture(curAnim().get(frame));
    auto lb = sprite.getLocalBounds();
    sprite.setOrigin(lb.width / 2.f, lb.height);
    sprite.setScale(facingRight ? SCALE : -SCALE, SCALE);
}

// ── AI ────────────────────────────────────────────────────────────────────────
void Enemy::updateAI(float dt, float playerX, float playerY)
{
    // Do nothing while playing hit or attack animations
    if (state == EState::HIT) {
        if (animDone) {
            state = EState::IDLE;
            frame = 0; ftimer = 0.f; animDone = false;
        }
        return;
    }
    if (state == EState::ATTACK) {
        // Deal damage on frame 4-6 of attack
        atkHitActive = (frame >= 3 && frame <= 5);
        updateAtkBox();
        if (animDone) {
            state = EState::IDLE;
            frame = 0; ftimer = 0.f; animDone = false;
            atkHitActive = false;
        }
        return;
    }

    float dist = std::abs(playerX - x);
    float dx   = playerX - x;

    if (dist <= ENM_ATK_R) {
        // In melee range → attack
        vx = 0.f;
        if (atkTimer >= atkCooldown) {
            atkTimer  = 0.f;
            state     = EState::ATTACK;
            frame     = 0;
            ftimer    = 0.f;
            animDone  = false;
        } else {
            state = EState::IDLE;
        }
    } else if (dist <= ENM_DETECT) {
        // In detection range → chase player
        facingRight = (dx > 0.f);
        vx          = (dx > 0.f) ? ENM_SPEED : -ENM_SPEED;
        state       = EState::WALK;
        x          += vx * dt;
    } else {
        // Out of range → idle patrol (stationary for simplicity)
        vx    = 0.f;
        state = EState::IDLE;
    }
}

void Enemy::updateAtkBox()
{
    float bx = facingRight ? x : x - ENM_ATK_R;
    atkBox = { bx, y - ENM_H, ENM_ATK_R, ENM_H };
}

// ── Damage ────────────────────────────────────────────────────────────────────
void Enemy::takeDamage(float dmg)
{
    if (state == EState::DEAD) return;
    hp -= dmg;
    if (hp <= 0.f) {
        hp       = 0.f;
        state    = EState::DEAD;
        frame    = 0;
        ftimer   = 0.f;
        animDone = false;
        vx       = 0.f;
    } else {
        state    = EState::HIT;
        frame    = 0;
        ftimer   = 0.f;
        animDone = false;
    }
}

// ── AABB ──────────────────────────────────────────────────────────────────────
sf::FloatRect Enemy::getBounds() const
{
    return { x - ENM_HW, y - ENM_H, ENM_HW * 2.f, ENM_H };
}

// ── Draw ──────────────────────────────────────────────────────────────────────
void Enemy::draw(sf::RenderWindow& win, float camX)
{
    sprite.setPosition(x - camX, y);
    win.draw(sprite);
}

// ── Animation helpers ─────────────────────────────────────────────────────────
void Enemy::updateAnim(float dt)
{
    const AnimData& anim = curAnim();
    if (anim.empty()) return;

    ftimer += dt;
    if (ftimer >= anim.frameTime) {
        ftimer -= anim.frameTime;
        bool oneShot = (state == EState::ATTACK ||
                        state == EState::HIT    ||
                        state == EState::DEAD);
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

const AnimData& Enemy::curAnim() const
{
    switch (state) {
        case EState::WALK:   return sWalk.empty() ? sIdle : sWalk;
        case EState::ATTACK: return sAtk.empty()  ? sIdle : sAtk;
        case EState::HIT:    return sHit.empty()   ? sIdle : sHit;
        case EState::DEAD:   return sDeath.empty() ? sIdle : sDeath;
        default:             return sIdle;
    }
}
