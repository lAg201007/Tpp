#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <random>
#include <chrono>
#include <thread>
#include <cmath>
#include "SFML_CLASSES.h" 
#include <fstream>

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

std::string getLevelSpritePath(int level, int tileend) {
    std::string levelStr;
    std::string tileEnd ="_" + std::to_string(tileend) + ".png";

    if (level % 10 == 0) {
        levelStr = "10";
    }
    else {
        levelStr = std::to_string(level % 10);
    }

    std::string fullPath = "Sprites/Tiles/" + levelStr + tileEnd;

    return fullPath;
}

std::string empty_tile_path = "Sprites/Tiles/empty_tile.png";
std::string tile1_path = "Sprites/Tiles/1_1.png";
std::string tile2_path = "Sprites/Tiles/1_2.png";
std::string tile3_path = "Sprites/Tiles/1_3.png";
std::string wall_path = "Sprites/Tiles/wall.png";

Texture empty_tile(empty_tile_path);
Texture tile1(tile1_path);
Texture tile2(tile2_path);
Texture tile3(tile3_path);

Texture wall("Sprites/Tiles/wall.png");

Texture T("Sprites/Pieces/T.png");
Texture J("Sprites/Pieces/J.png");
Texture L("Sprites/Pieces/L.png");
Texture I("Sprites/Pieces/I.png");
Texture O("Sprites/Pieces/O.png");
Texture Z("Sprites/Pieces/Z.png");
Texture S("Sprites/Pieces/S.png");

Sound MoveSound("SoundEffects/move.wav");
Sound PlaceSound("SoundEffects/place.wav");
Sound RotateSound("SoundEffects/rotate.wav");
Sound ClearLineSound("SoundEffects/single.wav");
Sound TetrisClearSound("SoundEffects/tetris.wav");
Sound LevelUpSound("SoundEffects/level_up.wav", 200);

Object Gui("Sprites/UI.png", 0, 0, 0, 0, 2.0f, 2.0f);
Object TitleScreen("Sprites/title_screen.png", 0, 0, 0, 0, 2.0f, 2.0f);
Object NextPieceUI("Sprites/Pieces/L.png", 416, 240, 12, 12, 2.0f, 2.0f);


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

std::vector<std::pair<int, int>> PiecesArray[7] = {LShape,JShape,OShape,IShape,TShape,ZShape,SShape};

class Piece {
public:
    int posX, posY;
    std::vector<std::pair<int, int>> pieceShape;
    std::vector<std::vector<Tile>>& tileMap;
    std::string currentTexturePath; // Novo atributo

    Piece(std::vector<std::vector<Tile>>& map, int Xpos, int Ypos, std::vector<std::pair<int, int>> shape)
        : tileMap(map), posX(Xpos), posY(Ypos), pieceShape(shape) {
        if (shape == IShape || shape == OShape || shape == TShape) {
            currentTexturePath = tile1_path;
            placeOnTileMap(currentTexturePath);
        }
        else if (shape == ZShape || shape == JShape) {
            currentTexturePath = tile3_path;
            placeOnTileMap(currentTexturePath);
        }
        else if (shape == LShape || shape == SShape) {
            currentTexturePath = tile2_path;
            placeOnTileMap(currentTexturePath);
        }
    }


    void clearOnTileMap() {
        for (const auto& [pieceX, pieceY] : pieceShape) {
            int x = posX + pieceX;
            int y = posY + pieceY;

            if (y >= 0 && y < tileMap.size() && x >= 0 && x < tileMap[0].size()) {
                tileMap[y][x].filePath = empty_tile_path;
            }
        }
    }

    void placeOnTileMap(std::string path) {
        for (const auto& [pieceX, pieceY] : pieceShape) {
            int x = posX + pieceX;
            int y = posY + pieceY;

            if (y >= 0 && y < tileMap.size() && x >= 0 && x < tileMap[0].size()) {
                tileMap[y][x].filePath = path;
            }
        }
    }

    void move(int pieceX, int pieceY) {
        clearOnTileMap();
        posX += pieceX;
        posY += pieceY;

        placeOnTileMap(currentTexturePath);
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
                std::cout << tileMap[y][x].filePath << std::endl;
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

        placeOnTileMap(currentTexturePath);
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

void renderTiles(sf::RenderWindow& window, std::vector<std::vector<Tile>>& tileMap, int level) {
    for (auto& row : tileMap) {
        for (auto& tile : row) {
            // Check if the current texture path matches the tile's file path
            if (tile.filePath == empty_tile_path) {
                tile.sprite->setTexture(*empty_tile.texture);
            }
            else if (tile.filePath == tile1_path) {
                tile.sprite->setTexture(*tile1.texture);
            }
            else if (tile.filePath == tile2_path) {
                tile.sprite->setTexture(*tile2.texture);
            }
            else if (tile.filePath == tile3_path) {
                tile.sprite->setTexture(*tile3.texture);
            }
            else if (tile.filePath == wall_path) {
                tile.sprite->setTexture(*wall.texture);
            }
            else if (tile.filePath == "Sprites/Tiles/" + std::to_string(level - 1) + "_1.png"){
                tile.filePath = "Sprites/Tiles/" + std::to_string(level) + "_1.png";
                renderTiles(window, tileMap, level);
            }
            else if (tile.filePath == "Sprites/Tiles/" + std::to_string(level - 1) + "_2.png") {
                tile.filePath = "Sprites/Tiles/" + std::to_string(level) + "_2.png";
                renderTiles(window, tileMap, level);
            }
            else if (tile.filePath == "Sprites/Tiles/" + std::to_string(level - 1) + "_3.png") {
                tile.filePath = "Sprites/Tiles/" + std::to_string(level) + "_3.png";
                renderTiles(window, tileMap, level);
            }
            // Draw the tile with the correct texture
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

void clearLines(const std::vector<int>& lines, int columns, std::vector<std::vector<Tile>>& tileMap, sf::RenderWindow& window, Texture texture, std::string path,int level) {
    if (lines.empty()) return;

    if (lines.size() == 4) {
        TetrisClearSound.sound->play();
    }
    else {
        ClearLineSound.sound->play();
    }

    for (int x = 2; x < columns - 1; x++) {
        for (int lineIndex : lines) {
            tileMap[lineIndex][x].filePath = path;
        }

        window.clear();
        renderTiles(window, tileMap, level);
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
                std::string originalFilePath = tileMap[y][x].filePath;
                tileMap[newY][x].filePath = originalFilePath;
                tileMap[y][x].filePath = empty_tile_path;
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

void SaveTopScore(int score) {
    std::string fileName = "data.bin";

    std::ofstream Data(fileName, std::ios::binary);
    Data << score;
    Data.close();
}

int calculateSpeed(int level) {
    if (level <= 9) {
        return 48 - (level * 5);
    }
    else if (level >= 10 && level <= 12) {
        return 5;
    }
    else if (level >= 13 && level <= 15) {
        return 4;
    }
    else if (level >= 16 && level <= 18) {
        return 3;
    }
    else if (level >= 19 && level <= 28) {
        return 2;
    }
    else if (level >= 29) {
        return 1;
    }
}

int main() {
    int top_score = 0;

    std::ifstream Data("data.bin");

    if (Data) {
        Data >> top_score;
        Data.close();
    }
    
    start:

    int level = 1;
    int linesForLevelingUp = 10;
    int linesThisLevel = 0;

    int score = 0;

    int normal_tickrate = calculateSpeed(level);
    int fast_tickrate = 3;
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
            if (tile.xGridPos == 1 || tile.xGridPos == colums - 1 ||  tile.yGridPos == rows) {
                tileMap[tile.yGridPos][tile.xGridPos].filePath = wall_path;
            }
        }
    }

    std::unique_ptr<Piece> MainPiece = std::make_unique<Piece>(tileMap, 5, 1, TShape);
   
    std::unique_ptr window = std::make_unique<sf::RenderWindow>(sf::VideoMode({ width, height }), "Tetris");

    window->setFramerateLimit(60);

    int previous_tickrate = normal_tickrate;

    bool InTitleScreen = true;
    bool InGame = false;
    bool InGameOverScreen = false;

    std::pair<std::vector<std::pair<int,int>>,std::vector<std::pair<int,int>>> PiecesRandom = {PiecesArray[rng(0,6)],PiecesArray[rng(0,6)]};
    
    while (window->isOpen()) {
        while (InTitleScreen) {
            while (const std::optional event = window->pollEvent()) {

                if (event->is<sf::Event::Closed>()) {
                    window->close();
                    return 0;
                }

                else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                        window->close();
                        return 0;
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::Enter) {
                        InTitleScreen = false;
                        InGame = true;
                    }
                }
            }

            window->clear();

            window->draw(*TitleScreen.sprite);

            window->display();
        }

        while (InGame) {
            // Os tiles das bordas sempre devem ficar vazios
            // Tiles usáveis: x de 1 a 10 e y de 1 a 20

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
                tickrate = fast_tickrate;
            }
            else {
                tickrate = normal_tickrate;
            }

            if (previous_tickrate != tickrate) {
                tick = 1;
                previous_tickrate = tickrate;
            }

            tick -= 1;

            while (const std::optional event = window->pollEvent()) {

                if (event->is<sf::Event::Closed>()) {
                    window->close();
                    SaveTopScore(top_score);
                    return 0;
                }

                else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                        window->close();
                        SaveTopScore(top_score);
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

            if (tick == 0) {
                if (!MainPiece->canMoveDown()) {

                    if (MainPiece->posY == 1) {
                        std::vector<int> allYs;
                        for (int y = rows - 1; y >= 0; y--) {
                            allYs.push_back(y);
                        }
                        clearLines(allYs, colums, tileMap, *window, tile2, tile2_path, level);
                        InGame = false;
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                        SaveTopScore(top_score);

                        std::string tile1_path = "Sprites/Tiles/1_1.png";
                        std::string tile2_path = "Sprites/Tiles/1_2.png";
                        std::string tile3_path = "Sprites/Tiles/1_3.png";

                        goto start;
                    }

                    MainPiece->move(0, 0);

                    PiecesRandom.first = PiecesRandom.second;
                    PiecesRandom.second = PiecesArray[rng(0, 6)];

                    std::vector<std::pair<int, int>> tilesOfPiece = MainPiece->getTiles();
                
                    std::vector<int> completedLines = checkCompletedLines(1, rows - 1, colums, tileMap);
                    if (!completedLines.empty()) {
                        lineCounter += completedLines.size();
                        linesThisLevel += completedLines.size();

                        if (completedLines.size() == 1) {
                            score += 40 * (level + 1);
                        }
                        else if (completedLines.size() == 2) {
                            score += 100 * (level + 1);
                        }
                        else if (completedLines.size() == 3) {
                            score += 300 * (level + 1);
                        }
                        else if (completedLines.size() == 4) {
                            score += 1200 * (level + 1);
                        }

                        if (score > top_score) {
                            top_score = score;
                        }

                        clearLines(completedLines, colums, tileMap, *window, empty_tile, empty_tile_path, level);
                        makeBlocksFall(completedLines, colums, tileMap);

                        if (linesThisLevel >= linesForLevelingUp) {
                            linesThisLevel -= linesForLevelingUp;
                            linesForLevelingUp + 10;
                            level++;

                            tile1_path = getLevelSpritePath(level, 1);
                            tile2_path = getLevelSpritePath(level, 2);
                            tile3_path = getLevelSpritePath(level, 3);

                            normal_tickrate = calculateSpeed(level);
                            LevelUpSound.sound->play();
                        }
                    }

                    PlaceSound.sound->play();

                    MainPiece.reset(new Piece(tileMap, 5, 1, PiecesRandom.first));

                }
                else {
                    MainPiece->move(0, 1);
                }
                tick = tickrate;
            }

            window->clear();

            renderTiles(*window, tileMap, level);

            window->draw(*Gui.sprite);

            CreateNumberCounter(304, 32, lineCounter, *window);

            CreateNumberCounter(416, 320, level, *window);

            CreateNumberCounter(384,112, score, *window);

            CreateNumberCounter(384, 64, top_score, *window);

            Texture* pieceTextures[] = { &L, &J, &O, &I, &T, &S, &Z};

            int textureIndex = 0;
            for (int i = 0; i < 7; i++) {
                if (PiecesRandom.second == PiecesArray[i]) {
                    textureIndex = i;
                    break;
                }
            }
            
            NextPieceUI.sprite->setTexture(*pieceTextures[textureIndex]->texture);

            window->draw(*NextPieceUI.sprite);

            window->display();
        }
    }
    return 0;
}