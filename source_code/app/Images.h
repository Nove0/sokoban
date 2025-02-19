#include<SFML/Graphics.hpp>
#include<iostream>

class Images
{
    static sf::Texture box_image;
    static sf::Texture player_image;
    static sf::Texture wall_image;
    static sf::Texture back_image;
    static sf::Texture target_image;

    static bool loaded;

    static void resize_sprite(sf::Sprite& sprite, int x, int y) 
    {
        sf::FloatRect original_size = sprite.getGlobalBounds();

        float scaleX = static_cast<float>(x) / original_size.width;
        float scaleY = static_cast<float>(y) / original_size.height;

        sprite.setScale(scaleX, scaleY);
    }

public:
    static void load_data()
    {
        if (loaded) return;

        if (!box_image.loadFromFile("../../images/box_image.png") ||
            !player_image.loadFromFile("../../images/player_image.png") ||
            !wall_image.loadFromFile("../../images/wall_image.jpg") ||
            !back_image.loadFromFile("../../images/back_image.png") ||
            !target_image.loadFromFile("../../images/x_image.png"))
        {
            std::cerr << "Error loading images" << std::endl;
            std::exit(1);
        }

        box_image.setSmooth(true);
        player_image.setSmooth(true);
        wall_image.setSmooth(true);
        back_image.setSmooth(true);
        target_image.setSmooth(true);

        loaded = true;
    }

    static sf::Sprite get_image(int type, int width, int height)
    {
        if (!loaded)
        {
            std::cerr << "Images are not loaded, use the Images::load_data() function to load them." << std::endl;
            std::exit(1);
        }

        sf::Sprite sprite;
        if (type == 1)
        {
            sprite.setTexture(wall_image);
        }
        else if (type == 2)
        {
            sprite.setTexture(box_image);
        }
        else if (type == 3)
        {
            sprite.setTexture(back_image);
        }
        else if (type == 4)
        {
            sprite.setTexture(target_image);
        }
        else
        {
            sprite.setTexture(player_image);
        }

        resize_sprite(sprite, width, height);
        return sprite;
    }
};

// Static variable definitions
bool Images::loaded = false;
sf::Texture Images::box_image;
sf::Texture Images::player_image;
sf::Texture Images::wall_image;
sf::Texture Images::back_image;
sf::Texture Images::target_image;
