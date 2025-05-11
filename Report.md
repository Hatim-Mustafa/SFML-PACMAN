# Project Report: Pacman Game Using OOP and SFML

### Group Members:
- Hatim Mustafa (Leader) – 24K-0673  
- Zaid Ahmed – 24K-0500  
- Fuzail Raza – 24K-0699  
- Seniya Naeem – 24K-0608  

**Submission Date:** 12th May, 2025

---

## 1. Executive Summary

### Project Overview
This project involved the development of a fully-functional Pacman game clone using Object-Oriented Programming (OOP) principles and the Simple and Fast Multimedia Library (SFML) for graphics rendering. The implementation focused on recreating core gameplay mechanics including character movement, ghost AI behaviors, collision systems, and game state management.

### Key Achievements
- Successfully implemented Pacman movement with keyboard controls  
- Developed Ghosts with authentic pathfinding  
- Created a tile-based map system with collision detection  
- Established food collection mechanics with visual feedback  
- Win and Lose logic  
- Implemented Sprites  
- Implemented Audio  
- Implemented version control and collaborative development using GitHub  

### Technical Highlights
- Utilized advanced pathfinding algorithms (BFS) for ghost AI  
- Applied OOP principles: encapsulation, inheritance, polymorphism, and abstraction  
- Overcame significant learning curves with SFML library

---

## 2. Introduction

### Background
The Pacman game was selected as it provides an excellent platform to demonstrate:

- Practical application of OOP concepts  
- Game development fundamentals  
- Algorithm implementation  
- Team collaboration in software development  

### Project Objectives
- Implement Pacman character with keyboard-controlled movement  
- Develop four distinct ghost behaviors (Blinky, Pinky, Inky, Clyde)  
- Create a functional scoring system  
- Implement game state management (win/lose conditions)  
- Build a tile-based map system with collision detection

---

## 3. Project Description

### Completed Features

**Pacman Character**  
- Grid-based movement  
- Keyboard-controlled using arrow keys  
- Collision detection  
- Visual representation using SFML shapes  

**Ghost Behaviors**  
- Blinky: Direct chase behavior using BFS  
- Pinky: Ambush behavior (targets 4 tiles ahead)  
- Inky: Unpredictable, offset-based targeting  
- Clyde: Mixed behavior, switches modes based on proximity  

**Game World**  
- Hardcoded 51×46 tile map  
- Food pellet placement and collection  
- Score system  
- Tunnel warp  
- Sprite-based graphics  

**Development Infrastructure**  
- GitHub for version control  
- Modular code organization  

**Game State Management**  
- Win if all food eaten  
- Lose if Pacman collides with ghosts 3 times

### Technical Specifications
- **Development Environment:** Visual Studio 2022  
- **Graphics Library:** SFML 2.6.0  
- **Programming Language:** C++17  
- **Version Control:** GitHub

---

## 4. Methodology

### Approach

**Planning Phase:**  
- Requirement analysis  
- Class diagram design  
- Task allocation  

**Implementation Phase:**  
- Weekly sprints  
- Frequent meetings  
- GitHub-based integration  

**Testing Phase:**  
- Unit testing  
- Integration testing  

### Team Responsibilities

| Member      | Primary Responsibilities                   |
|-------------|---------------------------------------------|
| Hatim       | SFML setup, Pacman movement, GitHub, collision & audio |
| Fuzail      | Ghost AI & pathfinding                     |
| Zaid        | Food system, collision detection, scoring  |
| Seniya      | Map class, ghost logic, documentation      |

---

## 5. Implementation

### Design and Structure

- **Pacman Class:** Handles movement, wall collision, and rendering  
- **Ghost Hierarchy:** Inheritance for different behaviors  
- **Map System:** 2D array of tiles (0 = wall, 1 = path, 2 = food)  
- **Food System:** Collision detection and score tracking  
- **Game Loop:** SFML event + fixed time updates  
- **Reset System:** Smart pointers and centralized control  

### Functionalities Developed

**Pacman:**
- Grid-aligned movement, smooth turns, tunnel warp, wall collision
- Sprite visuals

**Ghosts:**
- Blinky: Direct pathfinding  
- Pinky: Predictive ambush  
- Inky: Offset + unpredictable behavior  
- Clyde: Distance-based chase + scatter

**Game World:**
- 51×46 grid  
- SFML-based food pellets  
- Score and feedback  
- Win/Lose simulation  

---

## Technical Challenges Faced

1. **SFML Learning Curve:** Solved through self-study  
2. **Pathfinding:** BFS implemented for two ghosts  
3. **Code Organization:** Solved using header files and modular classes  
4. **Collision Detection with Food:** Grid-based solution  
5. **Pacman Centering:** Solved by rounding instead of flooring  
6. **Ghost Wall Collision:** Unsolved vibration issue  
7. **Sprites:** Created in Photoshop, aligned properly  
8. **Reset Logic:** Smart pointers and centralized `GameSimulation` class

---

## 6. Results

### Outcomes

- Fully functional gameplay prototype  
- OOP principles demonstrated clearly  
- SFML + algorithm experience gained  

### Testing

- Validated after each implementation  
- Unit + integration tests

---

## 7. Conclusion

This project successfully recreated the Pacman game using C++ and SFML while applying OOP principles such as encapsulation, inheritance, polymorphism, and abstraction. Core systems like pathfinding, score tracking, collision detection, and state management were developed with accuracy. The team learned SFML from scratch and tackled complex problems using collaboration and structured coding. The final product is a polished prototype demonstrating technical capability and software development discipline.


