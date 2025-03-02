#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <random>
#include <chrono>
#include <thread>
#include <cmath>
#include "SFML_CLASSES.h" 

int rng(int min, int max) {
    static std::mt19937 gen(std::random_device{}());
    return std::uniform_int_distribution<int>{min, max}(gen);
}

class Tile : public Object {
public:
    int xGridPos;
    int yGridPos;

    Tile(int xGrid, int yGrid, int baseX, int baseY)
        : Object("Sprites/Tiles/empty_tile.png", (xGrid + baseX) * 16.0f, (yGrid + baseY) * 16.0f, 8, 8,2,2) {
        xGridPos = xGrid; yGridPos = yGrid;
    }
};

std::string empty_tile_path = "Sprites/Tiles/empty_tile.png";
std::string tile1_path = "Sprites/Tiles/tile1.png";

Texture empty_tile(empty_tile_path);
Texture tile1(tile1_path);
Texture wall("Sprites/Tiles/wall.png");

Sound MoveSound("SoundEffects/move.wav");
Sound PlaceSound("SoundEffects/place.wav");
Sound RotateSound("SoundEffects/rotate.wav");
Sound ClearLineSound("SoundEffects/single.wav");
Sound TetrisClearSound("SoundEffects/tetris.wav");

Object Gui("Sprites/UI.png", 0, 0, 0, 0, 2.0f, 2.0f);

std::vector<std::pair<int, int>> LShape = {
    {0, 0}, {0, 1}, {0, 2}, {1, 2}
};

std::vector<std::pair<int, int>> JShape = {
    {0, 0}, {0, 1}, {0, 2}, {-1, 2}
};

std::vector<std::pair<int, int>> OShape = {
    {0, 0}, {0, 1}, {1, 0}, {1, 1}
};

std::vector<std::pair<int, int>> IShape = {
    {0, 0}, {1, 0}, {2, 0}, {3, 0}
};

std::vector<std::pair<int, int>> TShape = {
    {0, 0}, {1, 0}, {2, 0}, {1, 1}
};

std::vector<std::pair<int, int>> ZShape = {
    {0, 1}, {1, 1}, {1, 0}, {2, 0}
};

std::vector<std::pair<int, int>> SShape = {
    {0, 0}, {1, 0}, {1, 1}, {2, 1}
};

class Piece {
public:
    int posX, posY;
    std::vector<std::pair<int, int>> pieceShape;
    std::vector<std::vector<Tile>>& tileMap;

    Piece(std::vector<std::vector<Tile>>& map, int Xpos, int Ypos, std::vector<std::pair<int, int>> shape)
        : tileMap(map), posX(Xpos), posY(Ypos), pieceShape(shape) {
        placeOnTileMap();
    }

    void clearOnTileMap() {
        for (const auto& [pieceX, pieceY] : pieceShape) {
            int x = posX + pieceX;
            int y = posY + pieceY;

            if (y >= 0 && y < tileMap.size() && x >= 0 && x < tileMap[0].size()) {
                tileMap[y][x].sprite->setTexture(*empty_tile.texture); 
                tileMap[y][x].filePath = empty_tile_path;
            }
        }
    }

    void placeOnTileMap() {
        for (const auto& [pieceX, pieceY] : pieceShape) {
            int x = posX + pieceX;
            int y = posY + pieceY;

            if (y >= 0 && y < tileMap.size() && x >= 0 && x < tileMap[0].size()) {
                tileMap[y][x].sprite->setTexture(*tile1.texture);
                tileMap[y][x].filePath = tile1_path;
            }
        }
    }

    void move(int pieceX, int pieceY) {
        clearOnTileMap();
        posX += pieceX;
        posY += pieceY;
        placeOnTileMap(); 
    }

    bool checkIfIsPartOfPiece(int newX,int newY) {
        for (const auto& [x, y] : pieceShape) {
            if (posX + x == newX && posY + y == newY) {
                return true;
            }
        }

        return false;
    }

    bool checkIfMovable(int x,int y) {
        if (x < 2 || x >= tileMap[0].size() - 1 || y < 0 || y >= tileMap.size()) {
            return false;
        }

        if (tileMap[y][x].filePath != empty_tile_path) {
            bool isPartOfPiece = checkIfIsPartOfPiece(x, y);
            if (!isPartOfPiece) {
                return false;
            }
        }

        return true;
    }

    bool canMoveDown() {
        for (const auto& [dx, dy] : pieceShape) {
            int x = posX + dx;
            int y = posY + dy;
            int newY = y + 1; 

            bool checkMove = checkIfMovable(x, newY);
            if (!checkMove) { return false; }
        }

        return true;
    }

    bool canMoveSideways(int direction) {  
        for (const auto& [pieceX, pieceY] : pieceShape) {
            // o pieceX é a posição relativa de cada tile dentro do shape
            // então o currentX é a posição da peca (tilemap) somada a posição do tile
            // assim dando a posição do tile da peca no tile map

            int currentX = posX + pieceX;
            int currentY = posY + pieceY;
            int newX = currentX + direction;

            bool checkMove = checkIfMovable(newX, currentY);
            if (!checkMove) { return false; }
        }

        return true;
    }

    void rotate() {
        std::vector<std::pair<int, int>> originalShape = pieceShape;
        
        int minX = INT_MAX, minY = INT_MAX, maxX = INT_MIN, maxY = INT_MIN;
        for (const auto& [x, y] : pieceShape) {
            if (x < minX) minX = x;
            if (y < minY) minY = y;
            if (x > maxX) maxX = x;
            if (y > maxY) maxY = y;
        }
        int originX = (minX + maxX) / 2;
        int originY = (minY + maxY) / 2;

        std::vector<std::pair<int, int>> rotatedShape;
        for (const auto& [x, y] : pieceShape) {
            int newX = originX - (y - originY);
            int newY = originY + (x - originX);
            rotatedShape.push_back({ newX, newY });
        }

        clearOnTileMap();
        
        if (isRotationValid(rotatedShape)) {
            pieceShape = rotatedShape;
            RotateSound.sound->play();
        }

        placeOnTileMap();
    }

    bool isRotationValid(std::vector<std::pair<int,int>> rotatedShape) {
        for (const auto& [X, Y] : rotatedShape) {
            int x = posX + X;
            int y = posY + Y;

            bool checkRotation = checkIfMovable(x, y);
            if (!checkRotation) { return false; }
        }
        return true;
    }
    
    std::vector<std::pair<int, int>> getTiles() {
        std::vector<std::pair<int, int>> tiles;

        for (const auto& [pieceX, pieceY] : pieceShape) {
            int currentX = posX + pieceX;
            int currentY = posY + pieceY;

            tiles.push_back({ currentX, currentY });
        }

        return tiles;
    };
};

void renderTiles(sf::RenderWindow& window, std::vector<std::vector<Tile>>& tileMap) {
    // Loop pelos tiles depois de abrir a janela
    for (const auto& row : tileMap) {
        for (auto& tile : row) {
            window.draw(*tile.sprite);
        }
    }
}

std::vector<int> checkCompletedLines(int startY, int endY, int columns, const std::vector<std::vector<Tile>>& tileMap) {
    std::vector<int> completedLines;

    for (int y = endY; y >= startY; y--) {
        bool lineCompleted = true;

        for (int x = 2; x < columns - 1; x++) {
            if (tileMap[y][x].filePath == empty_tile_path) {
                lineCompleted = false;
                break;
            }
        }

        if (lineCompleted) {
            completedLines.push_back(y);
        }
    }

    return completedLines;
}

void clearLines(const std::vector<int>& lines, int columns, std::vector<std::vector<Tile>>& tileMap, sf::RenderWindow& window) {
    if (lines.empty()) return;

    if (lines.size() == 4) {
        TetrisClearSound.sound->play();
    }
    else {
        ClearLineSound.sound->play();
    }

    for (int x = 2; x < columns - 1; x++) {
        for (int lineIndex : lines) {
            tileMap[lineIndex][x].filePath = empty_tile_path;
            tileMap[lineIndex][x].sprite->setTexture(*empty_tile.texture);
        }

        window.clear();
        renderTiles(window, tileMap);
        window.draw(*Gui.sprite);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        window.display();
    }
}

void makeBlocksFall(const std::vector<int>& clearedLines, int columns, std::vector<std::vector<Tile>>& tileMap) {
    if (clearedLines.empty()) return;

    std::vector<int> dropAmount(tileMap.size(), 0);

    for (int y = 0; y < tileMap.size(); y++) {
        for (int clearedY : clearedLines) {
            if (clearedY > y) {
                dropAmount[y]++;
            }
        }
    }

    for (int y = tileMap.size() - 1; y >= 0; y--) {
        if (std::find(clearedLines.begin(), clearedLines.end(), y) != clearedLines.end() ||
            dropAmount[y] == 0) {
            continue;
        }

        int newY = y + dropAmount[y];

        if (newY < tileMap.size()) {
            for (int x = 2; x < columns - 1; x++) {
                tileMap[newY][x].filePath = tileMap[y][x].filePath;

                if (tileMap[y][x].filePath != empty_tile_path) {
                    tileMap[newY][x].sprite->setTexture(*tile1.texture);
                }
                else {
                    tileMap[newY][x].sprite->setTexture(*empty_tile.texture);
                }

                tileMap[y][x].filePath = empty_tile_path;
                tileMap[y][x].sprite->setTexture(*empty_tile.texture);
            }
        }
    }
}

void CreateNumberCounter(int startXPos, int startYPos, int number, sf::RenderWindow& window) {
    std::string numberStr = std::to_string(number);
    int numberQuantity = numberStr.length();
    int ActualX = startXPos;

    for (int i = 0; i < numberQuantity; i++) {
        int displayNumber = numberStr[i] - '0';
        std::string file = "Sprites/Numbers/" + std::to_string(displayNumber) + ".png";
        Object Number(file, ActualX, startYPos, 0, 0, 2.0f, 2.0f);
        window.draw(*Number.sprite);
        ActualX += 16;
    }
    
}

int main() {
    int normal_tickrate = 20;
    int fast_tickrate = normal_tickrate / 4;
    int tickrate = normal_tickrate;
    int tick = 5;

    const int width = 512;   
    const int height = 448;  

    int colums = 13;    
    int rows = 21;    

    int yGridPos = 5;
    int xGridPos = 11;

    int lineCounter = 0;

    std::vector<std::vector<Tile>> tileMap;

    for (int y = 0; y < rows; y++) {
        std::vector<Tile> row;
        for (int x = 0; x < colums; x++) {
            Tile tile(x, y, xGridPos,yGridPos);
            row.emplace_back(tile);
        }
        tileMap.push_back(row);
    }
    
    // Loop pelos tiles antes de abrir a janela
    for (const auto& row : tileMap) {
        for (auto& tile : row) {
            if (tile.xGridPos == 1 || tile.xGridPos == colums - 1 || tile.yGridPos == 0 || tile.yGridPos == rows) {
                tile.sprite->setTexture(*wall.texture);
            }
        }
    }

    std::unique_ptr<Piece> MainPiece = std::make_unique<Piece>(tileMap, 5, 1, TShape);
   
    std::unique_ptr window = std::make_unique<sf::RenderWindow>(sf::VideoMode({ width, height }), "Tetris");

    window->setFramerateLimit(60);

    while (window->isOpen()) {
        // Os tiles das bordas sempre devem ficar vazios
        // Tiles usáveis: x de 1 a 10 e y de 1 a 20

        tick -= 1;

        while (const std::optional event = window->pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window->close();
            }

            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    window->close();
                    return 0;
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Left) {
                    if (MainPiece->canMoveSideways(-1)) {
                        MoveSound.sound->play();
                        MainPiece->move(-1, 0);
                    }
                    
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Right) {
                    if (MainPiece->canMoveSideways(1)) {
                        MoveSound.sound->play();
                        MainPiece->move(1, 0);
                    }

                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Up) {
                    MainPiece->rotate();
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            tickrate = fast_tickrate;
        }
        else {
            tickrate = normal_tickrate;
        }

        if (tick == 0) {
            if (!MainPiece->canMoveDown()) {
                MainPiece->move(0, 0);
                int chosePiece = rng(1, 7);

                std::vector<std::pair<int, int>> tilesOfPiece = MainPiece->getTiles();
                
                std::vector<int> completedLines = checkCompletedLines(1, rows - 1, colums, tileMap);
                if (!completedLines.empty()) {
                    lineCounter += completedLines.size();
                    clearLines(completedLines, colums, tileMap, *window);
                    makeBlocksFall(completedLines, colums, tileMap);
                }

                PlaceSound.sound->play();

                switch (chosePiece) {
                case 1:
                    MainPiece.reset(new Piece(tileMap, 5, 1, TShape));
                    break;
                case 2:
                    MainPiece.reset(new Piece(tileMap, 5, 1, IShape));
                    break;
                case 3:
                    MainPiece.reset(new Piece(tileMap, 5, 1, LShape));
                    break;
                case 4:
                    MainPiece.reset(new Piece(tileMap, 5, 1, OShape));
                    break;
                case 5:
                    MainPiece.reset(new Piece(tileMap, 5, 1, ZShape));
                    break;
                case 6:
                    MainPiece.reset(new Piece(tileMap, 5, 1, SShape));
                    break;
                case 7:
                    MainPiece.reset(new Piece(tileMap, 5, 1, JShape));
                    break;
                }
   
            }
            else {
                MainPiece->move(0, 1);
            }
            tick = tickrate;
        }

        window->clear();

        renderTiles(*window, tileMap);

        window->draw(*Gui.sprite);

        CreateNumberCounter(304, 32, lineCounter, *window);

        window->display();
    }
    return 0;
}