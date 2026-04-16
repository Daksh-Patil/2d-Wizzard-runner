// ─────────────────────────────────────────────────────────────────────────────
//  Boss.cpp  –  Frost Guardian final boss
// ─────────────────────────────────────────────────────────────────────────────
#include "Boss.hpp"
#include "Constants.hpp"
#include <cmath>
#include <iostream>

// ── Asset loading ─────────────────────────────────────────────────────────────
bool Boss::loadAssets()
{
    aIdle.frameTime = 0.13f;
    aIdle.load("final boss/PNG files/idle", "idle_", 1, 6, 0);

    aWalk.frameTime = 0.08f;
    aWalk.load("final boss/PNG files/walk", "walk_", 1, 10, 0);

    aAtk.frameTime = 0.065f;
    aAtk.load("final boss/PNG files/1_atk", "1_atk_", 1, 14, 0);

    aHit.frameTime = 0.09f;
    aHit.load("final boss/PNG files/take_hit", "take_hit_", 1, 7, 0);

    aDeath.frameTime = 0.1f;
    aDeath.load("final boss/PNG files/death", "death_", 1, 16, 0);

    if (aIdle.empty()) {
        std::cerr << "[ERROR] Could not load boss idle frames\n";
        return false;
    }
    sprite.setTexture(aIdle.get(0));
    auto lb = sprite.getLocalBounds();
    sprite.setOrigin(lb.width / 2.f, lb.height - 18.f);
    sprite.setScale(SCALE, SCALE);
    return true;
}

// ── Stats ─────────────────────────────────────────────────────────────────────
void Boss::initStats(float playerMaxHP, float playerDamage)
{
    maxHP  = playerDamage * 140.f; // 7x the old amount; requires 140 attacks to kill
    hp     = maxHP;
    damage = playerMaxHP * 0.15f;  // Deals 15% of player's total HP
}

void Boss::spawn(float wx, float wy)
{
    x = wx;
    y = wy;
    isDead   = false;
    defeated = false;
    state    = BossState::IDLE;
    frame    = 0;
    ftimer   = 0.f;
    animDone = false;
    atkTimer = 0.f;
}

// ── Per-frame update ──────────────────────────────────────────────────────────
void Boss::update(float dt, float playerX, float playerY)
{
    if (defeated) return;

    if (state == BossState::DEAD) {
        updateAnim(dt);
        sprite.setTexture(curAnim().get(frame));
        auto lb = sprite.getLocalBounds();
        sprite.setOrigin(lb.width / 2.f, lb.height - 18.f);
        sprite.setScale(facingRight ? -SCALE : SCALE, SCALE);
        if (animDone) defeated = true;
        return;
    }

    atkTimer += dt;
    updateAI(dt, playerX, playerY);
    updateAnim(dt);

    sprite.setTexture(curAnim().get(frame));
    auto lb = sprite.getLocalBounds();
    sprite.setOrigin(lb.width / 2.f, lb.height - 18.f);
    sprite.setScale(facingRight ? -SCALE : SCALE, SCALE);
}

// ── AI ────────────────────────────────────────────────────────────────────────
void Boss::updateAI(float dt, float playerX, float playerY)
{
    if (state == BossState::HIT) {
        if (animDone) {
            state = BossState::IDLE;
            frame = 0; ftimer = 0.f; animDone = false;
        }
        return;
    }
    if (state == BossState::ATTACK) {
        atkHitActive = (frame >= 5 && frame <= 9);
        updateAtkBox();
        if (animDone) {
            state = BossState::IDLE;
            frame = 0; ftimer = 0.f; animDone = false;
            atkHitActive = false;
        }
        return;
    }

    float dist = std::abs(playerX - x);
    float dx   = playerX - x;
    facingRight = (dx > 0.f);

    if (dist <= BOSS_ATK_R) {
        vx = 0.f;
        if (atkTimer >= BOSS_ATK_CD) {
            atkTimer = 0.f;
            state    = BossState::ATTACK;
            frame    = 0; ftimer = 0.f; animDone = false;
        } else {
            state = BossState::IDLE;
        }
    } else {
        // Always chase player
        vx    = (dx > 0.f) ? BOSS_SPEED : -BOSS_SPEED;
        state = BossState::WALK;
        x    += vx * dt;
        // Keep boss inside world bounds
        x = std::max(100.f, std::min(LEVEL_W - 100.f, x));
        y = GROUND_Y;
    }
}

void Boss::updateAtkBox()
{
    float bx = facingRight ? x : x - BOSS_ATK_R;
    atkBox = { bx, y - 240.f, BOSS_ATK_R, 240.f };
}

// ── Damage ────────────────────────────────────────────────────────────────────
void Boss::takeDamage(float dmg)
{
    if (state == BossState::DEAD) return;
    hp -= dmg;
    if (hp <= 0.f) {
        hp       = 0.f;
        isDead   = true;
        state    = BossState::DEAD;
        frame    = 0; ftimer = 0.f; animDone = false;
        vx       = 0.f;
    } else {
        state    = BossState::HIT;
        frame    = 0; ftimer = 0.f; animDone = false;
    }
}

// ── AABB ──────────────────────────────────────────────────────────────────────
sf::FloatRect Boss::getBounds() const
{
    // Boss is larger; constants scaled by 2x
    constexpr float BW = 110.f, BH = 240.f;
    return { x - BW, y - BH, BW * 2.f, BH };
}

// ── Draw ──────────────────────────────────────────────────────────────────────
void Boss::draw(sf::RenderWindow& win, float camX)
{
    if (defeated) return;
    sprite.setPosition(x - camX, y);
    win.draw(sprite);
}

// ── Animation helpers ─────────────────────────────────────────────────────────
void Boss::updateAnim(float dt)
{
    const AnimData& anim = curAnim();
    if (anim.empty()) return;

    ftimer += dt;
    if (ftimer >= anim.frameTime) {
        ftimer -= anim.frameTime;
        bool oneShot = (state == BossState::ATTACK ||
                        state == BossState::HIT    ||
                        state == BossState::DEAD);
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

const AnimData& Boss::curAnim() const
{
    switch (state) {
        case BossState::WALK:   return aWalk.empty()  ? aIdle : aWalk;
        case BossState::ATTACK: return aAtk.empty()   ? aIdle : aAtk;
        case BossState::HIT:    return aHit.empty()    ? aIdle : aHit;
        case BossState::DEAD:   return aDeath.empty()  ? aIdle : aDeath;
        default:                return aIdle;
    }
}
