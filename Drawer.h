
#pragma once

#include "SFML/Graphics.hpp"
#include "Field.h"

/**
 * @enum Style
 * @brief Enum containing the drawing styles switches.
 */
enum class Style
{
    UGLY = 0, BEAUTY, TEXT
};

class Drawer
{
    private:

        sf::RenderWindow window;
        sf::Event sfEvent;

        sf::CircleShape particle;
        sf::Font font;
        std::vector<sf::Text> fieldText;
        sf::Image fieldImage;
        sf::Texture fieldTexture;
        sf::Sprite fieldSprite;
        Field& field;

        Style style;
        bool realistic;

        void updateKeybinds();
        void updateField();

        void drawParticles();
        static sf::Color getThreeGradientColor(float val);

    public:

        explicit Drawer(Field& field);

        void update();
        void render();
        void run();
};


