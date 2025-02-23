#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    const int width = 400;
    const int height = 600;

    std::unique_ptr window = std::make_unique<sf::RenderWindow> (sf::VideoMode({ width,height }), "Tetris");

    while (window->isOpen()) {

        while (const std::optional event = window->pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window->close();
            }

        }

        window->clear();


        window->display();
    }

    return 0;
}

