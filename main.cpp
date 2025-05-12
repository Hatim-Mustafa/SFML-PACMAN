// Hatim Check exactly WHY the ghosts are rubbing against the walls 

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "MAP.h"
#include <string>
#include <iostream>
#include <cmath>
#include <queue>
#include <fstream>
#include <sstream>
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
	Pac(Map* gameMap, Texture* texture) : pacman(16.f), velocity(-3.f, 0.f), map(gameMap), premove(""), pacTexture(texture) {
		pacman.setPosition(22 * 15.f + 8.f, 38.f * 15.f + 1.f); // Center in tile
		pacman.setFillColor(Color::Yellow);
		pacman.setOrigin(8.f, 8.f - 45.f); // Center origin

		// yay maine itne piyare koi sprite khud apne hathon se banake daale hain na:)
		pacSprite.setTexture(*pacTexture);
		pacSprite.setTextureRect(IntRect(0, 0, frameSize.x, frameSize.y));
		pacSprite.setOrigin(frameSize.x / 3.5f, frameSize.y / 3.5f);
		pacSprite.setScale(1.f, 1.f);
		pacSprite.setPosition(22 * 15.f + 8.f, 38.f * 15.f + 46.f);


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
			int tempdir = directionRow;
			if (event.key.code == Keyboard::Right) {
				velocity = Vector2f(3.f, 0.f);
				directionRow = 0;
			}
			else if (event.key.code == Keyboard::Left) {
				velocity = Vector2f(-3.f, 0.f);
				directionRow = 1;
			}
			else if (event.key.code == Keyboard::Up) {
				velocity = Vector2f(0.f, -3.f);
				directionRow = 2;
			}
			else if (event.key.code == Keyboard::Down) {
				velocity = Vector2f(0.f, 3.f);
				directionRow = 3;
			}
			Vector2f checkVelocity(velocity.x * 7, velocity.y * 7);
			Vector2f newPosition = pacman.getPosition() + checkVelocity;
			if (willCollide(newPosition)) {
				premove = direction();
				velocity = temp;
				directionRow = tempdir;
			}
			else {
				premove = "";
			}
		}
	}

	void handlePremove(std::string dir) {
		if (dir != "") {
			Vector2f temp = velocity;
			int tempdir = directionRow;
			if (dir == "Right") {
				velocity = Vector2f(3.f, 0.f);
				directionRow = 0;
			}
			else if (dir == "Left") {
				velocity = Vector2f(-3.f, 0.f);
				directionRow = 1;
			}
			else if (dir == "Up") {
				velocity = Vector2f(0.f, -3.f);
				directionRow = 2;
			}
			else if (dir == "Down") {
				velocity = Vector2f(0.f, 3.f);
				directionRow = 3;
			}
			Vector2f checkVelocity(velocity.x * 7, velocity.y * 7);
			Vector2f newPosition = pacman.getPosition() + checkVelocity;
			if (willCollide(newPosition)) {
				velocity = temp;
				directionRow = tempdir;
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
		pacSprite.setPosition(pacPos.x, pacPos.y + 45.f);

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
		if (!font.loadFromFile("C:/Users/admin/Desktop/Project/pac2/Emulogic-zrEw.ttf")) {
			std::cout << "could not open file" << std::endl;
		}
		scoreText.setFont(font);
		scoreText.setCharacterSize(24);
		scoreText.setFillColor(Color::White);
		scoreText.setPosition(0.f, 10.f);
	}

	void Score() {
		
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
		float halfSize = 16.f;
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
		speed = 1.25f;
		releaseClock.restart();
	}



	virtual void draw(RenderWindow& window) {
		window.draw(ghost);
	}

	virtual void update() {
		Vector2f newPos = ghost.getPosition() + velocity;

		// Check if we can continue in current direction
		if (!willCollide(newPos)) {
			ghost.move(velocity);
			position = ghost.getPosition();
		}
		else {
	
			std::vector<Vector2f> possibleDirections = {
				Vector2f(speed, 0),    // Right
				Vector2f(-speed, 0),    // Left
				Vector2f(0, speed),     // Down
				Vector2f(0, -speed)     // Up
			};

			
			for (auto& dir : possibleDirections) {
				if (!willCollide(ghost.getPosition() + dir)) {
					velocity = dir;
					break;
				}
			}
		}
	}

	virtual void MoveGhost(Pac& pac) = 0; 

	virtual ~Ghost() {}
	Vector2f getPosition() const { return position; }

	bool checkCollisionWithPac(Pac& pac) const {
		Vector2f ghostPos = ghost.getPosition();
		Vector2f pacPos = pac.getP();
		float pacRadius = pac.getR();
		float ghostHalfSize = 16.f; 

		
		float closestX = std::max(ghostPos.x - ghostHalfSize, std::min(pacPos.x, ghostPos.x + ghostHalfSize));
		float closestY = std::max(ghostPos.y - ghostHalfSize, std::min(pacPos.y, ghostPos.y + ghostHalfSize));

		float distanceX = pacPos.x - closestX;
		float distanceY = pacPos.y - closestY;

		return (distanceX * distanceX + distanceY * distanceY) < (pacRadius * pacRadius);
	}

};

class Blinky : public Ghost {
private:
	sf::Sprite blinkySprite;
	sf::Texture* blinkyTex;
public:
	Blinky(Map* map, Texture* blinkyTex) : Ghost(map, Color(255, 0, 0, 255), 22.5, 18.5, Vector2f(-1.5f, 0.f), 4.f), blinkyTex(blinkyTex) {
		blinkySprite.setTexture(*blinkyTex);

		blinkySprite.setOrigin(16.f, 16.f);

		blinkySprite.setScale(1.f / 30.f, 1.f / 30.f);
		blinkySprite.setPosition(22.5f * 15.f - 17.f, 18.f * 15.f + 45.f - 13.f);
	}

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

			velocity = dir * 1.5f;
			ghost.move(velocity);
		}
	}
	void update() override {
		Vector2f newPos = ghost.getPosition() + velocity;

		if (!willCollide(newPos)) {
			ghost.move(velocity);
			position = ghost.getPosition();
		}
		else {
			std::vector<Vector2f> possibleDirections = {
				{speed, 0}, {-speed, 0}, {0, speed}, {0, -speed}
			};

			for (auto& dir : possibleDirections) {
				if (!willCollide(ghost.getPosition() + dir)) {
					velocity = dir;
					break;
				}
			}
		}
		blinkySprite.setPosition(ghost.getPosition().x - 17.f, ghost.getPosition().y + 45.f - 13.f);
	}

	void draw(sf::RenderWindow& window) override {
		//window.draw(ghost);
		window.draw(blinkySprite);
	}

};

class Pinky : public Ghost {
private:
	bool released = false;
	sf::Clock releaseClock;
	sf::Sprite pinkySprite;
	sf::Texture* pinkyTex;

public:
	Pinky(Map* map, sf::Texture* pinkyTex)
		: Ghost(map, sf::Color(246, 87, 214), 22.5, 23, Vector2f(0.f, 0.f), 7.f),
		pinkyTex(pinkyTex)
	{
		pinkySprite.setTexture(*pinkyTex);

		pinkySprite.setOrigin(16.f, 16.f);

		pinkySprite.setScale(1.f, 1.f);
		pinkySprite.setPosition(22.5f * 15.f, 23.f * 15.f + 45.f);
	}

	void MoveGhost(Pac& pac) override {
		if (releaseClock.getElapsedTime().asSeconds() < releaseDelay)
			return;

		Vector2f startPos = ghost.getPosition();
		Vector2f pacPos = pac.getP();
		Vector2f pacVel = pac.getVelocity();
		Vector2f predictedPos = pacPos + pacVel * 4.f;

		std::vector<Vector2f> path = bfs(startPos, predictedPos);

		if (!path.empty()) {
			Vector2f next = path[0];
			Vector2f dir = next - startPos;
			float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);

			if (length != 0) dir /= length;
			velocity = dir * 0.70f;
			ghost.move(velocity);
		}
	}

	void update() override {
		Vector2f newPos = ghost.getPosition() + velocity;

		if (!willCollide(newPos)) {
			ghost.move(velocity);
			position = ghost.getPosition();
		}
		else {
			std::vector<Vector2f> possibleDirections = {
				{speed, 0}, {-speed, 0}, {0, speed}, {0, -speed}
			};

			for (auto& dir : possibleDirections) {
				if (!willCollide(ghost.getPosition() + dir)) {
					velocity = dir;
					break;
				}
			}
		}
		pinkySprite.setPosition(ghost.getPosition().x + 10.f, ghost.getPosition().y + 45.f - 10.f);
	}

	void draw(sf::RenderWindow& window) override {
		//window.draw(ghost);
		window.draw(pinkySprite);
	}
};


class Inky : public Ghost {
private:
	Blinky* blinky; // Need reference to Blinky for targeting
	bool released = false;
	Sprite inkySprite;
	Texture* inkyTex;


public:
	Inky(Map* map, Blinky* blinkyRef, Texture* inkyTex)
		: Ghost(map, Color::Cyan, 19.5, 23, Vector2f(0.f, 1.5f), 10.f), blinky(blinkyRef), inkyTex(inkyTex) {
		inkySprite.setTexture(*inkyTex);

		inkySprite.setOrigin(16.f, 16.f);

		inkySprite.setScale(1.f / 18.f, 1.f / 18.f);
		inkySprite.setPosition(19.5f * 15.f - 20.f, 23.f * 15.f + 45.f - 20.f);

	}

	void MoveGhost(Pac& pac) override {
		if (releaseClock.getElapsedTime().asSeconds() < releaseDelay)
			return;

		Vector2f startPos = ghost.getPosition();
		Vector2f pacPos = pac.getP();
		Vector2f pacVel = pac.getVelocity();

		
		if (pacVel.x == 0 && pacVel.y == 0) {
			switch (pac.directionRow) {  
			case 0: pacVel = Vector2f(3, 0); break;   // Right
			case 1: pacVel = Vector2f(-3, 0); break;  // Left
			case 2: pacVel = Vector2f(0, -3); break;  // Up
			case 3: pacVel = Vector2f(0, 3); break;   // Down
			}
		}

		
		Vector2f twoAhead = pacPos + pacVel * 15.f;  
		Vector2f blinkyPos = blinky->getPosition();

		Vector2f targetPos = blinkyPos + 2.f * (twoAhead - blinkyPos);

		targetPos.x = std::max(15.f, std::min(targetPos.x, 660.f));  
		targetPos.y = std::max(15.f, std::min(targetPos.y, 750.f));  


		Vector2f adjustedTarget = findClosestReachable(targetPos);
		std::vector<Vector2f> path = bfs(startPos, adjustedTarget);

		if (!path.empty()) {
			Vector2f next = path[0];
			Vector2f dir = next - startPos;
			float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
			if (length > 0) {
				velocity = (dir / length) * 1.25f;
				ghost.move(velocity);
			}
		}
	}

	void update() override {
		Vector2f newPos = ghost.getPosition() + velocity;

		if (!willCollide(newPos)) {
			ghost.move(velocity);
			position = ghost.getPosition();
		}
		else {
			std::vector<Vector2f> possibleDirections = {
				{speed, 0}, {-speed, 0}, {0, speed}, {0, -speed}
			};

			for (auto& dir : possibleDirections) {
				if (!willCollide(ghost.getPosition() + dir)) {
					velocity = dir;
					break;
				}
			}
		}
		inkySprite.setPosition(ghost.getPosition().x - 20.f, ghost.getPosition().y + 45.f - 20.f);
	}

	void draw(sf::RenderWindow& window) override {
		//window.draw(ghost);
		window.draw(inkySprite);
	}

private:
	Vector2f findClosestReachable(const Vector2f& target) {
		int targetCol = static_cast<int>(target.x / 15.f);
		int targetRow = static_cast<int>(target.y / 15.f);


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


		return Vector2f(25 * 15.f + 8.f, 23 * 15.f + 8.f);
	}
};

class Clyde : public Ghost {
private:
	Vector2f scatterCorner; // Bottom-left corner (0, mapHeight-1)
	Sprite clydeSprite;
	Texture* clydeTex;

public:
	Clyde(Map* map, Texture* clydeTex) : Ghost(map, Color(255, 165, 0), 25.5, 23, Vector2f(0.f, -1.5f), 13.f), clydeTex(clydeTex) {
		scatterCorner = Vector2f(40.f, (map->getRows() - 3) * 15.f + 8.f);

		clydeSprite.setTexture(*clydeTex);

		clydeSprite.setOrigin(16.f, 16.f);

		clydeSprite.setScale(1.f / 16.f, 1.f / 16.f);
		clydeSprite.setPosition(25.5f * 15.f - 15.f, 23.f * 15.f + 45.f - 15.f);
	}

	float calculateDistance(const Vector2f& pos1, const Vector2f& pos2) {
		float dx = pos1.x - pos2.x;
		float dy = pos1.y - pos2.y;
		return std::sqrt(dx * dx + dy * dy);
	}

	void update() override {
		Vector2f newPos = ghost.getPosition() + velocity;

		if (!willCollide(newPos)) {
			ghost.move(velocity);
			position = ghost.getPosition();
		}
		else {
			std::vector<Vector2f> possibleDirections = {
				{speed, 0}, {-speed, 0}, {0, speed}, {0, -speed}
			};

			for (auto& dir : possibleDirections) {
				if (!willCollide(ghost.getPosition() + dir)) {
					velocity = dir;
					break;
				}
			}
		}
		clydeSprite.setPosition(ghost.getPosition().x - 20.f, ghost.getPosition().y + 45.f - 20.f);
	}

	void draw(sf::RenderWindow& window) override {
		//window.draw(ghost);
		window.draw(clydeSprite);
	}

	void MoveGhost(Pac& pac) override {

		if (releaseClock.getElapsedTime().asSeconds() < releaseDelay)
			return;

		Vector2f startPos = ghost.getPosition();
		Vector2f pacPos = pac.getP();


		float distance = calculateDistance(startPos, pacPos);

		Vector2f targetPos;
		if (distance > 120.f) { 
			targetPos = pacPos;
		}
		else { 
			targetPos = scatterCorner;
		}

		std::vector<Vector2f> path = bfs(startPos, targetPos);

		if (!path.empty()) {
			Vector2f next = path[0];
			Vector2f dir = next - startPos;

			float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
			if (length != 0)
				dir /= length;

			velocity = dir * 1.5f;
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
	FoodManager* khana;
	int livesLeft;
	std::fstream file;
	Text HighScore;

public:
	GameSimulation(FoodManager* f) : gameOver(false), gameWon(false), livesLeft(3), khana(f) {
		if (!buffer1.loadFromFile("C:/Users/admin/Desktop/Project/pac2/pacman_beginning.wav")) {
			std::cout << "could not open file" << std::endl;
		}
		beginningSound.setBuffer(buffer1);

		if (!buffer2.loadFromFile("C:/Users/admin/Desktop/Project/pac2/pacman_chomp.wav")) {
			std::cout << "could not open file" << std::endl;
		}
		chompSound.setBuffer(buffer2);

		if (!buffer3.loadFromFile("C:/Users/admin/Desktop/Project/pac2/pacman_death.wav")) {
			std::cout << "could not open file" << std::endl;
		}
		deathSound.setBuffer(buffer3);

		if (!font.loadFromFile("C:/Users/admin/Desktop/Project/pac2/Emulogic-zrEw.ttf")) {
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

		//set up highscore text
		HighScore.setFont(font);
		HighScore.setCharacterSize(24);
		HighScore.setFillColor(Color::White);
		HighScore.setPosition(290.f, 10.f);
	}

	void HighScoreFunc() {
		std::ifstream file("score.txt");
		if (!file) {
			HighScore.setString("High Score: 0");
			return;
		}
		int hs = 0;
		int score;
		std::string s;
		while (std::getline(file, s)) {
			score = std::stoi(s);
			if (hs < score) {
				hs = score;
			}
		}
		HighScore.setString("High Score: " + std::to_string(hs));
	}

	bool isGameOver() const { return gameOver; }
	bool isGameWon() const { return gameWon; }

	void reset() {
		gameOver = false;
		gameWon = false;
		livesLeft = 3;
	}

	void checkGameOver(const Ghost& ghost1, const Ghost& ghost2, const Ghost& ghost3, const Ghost& ghost4, Pac& pac, int currentScore, int totalFood) {
		// Check for ghost collision 
		if (ghost1.checkCollisionWithPac(pac) || ghost2.checkCollisionWithPac(pac) || ghost3.checkCollisionWithPac(pac) || ghost4.checkCollisionWithPac(pac))
		{
			if (livesLeft == 1) {
				deathSound.play();
				while (deathSound.getStatus() == Sound::Playing) {}
				gameOver = true;
				gameWon = false;
				file.open("score.txt", std::ios::app);
				file << khana->getScore() << std::endl;

			}
			else {
				deathSound.play();
				while (deathSound.getStatus() == Sound::Playing) {}
				livesLeft--;
				pac.setInitialPosition();
				pac.setInitialVelocity();
			}
		}
		// Check if all food eaten 
		if (currentScore == 1840) {
			gameOver = true;
			gameWon = true;
			file.open("score.txt", std::ios::app);
			file << khana->getScore() << std::endl;
		}
	}

	void draw(RenderWindow& window) {
		HighScoreFunc();
		window.draw(HighScore);
		for (int i = 0; i < livesLeft - 1; i++) {
			Sprite life;
			Texture lifeTex;

			if (!lifeTex.loadFromFile("C:/Users/admin/Desktop/Project/pac2/pacman.png")) {
				std::cout << "Failed to load Pac-Man sprite!" << std::endl;
			}

			life.setTexture(lifeTex);
			life.setScale(1.f / 15.f, 1.f / 15.f);
			life.setPosition(i * (40) + 10, 817);
			window.draw(life);
		}
		if (gameOver) {
			
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
	if (!pacTexture.loadFromFile("C:/Users/admin/Desktop/Project/pac2/pacman10.png")) {
		std::cout << "Failed to load Pac-Man sprite!" << std::endl;
	}

	sf::Texture blinkyTex;
	if (!blinkyTex.loadFromFile("C:/Users/admin/Desktop/Project/pac2/blinky.png")) {
		std::cout << "Failed to load PinkySprite sprite!" << std::endl;
	}

	sf::Texture pinkyTex;
	if (!pinkyTex.loadFromFile("C:/Users/admin/Desktop/Project/pac2/pinkysprite.png")) {
		std::cout << "Failed to load PinkySprite sprite!" << std::endl;
	}

	sf::Texture inkyTex;
	if (!inkyTex.loadFromFile("C:/Users/admin/Desktop/Project/pac2/inky.png")) {
		std::cout << "Failed to load PinkySprite sprite!" << std::endl;
	}

	sf::Texture clydeTex;
	if (!clydeTex.loadFromFile("C:/Users/admin/Desktop/Project/pac2/clyde.png")) {
		std::cout << "Failed to load PinkySprite sprite!" << std::endl;
	}



	Map map;
	Pac* pac = new Pac(&map, &pacTexture); 
	Blinky* shadow = new Blinky(&map, &blinkyTex);
	Clyde* pokey = new Clyde(&map, &clydeTex);  //pookie ghost hehe :ribbon (why are you adding dumb comments to this very serious project of ours seniya  -zaid)
	Pinky* speedy = new Pinky(&map, &pinkyTex);
	Inky* bashful = new Inky(&map, shadow, &inkyTex);

	Clock clock;

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

	GameSimulation game(manage);

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
					
					delete pac;
					delete shadow;
					delete speedy;
					delete bashful;
					delete pokey;
					delete[] f;
					delete manage;

					
					pac = new Pac(&map, &pacTexture);
					shadow = new Blinky(&map, &blinkyTex);
					speedy = new Pinky(&map, &pinkyTex);
					bashful = new Inky(&map, shadow, &inkyTex);
					pokey = new Clyde(&map, &clydeTex);

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
			game.playChompSound();
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
