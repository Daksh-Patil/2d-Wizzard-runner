# 🧙‍♂️ 2D Wizard Runner

A fast-paced **2D platform runner game** built using **C++ and SFML**, featuring smooth player mechanics, enemy interactions, and engaging level progression. This project demonstrates core game development concepts such as physics, collision detection, and modular object-oriented design.

---

## 🎮 Game Overview

**2D Wizard Runner** is an action-platformer where the player controls a wizard navigating through dynamically designed levels filled with obstacles and enemies. The goal is to survive, defeat enemies, and reach the end of each level.

---

## 🚀 Features

* 🧙‍♂️ Player-controlled wizard character
* 🏃 Smooth movement (run, jump, gravity-based physics)
* 💥 Combat system (attack enemies)
* 💀 Enemy AI with basic behavior
* 🧱 Tile-based level design
* 🚪 Level progression system
* ⚡ Increasing difficulty across levels
* 🧠 Object-Oriented design for scalability

---

## 🧠 Core Concepts Implemented

### 🔹 Game Physics

* Gravity-based movement system
* Jump mechanics using velocity updates
* Frame-based motion handling

### 🔹 Collision Detection

* Implemented using **AABB (Axis-Aligned Bounding Box)**
* Prevents player from passing through platforms
* Handles ground and object interaction

### 🔹 Enemy System

* Enemies move toward the player when in range
* Attack when close
* Health-based combat system

### 🔹 Level Design

* Tile-based level creation
* Structured layout with platforms and obstacles
* Door-based level transitions

---

## 🛠️ Tech Stack

* **Language:** C++
* **Graphics Library:** SFML
* **Paradigm:** Object-Oriented Programming (OOP)

---

## 📂 Project Structure

```
/src        → Game source files  
/include    → Header files  
/assets     → Sprites, textures, tiles  
/bin        → Compiled executable  
```

---

## ⚙️ How to Build & Run

### 🔹 Using g++

```bash
g++ -std=c++11 -I include -c src/*.cpp
g++ *.o -o game -lsfml-graphics -lsfml-window -lsfml-system
./game
```

### 🔹 Using Makefile

```bash
make
./game
```

---

## 🎯 Controls

| Key         | Action     |
| ----------- | ---------- |
| A / ←       | Move Left  |
| D / →       | Move Right |
| Space       | Jump       |
| Mouse / Key | Attack     |

---

## ⚔️ Gameplay Mechanics

* Player navigates through levels while avoiding obstacles
* Enemies chase and attack the player
* Health system determines survival
* Each level increases difficulty
* Objective: reach the end safely

---

## 🔥 Challenges Faced

* Implementing smooth collision detection
* Balancing enemy difficulty
* Handling physics and jump consistency
* Managing game states across levels

---

## 🚀 Future Improvements

* 🎨 Animation system for characters
* 🔊 Sound effects and music
* 🤖 Advanced AI (pathfinding)
* 🧭 Procedural level generation
* 💾 Save/Load system

---

## 👨‍💻 Author

Developed by **Daksh Patil** as a game development project to explore SFML, C++, and real-time game mechanics.

---

## ⭐ Support

If you like this project:

* ⭐ Star the repository
* 🍴 Fork and improve
* 🧠 Suggest features

---
