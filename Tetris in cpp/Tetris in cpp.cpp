#include <iostream>
#include <SFML/Graphics.hpp>
#include <memory>

class Object {
public:
    std::shared_ptr<sf::Texture> spriteTexture;
    std::shared_ptr<sf::Sprite> sprite;
    std::string filePath;

    Object(std::string imgfile, float startXpos, float startYpos, int originX = 0, int originY = 0, float scaleX = 1, float scaleY = 1) : filePath(imgfile) {
        spriteTexture = std::make_shared<sf::Texture>();

        if (!spriteTexture->loadFromFile(imgfile)) {
            std::cerr << "Não foi possível carregar a imagem: " << imgfile << std::endl;
        }

        sprite = std::make_shared<sf::Sprite>(*spriteTexture);

        sprite->setPosition({ startXpos, startYpos });


        if (originX && originY) {
            sprite->setOrigin(sf::Vector2f(static_cast<float>(originX), static_cast<float>(originY)));
        }

        sprite->setScale({ scaleX, scaleY });
    }
};

class Tile : public Object {
public:
    int xGridPos;
    int yGridPos;

    Tile(int xGrid, int yGrid)
        : Object("Sprites/empty_tile.png", xGrid * 8.0f, yGrid * 8.0f,4,4) { 
        xGridPos = xGrid; yGridPos = yGrid;
    }
};

class Texture {
public:
    std::shared_ptr<sf::Texture> texture;

    Texture(std::string imgfile) {
        texture = std::make_shared<sf::Texture>();

        if (!texture->loadFromFile(imgfile)) {
            std::cerr << "Não foi possível carregar a imagem: " << imgfile << std::endl;
        }
    }
};

std::string empty_tile_path = "Sprites/empty_tile.png";
std::string tile1_path = "Sprites/tile1.png";
Texture empty_tile(empty_tile_path);
Texture tile1(tile1_path);

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
        for (const auto& [dx, dy] : pieceShape) {
            int x = posX + dx;
            int y = posY + dy;

            if (y >= 0 && y < tileMap.size() && x >= 0 && x < tileMap[0].size()) {
                tileMap[y][x].sprite->setTexture(*empty_tile.texture); 
                tileMap[y][x].filePath = empty_tile_path;
            }
        }
    }

    void placeOnTileMap() {
        for (const auto& [dx, dy] : pieceShape) {
            int x = posX + dx;
            int y = posY + dy;

            if (y >= 0 && y < tileMap.size() && x >= 0 && x < tileMap[0].size()) {
                tileMap[y][x].sprite->setTexture(*tile1.texture);
                tileMap[y][x].filePath = tile1_path;
            }
        }
    }

    void move(int dx, int dy) {
        clearOnTileMap();
        posX += dx;
        posY += dy;
        placeOnTileMap(); 
    }

    int getLowestTileY() {
        int maxLocalY = pieceShape[0].second; 

        for (const auto& [dx, dy] : pieceShape) {
            if (dy > maxLocalY) {
                maxLocalY = dy; 
                
            }
        }

        return posY + maxLocalY;
    }

    bool canMoveDown() {
        int lowestY = getLowestTileY();

        for (const auto& [dx, dy] : pieceShape) {
           int x = posX + dx;
           int y = lowestY + 1;
           int generalY = posY + dy;

           if (y >= tileMap.size() ) {
               return false;
           }
           else if (tileMap[y][x].filePath != empty_tile_path) {
               return false;
           }

        }

        return true;
    }

};

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

int main() {
    int tickrate = 5;
    int tick = 5;

    const int width = 256;
    const int height = 400; 

    int colums = width / 8;
    int rows = height / 8;

    std::vector<std::vector<Tile>> tileMap;

    for (int i = 0; i < rows + 1; i++) {
        std::vector<Tile> row;
        for (int a = 0; a < colums + 1; a++) {
            Tile tile(a, i);
            row.emplace_back(tile);
        }
        tileMap.push_back(row);
    }
    
    // Loop pelos tiles antes de abrir a janela
    for (const auto& row : tileMap) {
        for (auto& tile : row) {
            if (tile.xGridPos == 0 || tile.xGridPos == colums || tile.yGridPos == 0 || tile.yGridPos == rows) {
                tile.sprite->setTexture(*empty_tile.texture);
            }
        }
    }

    std::unique_ptr<Piece> MainPiece = std::make_unique<Piece>(tileMap, 15, 1, LShape);
   
    std::unique_ptr window = std::make_unique<sf::RenderWindow>(sf::VideoMode({ width, height }), "Tetris");

    window->setFramerateLimit(60);

    while (window->isOpen()) {
        // Os tiles das bordas sempre devem ficar vazios
        // Tiles usáveis: x de 1 a 31 e y de 1 a 49

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
                    MainPiece->move(-1, 0);
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Right) {
                    MainPiece->move(1, 0);
                }
            }
        }

        if (tick == 0) {
            if (!MainPiece->canMoveDown()) {
                MainPiece->move(0, 0);
                MainPiece.reset(new Piece(tileMap, 15, 1, LShape));
            }
            else {
                MainPiece->move(0, 1);
            }
            tick = tickrate;
        }

        window->clear();

        // Loop pelos tiles depois de abrir a janela
        for (const auto& row : tileMap) {
            for (auto& tile : row) {
                window->draw(*tile.sprite);
            }
        }
        window->display();
    }
    return 0;
}

