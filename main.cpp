// Hatim Check exactly WHY the ghosts are rubbing against the walls 

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Map.h"
#include <string>
#include <iostream>
#include <cmath>
#include <queue>
#include <map>
#include <set>

using namespace sf;

class Pac;
class Ghost;
class Pinky;
class Blinky;
class Clyde;
class Inky;
int const khaana = 184;


// A rather Interesting Concept To
//template <typename T>
//T clamp(T value, T min, T max) {
//	return (value < min) ? min : (value > max) ? max : value;
//}

class GameState {
public:
	static bool isGameOver;
	static bool isPaused;
};

bool GameState::isGameOver = false;
bool GameState::isPaused = false;

class Pac {
private:
	CircleShape pacman;
	Sprite pacSprite;
	Texture* pacTexture;
	Vector2f velocity;
	Map* map;
	std::string premove;

	// Animation variables
	int currentFrame = 0;
	float frameTimer = 0.f;
	float frameDelay = 0.1f;
	Vector2i frameSize = { 32, 32 };
	int totalFrames = 3; 
	int directionRow = 1; // 0: right, 1: left, 2: up, 3: down

public:
	Pac(Map* gameMap, Texture *texture) : pacman(16.f), velocity(-3.f, 0.f), map(gameMap), premove(""),pacTexture(texture) {
		pacman.setPosition(22 * 15.f + 8.f, 38.f * 15.f); // Center in tile
		pacman.setFillColor(Color::Yellow);
		pacman.setOrigin(8.f, 8.f - 45.f); // Center origin

		pacSprite.setTexture(*pacTexture);
		pacSprite.setTextureRect(IntRect(0, 0, frameSize.x, frameSize.y));
		pacSprite.setOrigin(frameSize.x / 3.5f, frameSize.y / 3.5f);
		pacSprite.setScale(1.f, 1.f); 


	}

void setInitialPosition() {
	pacman.setPosition(22 * 15.f + 8.f, 38.f * 15.f);
}

void setInitialVelocity() {
	velocity = Vector2f(-3.f, 0.f);
}

	void draw(RenderWindow& window) {
		//window.draw(pacman);
		window.draw(pacSprite);
	}

	float getR() {
		return pacman.getRadius();
	}

	Vector2f getP() {
		return pacman.getPosition();
	}

	void handleInput(Event& event) {
		if (event.type == Event::KeyPressed) {
			Vector2f temp = velocity;
			if (event.key.code == Keyboard::Right) {
				velocity = Vector2f(2.f, 0.f);
				directionRow = 0;
			}
			else if (event.key.code == Keyboard::Left) {
				velocity = Vector2f(-2.f, 0.f);
				directionRow = 1;
			}
			else if (event.key.code == Keyboard::Up) {
				velocity = Vector2f(0.f, -2.f);
				directionRow = 2;
			}
			else if (event.key.code == Keyboard::Down) {
				velocity = Vector2f(0.f, 2.f);
				directionRow = 3;
			}
			Vector2f checkVelocity(velocity.x * 7, velocity.y * 7);
			Vector2f newPosition = pacman.getPosition() + checkVelocity;
			if (willCollide(newPosition)) {
				premove = direction();
				velocity = temp;
			}
			else {
				premove = "";
			}
		}
	}

	void handlePremove(std::string dir) {
		if (dir != "") {
			Vector2f temp = velocity;
			if (dir == "Right") {
				velocity = Vector2f(2.f, 0.f);
				directionRow = 0;
			}
			else if (dir == "Left") {
				velocity = Vector2f(-2.f, 0.f);
				directionRow = 1;
			}
			else if (dir == "Up") {
				velocity = Vector2f(0.f, -2.f);
				directionRow = 2;
			}
			else if (dir == "Down") {
				velocity = Vector2f(0.f, 2.f);
				directionRow = 3;
			}
			Vector2f checkVelocity(velocity.x * 7, velocity.y * 7);
			Vector2f newPosition = pacman.getPosition() + checkVelocity;
			if (willCollide(newPosition)) {
				velocity = temp;
			}
			else {
				premove = "";
			}
		}
	}

	void teleport(Vector2f& newPosition) {
		if (newPosition.x < 10) {
			pacman.setPosition(660, newPosition.y);
			pacman.move(velocity);
			newPosition = pacman.getPosition();
		}
		else if (newPosition.x > 660) {
			pacman.setPosition(10, newPosition.y);
			pacman.move(velocity);
			newPosition = pacman.getPosition();
		}
	}

	void update(float deltaTime) {
		
		Vector2f pacPos = pacman.getPosition();
		pacSprite.setPosition(pacPos.x,pacPos.y + 45.f);

		//checking for premove
		handlePremove(premove);

		// Predict new position
		Vector2f newPosition = pacman.getPosition() + velocity;

		teleport(newPosition);


		// Check collision in movement direction
		if (!willCollide(newPosition)) {
			pacman.move(velocity);
		}

		frameTimer += deltaTime;
		if (frameTimer >= frameDelay) {
			frameTimer -= frameDelay;
			currentFrame = (currentFrame + 1) % totalFrames;
			pacSprite.setTextureRect(IntRect(
				currentFrame * frameSize.x,
				directionRow * frameSize.y,
				frameSize.x,
				frameSize.y
			));
		}
	}

	// I added this getVelocity function here

	Vector2f getVelocity() {
		return velocity;
	}

private:
	std::string direction() const {
		if (velocity.x > 0) return "Right";
		else if (velocity.x < 0) return "Left";
		else if (velocity.y > 0) return "Down";
		else if (velocity.y < 0) return "Up";
		return "None";
	}

	bool willCollide(const Vector2f& newPosition) {
		// Get the 8 edges of Pacman's collision circle
		float radius = pacman.getRadius();
		Vector2f edges[8] = {
			Vector2f(newPosition.x - radius, newPosition.y - radius), // top-left
			Vector2f(newPosition.x + radius, newPosition.y - radius), // top-right
			Vector2f(newPosition.x - radius, newPosition.y + radius), // bottom-left
			Vector2f(newPosition.x + radius, newPosition.y + radius), // bottom-right
			Vector2f(newPosition.x - radius, newPosition.y), //left
			Vector2f(newPosition.x + radius, newPosition.y), //right
			Vector2f(newPosition.x, newPosition.y - radius), //top
			Vector2f(newPosition.x, newPosition.y + radius), //bottom
		};

		// Check each edge against walls
		for (const auto& edge : edges) {
			int col = static_cast<int>(round(edge.x / 15.f));
			int row = static_cast<int>(round(edge.y / 15.f));
			if (map->isWall(row, col)) {
				return true;
			}
		}
		return false;
	}

	friend class Pinky;
	friend class Inky;
	friend class Clyde;
	friend class Blinky;
};

class Food {
	CircleShape f;
	Map* fmap;
	Pac* pman;
	int rows, cols;

public:

	bool isEaten;

	Food() {}

	Food(Map* fmap, Pac* pman, int rows, int cols) :isEaten(false), fmap(fmap), pman(pman), rows(rows), cols(cols), f(3.f) {

		f.setFillColor(sf::Color::White);
		f.setPosition(cols * 15.f + 2.5f, rows * 15.f + 2.5f + 45.f);
	}

	bool willEat(const Vector2f& newPosition) {

		float radius = pman->getR();
		Vector2f edges[8] = {
			Vector2f(newPosition.x - radius, newPosition.y - radius), // top-left
			Vector2f(newPosition.x + radius, newPosition.y - radius), // top-right
			Vector2f(newPosition.x - radius, newPosition.y + radius), // bottom-left
			Vector2f(newPosition.x + radius, newPosition.y + radius), // bottom-right
			Vector2f(newPosition.x - radius, newPosition.y), //left
			Vector2f(newPosition.x + radius, newPosition.y), //right
			Vector2f(newPosition.x, newPosition.y - radius), //top
			Vector2f(newPosition.x, newPosition.y + radius), //bottom
		};

		// Check each edge against food
		for (const auto& edge : edges) {
			int col = static_cast<int>(round((edge.x + 3.f) / 15.f));
			int row = static_cast<int>(round((edge.y + 5.f) / 15.f));
			if (row == rows && col == cols) {
				return true;
			}
		}
		return false;
	}

	friend class FoodManager;

};

class FoodManager {
private:
	Food* food;
	Map* fmap;
	Pac* pman;
	Vector2f velocity;
	int score = 0;
	Font font;
	Text scoreText;


public:
	FoodManager(Food* food, Map* fmap, Pac* pman) : food(food), fmap(fmap), pman(pman), velocity(-3.f, 0.f) {
		if (!font.loadFromFile("C:/Users/Dell/OneDrive/Desktop/Hatim/OOP/SFML-PACMAN/Emulogic-zrEw.ttf")) {
			std::cout << "could not open file" << std::endl;
		}

		scoreText.setFont(font);
		scoreText.setCharacterSize(26);
		scoreText.setFillColor(Color::White);
		scoreText.setPosition(0.f, 10.f);
	}

	void Score() {
		// Predict new position
		Vector2f newPosition = pman->getP() + velocity;

		pman->teleport(newPosition);


		// update score when food is eaten
		for (int k = 0; k < khaana; ++k) {
			if (food[k].willEat(newPosition) && !food[k].isEaten) {
				score += 10;
				food[k].isEaten = true;
				scoreText.setString("Score: " + std::to_string(score));
			}
		}
	}
	void draw(RenderWindow& window) const {
		for (int k = 0; k < khaana; ++k) {
			if (!food[k].isEaten) {
				window.draw(food[k].f);
			}
		}

		window.draw(scoreText);
	}
	int getScore() const { return score; }
};


class Ghost {
protected:
	RectangleShape ghost;
	Vector2f position;
	Color color;
	Vector2f velocity;
	float speed;
	float releaseDelay;
	sf::Clock releaseClock;
	Map* map;
	bool isScared = false;

	// BFS LOGIC WHICH I UNDERSTOOD (I should be graded additional marks in DSA) ~Fuzail Raza

	std::vector<Vector2f> bfs(const Vector2f& start, const Vector2f& target) {
		int startCol = static_cast<int>(start.x / 15.f);
		int startRow = static_cast<int>(start.y / 15.f);
		int targetCol = static_cast<int>(target.x / 15.f);
		int targetRow = static_cast<int>(target.y / 15.f);

		std::queue<std::pair<int, int>> q;
		std::map<std::pair<int, int>, std::pair<int, int>> parent;
		std::vector<std::vector<bool>> visited(51, std::vector<bool>(46, false));

		q.push({ startRow, startCol });
		visited[startRow][startCol] = true;

		int dr[4] = { -1, 1, 0, 0 };
		int dc[4] = { 0, 0, -1, 1 };

		while (!q.empty()) {
			std::pair<int, int> current = q.front(); q.pop();
			int r = current.first;
			int c = current.second;

			if (r == targetRow && c == targetCol)
				break;

			for (int i = 0; i < 4; i++) {
				int nr = r + dr[i];
				int nc = c + dc[i];

				if (nr >= 0 && nr < 51 && nc >= 0 && nc < 46 && !map->isWall(nr, nc) && !visited[nr][nc]) {
					visited[nr][nc] = true;
					q.push({ nr, nc });
					parent[{nr, nc}] = { r, c };
				}
			}
		}

		std::vector<Vector2f> path;
		std::pair<int, int> current = { targetRow, targetCol };

		while (current != std::make_pair(startRow, startCol)) {
			path.push_back(Vector2f(current.second * 15.f + 8.f, current.first * 15.f + 8.f));
			if (parent.find(current) == parent.end()) break;
			current = parent[current];
		}

		std::reverse(path.begin(), path.end());
		return path;
	}


	bool willCollide(const Vector2f& newPosition) const {
		float halfSize = 8.f;
		Vector2f edges[4] = {
			Vector2f(newPosition.x - halfSize, newPosition.y - halfSize),
			Vector2f(newPosition.x + halfSize, newPosition.y - halfSize),
			Vector2f(newPosition.x - halfSize, newPosition.y + halfSize),
			Vector2f(newPosition.x + halfSize, newPosition.y + halfSize)
		};

		for (const auto& edge : edges) {
			int col = static_cast<int>(edge.x / 15.f);
			int row = static_cast<int>(edge.y / 15.f);
			if (map->isWall(row, col)) {
				return true;
			}
		}
		return false;
	}

public:

	Ghost(Map* gameMap, Color ghostColor, float startX, float startY, Vector2f initialVelocity, float delay = 0.f)
		: map(gameMap), color(ghostColor), velocity(initialVelocity), releaseDelay(delay) {
		ghost.setSize(Vector2f(32.f, 32.f));
		ghost.setFillColor(ghostColor);
		position = Vector2f(startX * 15.f, startY * 15.f);
		ghost.setPosition(position);
		ghost.setOrigin(16.f, 16.f - 45.f);
		speed = 0.75f;
		releaseClock.restart();
	}



	void draw(RenderWindow& window) {
		window.draw(ghost);
	}

	void update() {
		Vector2f newPos = ghost.getPosition() + velocity;

		// Check if we can continue in current direction
		if (!willCollide(newPos)) {
			ghost.move(velocity);
			position = ghost.getPosition();
		}
		else {
			// When hitting a wall, find a new direction from possible moves
			std::vector<Vector2f> possibleDirections = {
				Vector2f(speed, 0),    // Right
				Vector2f(-speed, 0),    // Left
				Vector2f(0, speed),     // Down
				Vector2f(0, -speed)     // Up
			};

			// Try all directions and pick one that doesn't collide
			for (auto& dir : possibleDirections) {
				if (!willCollide(ghost.getPosition() + dir)) {
					velocity = dir;
					break;
				}
			}
		}
	}

	virtual void MoveGhost(Pac& pac) = 0; // pure virtual

	virtual ~Ghost() {}
	Vector2f getPosition() const { return position; }

	bool checkCollisionWithPac(Pac& pac) const {
		Vector2f ghostPos = ghost.getPosition();
		Vector2f pacPos = pac.getP();
		float pacRadius = pac.getR();
		float ghostHalfSize = 16.f; // Since ghost size is 32x32

		// Simple rectangle-circle collision detection
		float closestX = std::max(ghostPos.x - ghostHalfSize, std::min(pacPos.x, ghostPos.x + ghostHalfSize));
		float closestY = std::max(ghostPos.y - ghostHalfSize, std::min(pacPos.y, ghostPos.y + ghostHalfSize));

		float distanceX = pacPos.x - closestX;
		float distanceY = pacPos.y - closestY;

		return (distanceX * distanceX + distanceY * distanceY) < (pacRadius * pacRadius);
	}
};

class Blinky : public Ghost {
private:
public:
	Blinky(Map* map) : Ghost(map, Color(255, 0, 0, 255), 22.5, 18.5, Vector2f(-1.5f, 0.f), 4.f) {}

	void MoveGhost(Pac& pac) override {

		if (releaseClock.getElapsedTime().asSeconds() < releaseDelay)
			return;

		Vector2f startPos = ghost.getPosition();
		Vector2f targetPos = pac.getP();

		std::vector<Vector2f> path = bfs(startPos, targetPos);

		if (!path.empty()) {
			Vector2f next = path[0];
			Vector2f dir = next - startPos;

			float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
			if (length != 0)
				dir /= length;

			velocity = dir * 0.75f;
			ghost.move(velocity);
		}
	}

};

class Pinky : public Ghost {
private:
	bool released = false;
	sf::Clock releaseClock;

public:
	Pinky(Map* map) : Ghost(map, Color(246, 87, 214), 22.5, 23, Vector2f(0.f, 0.f), 7.f) {
	}

	void MoveGhost(Pac& pac) override {

		if (releaseClock.getElapsedTime().asSeconds() < releaseDelay)
			return;

		Vector2f startPos = ghost.getPosition();
		Vector2f pacPos = pac.getP();
		Vector2f pacVel = pac.getVelocity();

		// Took everything from PacMan cuz uss nay mayray liyay kya chora tha </3

		Vector2f predictedPos = pacPos + pacVel * 4.f;

		// Predicted pos is basically 4 tiles ahead of where the pacman is facing 

		std::vector<Vector2f> path = bfs(startPos, predictedPos);

		if (!path.empty()) {
			Vector2f next = path[0];
			Vector2f dir = next - startPos;

			float length = std::sqrt(dir.x * dir.x + dir.y * dir.y); // Distance formula to check if it is 0 or nah
			if (length != 0) {
				dir /= length;
			}

			velocity = dir * 0.70f;  // Apparantely Pinky slower than Blinky
			ghost.move(velocity);
		}
	}


};

class Inky : public Ghost {
private:
	Blinky* blinky; // Need reference to Blinky for targeting
	bool released = false;

public:
	Inky(Map* map, Blinky* blinkyRef)
		: Ghost(map, Color::Cyan, 19.5, 23, Vector2f(0.f, 1.5f), 10.f), blinky(blinkyRef) {
	}

	void MoveGhost(Pac& pac) override {

		if (releaseClock.getElapsedTime().asSeconds() < releaseDelay)
			return;

		Vector2f startPos = ghost.getPosition();
		Vector2f pacPos = pac.getP();
		Vector2f pacVel = pac.getVelocity();

		// Step 1: Find tile two spaces ahead of Pac-Man
		Vector2f twoAhead = pacPos + pacVel * 30.f; // 2 tiles * 15px per tile

		// Step 2: Get Blinky's position
		Vector2f blinkyPos = blinky->getPosition();

		// Step 3: Draw vector from Blinky to twoAhead tile
		Vector2f vectorToDouble = twoAhead - blinkyPos;

		// Step 4: Double the vector
		Vector2f targetPos = blinkyPos + 2.f * vectorToDouble;

		// If target is out of bounds, adjust it
		targetPos.x = std::max(0.f, std::min(targetPos.x, 690.f));
		targetPos.y = std::max(0.f, std::min(targetPos.y, 765.f + 45.f));

		// Find path to target
		std::vector<Vector2f> path = bfs(startPos, targetPos);

		if (!path.empty()) {
			Vector2f next = path[0];
			Vector2f dir = next - startPos;

			float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
			if (length != 0)
				dir /= length;

			velocity = dir * 0.75f;
			ghost.move(velocity);
		}
		else {
			// If no path found (target in wall), try to get as close as possible
			// Find the closest reachable position to target
			Vector2f adjustedTarget = findClosestReachable(targetPos);
			path = bfs(startPos, adjustedTarget);

			if (!path.empty()) {
				Vector2f next = path[0];
				Vector2f dir = next - startPos;

				float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
				if (length != 0)
					dir /= length;

				velocity = dir * 0.75f;
				ghost.move(velocity);
			}
		}
	}

private:
	Vector2f findClosestReachable(const Vector2f& target) {
		int targetCol = static_cast<int>(target.x / 15.f);
		int targetRow = static_cast<int>(target.y / 15.f);

		// Check in increasing radius around target
		for (int radius = 1; radius < 10; radius++) {
			for (int r = targetRow - radius; r <= targetRow + radius; r++) {
				for (int c = targetCol - radius; c <= targetCol + radius; c++) {
					if (r >= 0 && r < map->getRows() && c >= 0 && c < map->getCols()) {
						if (!map->isWall(r, c)) {
							return Vector2f(c * 15.f + 8.f, r * 15.f + 8.f);
						}
					}
				}
			}
		}

		// Fallback to a default position if nothing found (shouldn't happen)
		return Vector2f(25 * 15.f + 8.f, 23 * 15.f + 8.f);
	}
};

class Clyde : public Ghost {
private:
	Vector2f scatterCorner; // Bottom-left corner (0, mapHeight-1)

public:
	Clyde(Map* map) : Ghost(map, Color(255, 165, 0), 25.5, 23, Vector2f(0.f, -1.5f), 13.f) {
		scatterCorner = Vector2f(40.f, (map->getRows() - 3) * 15.f + 8.f);
	}

	float calculateDistance(const Vector2f& pos1, const Vector2f& pos2) {
		float dx = pos1.x - pos2.x;
		float dy = pos1.y - pos2.y;
		return std::sqrt(dx * dx + dy * dy);
	}

	void MoveGhost(Pac& pac) override {

		if (releaseClock.getElapsedTime().asSeconds() < releaseDelay)
			return;

		Vector2f startPos = ghost.getPosition();
		Vector2f pacPos = pac.getP();

		// Calculate distance in pixels (8 tiles = 8*15 = 120 pixels)
		float distance = calculateDistance(startPos, pacPos);

		Vector2f targetPos;
		if (distance > 120.f) { // More than 8 tiles away - chase Pac-Man
			targetPos = pacPos;
		}
		else { // Within 8 tiles - retreat to scatter corner
			targetPos = scatterCorner;
		}

		std::vector<Vector2f> path = bfs(startPos, targetPos);

		if (!path.empty()) {
			Vector2f next = path[0];
			Vector2f dir = next - startPos;

			float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
			if (length != 0)
				dir /= length;

			velocity = dir * 0.75f;
			ghost.move(velocity);
		}
	}
};


class GameSimulation {
	SoundBuffer buffer1;
	Sound beginningSound;
	SoundBuffer buffer2;
	Sound chompSound;
	SoundBuffer buffer3;
	Sound deathSound;
	bool gameOver;
	bool gameWon;
	Font font;
	Text gameOverText;
	Text winText;
	Text restartText;
	int livesLeft;

public:
	GameSimulation() : gameOver(false), gameWon(false), livesLeft(3) {
		if (!buffer1.loadFromFile("C:/Users/Dell/OneDrive/Desktop/Hatim/OOP/SFML-PACMAN/pacman_beginning.wav")) {
			std::cout << "could not open file" << std::endl;
		}
		beginningSound.setBuffer(buffer1);

		if (!buffer2.loadFromFile("C:/Users/Dell/OneDrive/Desktop/Hatim/OOP/SFML-PACMAN/pacman_chomp.wav")) {
			std::cout << "could not open file" << std::endl;
		}
		chompSound.setBuffer(buffer2);

		if (!buffer3.loadFromFile("C:/Users/Dell/OneDrive/Desktop/Hatim/OOP/SFML-PACMAN/pacman_death.wav")) {
			std::cout << "could not open file" << std::endl;
		}
		deathSound.setBuffer(buffer3);

		if (!font.loadFromFile("C:/Users/Dell/OneDrive/Desktop/Hatim/OOP/SFML-PACMAN/Emulogic-zrEw.ttf")) {
			std::cout << "Error loading font" << std::endl;
		}

		// Set up game over text
		gameOverText.setFont(font);
		gameOverText.setString("GAME OVER");
		gameOverText.setCharacterSize(48);
		gameOverText.setFillColor(Color::Red);

		// Set up win text
		winText.setFont(font);
		winText.setString("YOU WIN!");
		winText.setCharacterSize(48);
		winText.setFillColor(Color::Green);

		// Set up restart text
		restartText.setFont(font);
		restartText.setString("Press any key to restart");
		restartText.setCharacterSize(24);
		restartText.setFillColor(Color::White);
	}

	bool isGameOver() const { return gameOver; }
	bool isGameWon() const { return gameWon; }

	void reset() {
		gameOver = false;
		gameWon = false;
		livesLeft = 3;
	}

	void checkGameOver(const Ghost& ghost1, const Ghost& ghost2, const Ghost& ghost3, const Ghost& ghost4, Pac& pac, int currentScore, int totalFood) {
		// Check for ghost collision (lose condition)
		if (ghost1.checkCollisionWithPac(pac) || ghost2.checkCollisionWithPac(pac) || ghost3.checkCollisionWithPac(pac) || ghost4.checkCollisionWithPac(pac))
		{
			if (livesLeft == 1) {
				deathSound.play();
				while (deathSound.getStatus() == Sound::Playing) {}
				gameOver = true;
				gameWon = false;
			}
			else {
				deathSound.play();
				while(deathSound.getStatus() == Sound::Playing) {}
				livesLeft--;
				pac.setInitialPosition();
				pac.setInitialVelocity();
			}
		}
		// Check if all food eaten (win condition)
		if (currentScore == 1840) {
			gameOver = true;
			gameWon = true;
		}
	}

	void draw(RenderWindow& window) {
		for (int i = 0; i < livesLeft - 1; i++) {
			CircleShape life(16.f);
			life.setFillColor(Color::Yellow);
			life.setPosition(i *(40)+10, 817);
			window.draw(life);
		}
		if (gameOver) {
			// Center the texts
			if (gameWon) {
				winText.setPosition(
					window.getSize().x / 2 - winText.getLocalBounds().width / 2,
					window.getSize().y / 2 - winText.getLocalBounds().height / 2
				);
				window.draw(winText);
			}
			else {
				gameOverText.setPosition(
					window.getSize().x / 2 - gameOverText.getLocalBounds().width / 2,
					window.getSize().y / 2 - gameOverText.getLocalBounds().height / 2
				);
				window.draw(gameOverText);
			}

			restartText.setPosition(
				window.getSize().x / 2 - restartText.getLocalBounds().width / 2,
				window.getSize().y / 2 + 60
			);
			window.draw(restartText);
		}
	}

	void playBeginningSound() {
		beginningSound.play();
	}

	void playChompSound() {
		if (chompSound.getStatus() != Sound::Playing) {
			chompSound.play();
		}
		else {
		}
	}	
};


int main() {

	RenderWindow window(VideoMode(690, 765 + 90), "PAC-MAN");
	window.setFramerateLimit(60);


	sf::Texture pacTexture;
	if (!pacTexture.loadFromFile("C:/Users/umera/Downloads/pacman10.png")) {
		std::cout << "Failed to load Pac-Man sprite!" << std::endl;
	}
	
	Map map;
	Pac* pac = new Pac(&map, &pacTexture); // Use pointers for clean reset
	Blinky* shadow = new Blinky(&map);
	Clyde* pokey = new Clyde(&map);  //pookie ghost hehe :ribbon (why are you adding dumb comments to this very serious project of ours seniya  -zaid)
	Pinky* speedy = new Pinky(&map);
	Inky* bashful = new Inky(&map, shadow);

	Clock clock;

	GameSimulation game;

	sf::Clock startDelayClock;
	bool delayOver = false;

	Food* f = new Food[khaana];
	int foodIndex = 0;
	for (int i = 0; i < 51; ++i) {
		for (int j = 0; j < 46; ++j) {
			if (map.isFood(i, j) && foodIndex < khaana) {
				f[foodIndex] = Food(&map, pac, i, j);
				++foodIndex;
			}
		}
	}
	FoodManager* manage = new FoodManager(f, &map, pac);

	game.playBeginningSound();

	while (window.isOpen()) {
		if (startDelayClock.getElapsedTime().asSeconds() > 4.0f) {
			delayOver = true;
		}

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();
			else if (event.type == Event::KeyPressed) {
				if (game.isGameOver()) {
					// Delete old objects
					delete pac;
					delete shadow;
					delete speedy;
					delete bashful;
					delete pokey;
					delete[] f;
					delete manage;

					// Reinitialize everything
					pac = new Pac(&map,&pacTexture);
					shadow = new Blinky(&map);
					speedy = new Pinky(&map);
					bashful = new Inky(&map, shadow);
					pokey = new Clyde(&map);

					foodIndex = 0;
					f = new Food[khaana];
					for (int i = 0; i < 51; ++i) {
						for (int j = 0; j < 46; ++j) {
							if (map.isFood(i, j) && foodIndex < khaana) {
								f[foodIndex] = Food(&map, pac, i, j);
								++foodIndex;
							}
						}
					}
					manage = new FoodManager(f, &map, pac);
					game.reset();
				}
				else {
					pac->handleInput(event);
				}
			}
		}
		if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
			window.close();
		}

		if (!game.isGameOver() && delayOver) {
			pac->update(clock.restart().asSeconds());
			manage->Score();

			// Ghost movement
			shadow->MoveGhost(*pac); // Blinky chases Pac-Man
			speedy->MoveGhost(*pac); // Pinky bbg traps Pac-Man
			bashful->MoveGhost(*pac); //unpredictable like a tsundere gf chasing pacman
			pokey->MoveGhost(*pac); //pookie ghost gets scared if too close to pacman like zaid and women

			shadow->update();
			speedy->update();
			bashful->update();
			pokey->update();

			game.checkGameOver(*shadow, *speedy, *bashful, *pokey, *pac,
				manage->getScore(), khaana);
		}
		window.clear(Color::Black);
		map.draw(window);
		pac->draw(window);
		manage->draw(window);
		pokey->draw(window);
		speedy->draw(window);
		bashful->draw(window);
		shadow->draw(window);
		game.draw(window);
		window.display();
	}

	delete pac;
	delete shadow;
	delete speedy;
	delete bashful;
	delete pokey;
	delete[] f;
	delete manage;

	return 0;
}
