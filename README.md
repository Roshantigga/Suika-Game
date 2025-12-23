# Suika-Game
# 🍉 Suika Game (Watermelon Game) – Raylib C++

A faithful **Suika (Watermelon) Game clone** built using **C++ and Raylib**.  
The game follows the original Suika rules: click to drop fruits, merge identical fruits to create bigger ones, and avoid overflowing the container.

---

## 🎮 Gameplay Rules

- Click anywhere inside the container to **drop a fruit**
- Only **one fruit can fall at a time**
- Fruits merge **only when two identical fruits collide**
- Merging creates a **bigger fruit**
- Score increases **once per merge**
- Game ends when a **settled fruit crosses the top danger line**
- The **next fruit preview** is shown on the top-right

This implementation avoids common bugs like:
- Negative score overflow
- Multiple merges per frame
- Fruits disappearing incorrectly

---

## 🖥️ Screen Details

- Resolution: **540 × 760**
- Larger play area for smoother gameplay
- Visual next-fruit preview

---

## 📁 Project Structure
Suika-Game-Raylib/
│
├── main.cpp
├── README.md
└── assets/
├── circle0.png
├── circle1.png
├── circle2.png
├── circle3.png
├── circle4.png
├── circle5.png
├── circle6.png
├── circle7.png
├── circle8.png
├── circle9.png
└── circle10.png

## To run this project simply click on main Application after cloning or downloading the project or in terminal in same directory run ./main
