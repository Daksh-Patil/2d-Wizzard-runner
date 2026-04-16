#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  Constants.hpp  –  Global game-wide constants
// ─────────────────────────────────────────────────────────────────────────────

// ── Window ───────────────────────────────────────────────────────────────────
constexpr int   WIN_W         = 1280;
constexpr int   WIN_H         = 720;

// ── Physics ──────────────────────────────────────────────────────────────────
constexpr float GRAVITY       = 1900.0f;  // px / s²
constexpr float JUMP_VEL      = -760.0f;  // initial jump velocity (negative = up)
constexpr float MOVE_SPEED    = 285.0f;   // horizontal px / s

// ── World ────────────────────────────────────────────────────────────────────
// The camera scrolls horizontally only; world-Y == screen-Y
constexpr float GROUND_Y      = 600.0f;  // world-Y of ground top surface (tiles fill 600→720)
constexpr float LEVEL_W       = 7000.0f; // total horizontal world width

// ── Player stats ─────────────────────────────────────────────────────────────
constexpr float PLR_BASE_HP   = 100.0f;
constexpr float PLR_HP_MULT   = 1.25f;   // HP multiplier per level
constexpr float PLR_DMG       = 6.25f;   // attack damage
constexpr float PLR_REACH     = 88.0f;   // horizontal attack reach (px)
constexpr float PLR_IFRAMES   = 0.55f;   // invincibility seconds after hit
constexpr float PLR_H         = 76.0f;   // approx player sprite height (screen px)
constexpr float PLR_HW        = 26.0f;   // approx player half-width  (screen px)

// ── Enemy stats ──────────────────────────────────────────────────────────────
constexpr float ENM_BASE_HP   = 60.0f;
constexpr float ENM_HP_MULT   = 1.15f;
constexpr float ENM_DMG       = 15.0f;
constexpr float ENM_SPEED     = 118.0f;  // px / s
constexpr float ENM_DETECT    = 400.0f;  // agro range (px)
constexpr float ENM_ATK_R     = 68.0f;   // melee attack range (px)
constexpr float ENM_ATK_CD    = 1.5f;    // attack cooldown (s)
constexpr float ENM_H         = 62.0f;   // approx enemy height
constexpr float ENM_HW        = 22.0f;   // approx enemy half-width

// Enemy counts per level (index 0 = level 1)
constexpr int   ENEMY_COUNTS[5] = { 10, 15, 20, 25, 30 };

// ── Boss stats ───────────────────────────────────────────────────────────────
constexpr float BOSS_SPEED    = 145.0f;
constexpr float BOSS_ATK_R    = 230.0f;
constexpr float BOSS_ATK_CD   = 1.8f;

// ── System font ──────────────────────────────────────────────────────────────
inline const char* SYS_FONT   = "C:/Windows/Fonts/arial.ttf";
