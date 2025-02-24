#include <iostream>
#include <SFML/Graphics.hpp>
#include <memory>

class Object {
public:
    std::shared_ptr<sf::Texture> spriteTexture; 
    std::shared_ptr<sf::Sprite> sprite;

    Object(std::string imgfile, float startXpos, float startYpos, int originX = 0, int originY = 0, float scaleX = 1, float scaleY = 1) {
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
        : Object("Sprites/tile1.png", xGrid * 8.0f, yGrid * 8.0f,4,4) { 
        xGridPos = xGrid; yGridPos = yGrid;
    }
};

int main()
{
    const int width = 256;
    const int height = 240;
    
    int colums = width / 8;
    int rows = height / 8;

    std::vector<std::vector<Tile>> tileMap;

    for (int i = 1; i < rows + 1; i++) {
        std::vector<Tile> row;
        for (int a = 1; a < colums + 1; a++) {
            Tile tile(a,i);
            row.emplace_back(tile);
        }
        tileMap.push_back(row);
    }
  

    std::unique_ptr window = std::make_unique<sf::RenderWindow> (sf::VideoMode({ width,height }), "Tetris");

    window->setFramerateLimit(60);

    while (window->isOpen()) {

        while (const std::optional event = window->pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window->close();
            }

        }

        window->clear();

        for (const auto& row : tileMap) {
            for (auto& tile : row) {
                window->draw(*tile.sprite);
            }
        }

        window->display();
    }

    return 0;
}

