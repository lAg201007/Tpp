#ifndef OBJECT
#define OBJECT

class Object {
public:
    std::shared_ptr<sf::Texture> spriteTexture;
    std::shared_ptr<sf::Sprite> sprite;
    std::string filePath;

    Object(std::string imgfile, float startXpos, float startYpos, int originX = 0, int originY = 0, float scaleX = 1, float scaleY = 1) : filePath(imgfile) {
        spriteTexture = std::make_shared<sf::Texture>();

        if (!spriteTexture->loadFromFile(imgfile)) {
            std::cerr << "N�o foi poss�vel carregar a imagem: " << imgfile << std::endl;
        }

        sprite = std::make_shared<sf::Sprite>(*spriteTexture);

        sprite->setPosition({ startXpos, startYpos });


        if (originX && originY) {
            sprite->setOrigin(sf::Vector2f(static_cast<float>(originX), static_cast<float>(originY)));
        }

        sprite->setScale({ scaleX, scaleY });
    }
};

#endif 

#ifndef TEXTURE
#define TEXTURE

class Texture {
public:
    std::shared_ptr<sf::Texture> texture;

    Texture(std::string imgfile) {
        texture = std::make_shared<sf::Texture>();

        if (!texture->loadFromFile(imgfile)) {
            std::cerr << "N�o foi poss�vel carregar a imagem: " << imgfile << std::endl;
        }
    }
};

#endif 

#ifndef SOUND
#define SOUND

class Sound {
public:
    sf::SoundBuffer Buffer;
    std::unique_ptr<sf::Sound> sound;
    Sound(std::string soundfile, int soundVolume = 100) {

        if (!Buffer.loadFromFile(soundfile)) {
            std::cerr << "N�o foi possivel carregar som para o arquivo: " << soundfile << std::endl;
        }

        sound = std::make_unique<sf::Sound>(Buffer);
        sound->setVolume(soundVolume);
    }
};

#endif 
