#include <SFML/Graphics.hpp>
#include <stack>
#include <vector>
#include <random>
#include <iostream>
using namespace std;

#define SIZE 30
#define CELL_WIDTH 20

class Cell {
public:
    int x = 0, y = 0;
    int pos = 0;
    float size = CELL_WIDTH;
    float thickness = 2.f;
    bool walls[4] = { true, true, true, true };
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

        if (walls[0]) {
            rect.setSize({ size, thickness });
            rect.setPosition(x, y);
            window.draw(rect);
        }
        if (walls[1]) {
            rect.setSize({ thickness, size });
            rect.setPosition(x + size, y);
            window.draw(rect);
        }
        if (walls[2]) {
            rect.setSize({ size + thickness, thickness });
            rect.setPosition(x, y + size);
            window.draw(rect);
        }
        if (walls[3]) {
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

class Lobby {
private:
    sf::RenderWindow &window;
    sf::Font font;
    sf::Text title;
    sf::Text start;
    sf::Text exit;
    bool startgame;


            

public:

    Lobby(sf::RenderWindow &window):window(window) {
        startgame = false;
        cout << "Welcome to the Maze Game! " << endl;;
        cout << "Use arrow keys or HJKL to navigate"<<endl;
        cout << "Reach the bottom right corner to win! "<<endl;


		if (!font.loadFromFile("Data/Roboto.ttf")) {
			std::cout << "Error loading font" << std::endl;
		}
		title.setFont(font);
		title.setString("Maze Game");
		title.setCharacterSize(50);
		title.setFillColor(sf::Color::White);
		title.setPosition(window.getSize().x/2 -title.getGlobalBounds().width/2,100);

		start.setFont(font);
		start.setString("Press Enter to Start");
		start.setCharacterSize(30);
		start.setFillColor(sf::Color::White);
		start.setPosition(window.getSize().x/2-start.getGlobalBounds().width/2,300);

		exit.setFont(font);
		exit.setString("Press Escape to Exit");
		exit.setCharacterSize(30);
		exit.setFillColor(sf::Color::White);
		exit.setPosition(window.getSize().x/2-exit.getGlobalBounds().width/2, 400);





    }   
	bool run() {

        while (window.isOpen()) {
            handlevent();
            render();
            if (startgame) {
			
                return true;
            }
           
        }
        return false;
	
	}

	void handlevent() {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Enter) {
					startgame = true;
				}
				else if (event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
			}
		}
	}

    void render() {
		window.clear(sf::Color(13, 2, 33));
		window.draw(title);
		window.draw(start);
		window.draw(exit);
		window.display();

    }
};

class Game {
private:
    sf::Clock clock;
    sf::Time elapsedTime;
    sf::Font font;
    sf::Text timerText;
    sf::Clock freezeClock;

    int bestTime = -1;
    int countdownSeconds = 60;
    bool countdownExpired = false;
    bool showingGameOver = false;

    sf::Text bestTimeText;
    sf::Text countdownText;
    sf::Text gameOverText;

    sf::RenderWindow &window;
    Maze maze;
    int currentPos = 0;

    sf::RectangleShape currentHighlight;
    sf::RectangleShape goalHighlight;

public:
    Game(sf::RenderWindow &win) : window(win),
        maze(SIZE) {

        if (!font.loadFromFile("Data/Roboto.ttf")) {
            std::cout << "Error loading font" << std::endl;
        }

        gameOverText.setFont(font);
        gameOverText.setString("Game Over");
        gameOverText.setCharacterSize(60);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setStyle(sf::Text::Bold);
        gameOverText.setPosition(200, 200);

        timerText.setFont(font);
        timerText.setCharacterSize(20);
        timerText.setFillColor(sf::Color::White);
        timerText.setPosition(30, 5);

        bestTimeText.setFont(font);
        bestTimeText.setCharacterSize(20);
        bestTimeText.setFillColor(sf::Color::Yellow);
        bestTimeText.setPosition(150, 5);

        countdownText.setFont(font);
        countdownText.setCharacterSize(20);
        countdownText.setFillColor(sf::Color::Red);
        countdownText.setPosition(300, 5);

       
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

            if (event.type == sf::Event::KeyPressed && !showingGameOver)
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
        if (showingGameOver) {
            if (freezeClock.getElapsedTime().asSeconds() > 3.0f) {
                restartGame();
            }
            return;
        }

        if (currentPos == SIZE * SIZE - 1) {
            int elapsed = static_cast<int>(clock.getElapsedTime().asSeconds());

            if (bestTime == -1 || elapsed < bestTime)
                bestTime = elapsed;

            restartGame();
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
            showingGameOver = true;
            freezeClock.restart();
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

        if (showingGameOver) {
            window.draw(gameOverText);
        }

        window.display();
    }

    void restartGame() {
        maze.generateMaze();
        currentPos = 0;
        maze.getCell(currentPos).isActive = true;
        clock.restart();
        countdownExpired = false;
        showingGameOver = false;
    }
};

int main() {  
sf::RenderWindow window(sf::VideoMode(CELL_WIDTH * SIZE + 60, CELL_WIDTH * SIZE + 60), "Maze Game");  
   window.setFramerateLimit(60);  
Lobby lobby(window);  
if (lobby.run()) { 
	Game game(window);  
	game.run();  
}  
else {  
	return 0;  
}  
}
