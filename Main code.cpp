#include <SFML/Graphics.hpp>
#include <stack>
#include <vector>
#include <random>
#include <iostream>

#define SIZE 30
#define CELL_WIDTH 20

class Cell {
public:
    int x = 0, y = 0;
    int pos = 0;
    float size = CELL_WIDTH;
    float thickness = 2.f;
    bool walls[4] = { true, true, true, true }; // top, right, bottom, left
    bool visited = false;
    bool isActive = false;

    Cell() {}
    Cell(int _x, int _y, int _pos) : x(_x), y(_y), pos(_pos) {}

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape rect;

        if (isActive) {
            rect.setFillColor(sf::Color(247, 23, 53));
            rect.setSize({ size, size });
            rect.setPosition(x, y);
            window.draw(rect);
        }

        rect.setFillColor(sf::Color(223, 243, 228));

        if (walls[0]) { // top
            rect.setSize({ size, thickness });
            rect.setPosition(x, y);
            window.draw(rect);
        }
        if (walls[1]) { // right
            rect.setSize({ thickness, size });
            rect.setPosition(x + size, y);
            window.draw(rect);
        }
        if (walls[2]) { // bottom
            rect.setSize({ size + thickness, thickness });
            rect.setPosition(x, y + size);
            window.draw(rect);
        }
        if (walls[3]) { // left
            rect.setSize({ thickness, size });
            rect.setPosition(x, y);
            window.draw(rect);
        }
    }
};

class Maze {
private:
    std::vector<Cell> cells;
    int size;

    void reset() {
        for (auto& cell : cells) {
            for (int i = 0; i < 4; i++) cell.walls[i] = true;
            cell.visited = false;
            cell.isActive = false;
        }
    }

    void removeWalls(Cell& current, Cell& neighbor) {
        int diff = current.pos - neighbor.pos;
        if (diff == size) {
            current.walls[0] = false; neighbor.walls[2] = false;
        }
        else if (diff == -1) {
            current.walls[1] = false; neighbor.walls[3] = false;
        }
        else if (diff == -size) {
            current.walls[2] = false; neighbor.walls[0] = false;
        }
        else if (diff == 1) {
            current.walls[3] = false; neighbor.walls[1] = false;
        }
    }

public:
    Maze(int _size) : size(_size) {
        cells.resize(size * size);
        for (int row = 0, k = 0; row < size; row++) {
            for (int col = 0; col < size; col++, k++) {
                int x = 30 + col * CELL_WIDTH;
                int y = 30 + row * CELL_WIDTH;
                cells[k] = Cell(x, y, k);
            }
        }
        generateMaze();
    }

    void generateMaze() {
        reset();
        std::stack<Cell*> stack;
        cells[0].visited = true;
        stack.push(&cells[0]);

        while (!stack.empty()) {
            Cell* current = stack.top();
            stack.pop();

            int pos = current->pos;
            std::vector<int> neighbors;

            if (pos % size != 0 && !cells[pos - 1].visited) neighbors.push_back(pos - 1);
            if ((pos + 1) % size != 0 && pos + 1 < size * size && !cells[pos + 1].visited) neighbors.push_back(pos + 1);
            if (pos + size < size * size && !cells[pos + size].visited) neighbors.push_back(pos + size);
            if (pos - size >= 0 && !cells[pos - size].visited) neighbors.push_back(pos - size);

            if (!neighbors.empty()) {
                stack.push(current);
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dist(0, neighbors.size() - 1);
                int nextPos = neighbors[dist(gen)];

                Cell* neighbor = &cells[nextPos];
                removeWalls(*current, *neighbor);
                neighbor->visited = true;
                stack.push(neighbor);
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        for (auto& cell : cells) cell.draw(window);
    }

    Cell& getCell(int index) { return cells[index]; }

    int getSize() const { return size; }
};

class Game {
private:

    sf::Clock clock;
    sf::Time elapsedTime;
    sf::Font font;
    sf::Text timerText;

   
    int bestTime = -1; // -1 means no best time yet
    int countdownSeconds = 60; // 60-second countdown
    bool countdownExpired = false;

    
    sf::Text bestTimeText;
    sf::Text countdownText;


    sf::RenderWindow window;
    Maze maze;
    int currentPos = 0;

    sf::RectangleShape currentHighlight;
    sf::RectangleShape goalHighlight;

public:
    Game() : window(sf::VideoMode(CELL_WIDTH* SIZE + 60, CELL_WIDTH* SIZE + 60), "OOP Maze"),
        maze(SIZE) {

		if (!font.loadFromFile("Data/Roboto.ttf")) {
			std::cout << "Error loading font" << std::endl;
		}

        timerText.setFont(font);
        timerText.setCharacterSize(20);
        timerText.setFillColor(sf::Color::White);
        timerText.setPosition(30, 5); // Position near top-left

        bestTimeText.setFont(font);
        bestTimeText.setCharacterSize(20);
        bestTimeText.setFillColor(sf::Color::Yellow);
        bestTimeText.setPosition(150, 5);

        countdownText.setFont(font);
        countdownText.setCharacterSize(20);
        countdownText.setFillColor(sf::Color::Red);
        countdownText.setPosition(300, 5);


       

        window.setFramerateLimit(30);
        maze.getCell(currentPos).isActive = true;

        currentHighlight.setSize({ CELL_WIDTH, CELL_WIDTH });
        currentHighlight.setFillColor(sf::Color(166, 207, 213));

        goalHighlight.setSize({ CELL_WIDTH, CELL_WIDTH });
        goalHighlight.setFillColor(sf::Color(0, 128, 0));
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }

private:
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
                handleMovement(event.key.code);
        }
    }

    void handleMovement(sf::Keyboard::Key key) {
        Cell& current = maze.getCell(currentPos);
        int size = maze.getSize();

        int next = -1;
        if ((key == sf::Keyboard::Left || key == sf::Keyboard::H) && !current.walls[3]) next = currentPos - 1;
        else if ((key == sf::Keyboard::Right || key == sf::Keyboard::L) && !current.walls[1]) next = currentPos + 1;
        else if ((key == sf::Keyboard::Up || key == sf::Keyboard::K) && currentPos >= size && !current.walls[0]) next = currentPos - size;
        else if ((key == sf::Keyboard::Down || key == sf::Keyboard::J) && currentPos + size < size * size && !current.walls[2]) next = currentPos + size;

        if (next >= 0 && next < size * size) {
            current.isActive = false;
            currentPos = next;
            maze.getCell(currentPos).isActive = true;
        }
    }

    void update() {
        if (currentPos == SIZE * SIZE - 1) {
            maze.generateMaze();
            currentPos = 0;
            maze.getCell(currentPos).isActive = true;
            clock.restart();
        }

        elapsedTime = clock.getElapsedTime();
        int seconds = static_cast<int>(elapsedTime.asSeconds());
		int remainingTime = countdownSeconds - seconds;
        timerText.setString("Time: " + std::to_string(seconds) + "s");


        if (bestTime != -1)
            bestTimeText.setString("Best: " + std::to_string(bestTime) + "s");
        else
            bestTimeText.setString("Best: --");

        if (remainingTime >= 0)
            countdownText.setString("Countdown: " + std::to_string(remainingTime) + "s");
        else {
            countdownText.setString("Time's up!");
            countdownExpired = true;
        }
        if (currentPos == SIZE * SIZE - 1) {
            int elapsed = static_cast<int>(clock.getElapsedTime().asSeconds());

            if (bestTime == -1 || elapsed < bestTime)
                bestTime = elapsed;

            maze.generateMaze();
            currentPos = 0;
            clock.restart();
            countdownExpired = false;
            maze.getCell(currentPos).isActive = true;
        }
		if (countdownExpired) {
			countdownText.setString("Time's up!");
			countdownText.setFillColor(sf::Color::Red);
		}
		else {
			countdownText.setFillColor(sf::Color::Red);
		}

    }

    void render() {
        window.clear(sf::Color(13, 2, 33));
        maze.draw(window);
        currentHighlight.setPosition(maze.getCell(currentPos).x, maze.getCell(currentPos).y);
        window.draw(currentHighlight);
        goalHighlight.setPosition(maze.getCell(SIZE * SIZE - 1).x, maze.getCell(SIZE * SIZE - 1).y);
        window.draw(goalHighlight);
		window.draw(timerText);
        window.draw(bestTimeText);
        window.draw(countdownText);
        window.display();
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
