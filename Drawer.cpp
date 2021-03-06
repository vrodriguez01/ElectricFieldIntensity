
#include "Drawer.h"

/**
 * @brief "Drawer" class constructor.
 * @details Creates the window, the field image, texture and
 * sprite, and sets the drawing style, realism and font.
 * @param field Reference to the field to be drawn.
 */
Drawer::Drawer(Field& field): field(field)
{
    sf::VideoMode windowBounds = sf::VideoMode::getDesktopMode();
    windowBounds.width = 1024;
    windowBounds.height = 576;
    window.create(windowBounds, "Lignes de champ", sf::Style::Titlebar | sf::Style::Close);

    fieldImage.create(window.getSize().x, window.getSize().y, sf::Color::White);
    fieldTexture.create(window.getSize().x, window.getSize().y);
    fieldSprite.setTexture(fieldTexture);

    style = Style::UGLY;
    realistic = true;

    font.loadFromFile("DTM-Mono.otf");

    window.setKeyRepeatEnabled(false);
}

/**
 * Update loop.
 */
void Drawer::update()
{
    while(window.pollEvent(sfEvent))
    {
        if(sfEvent.type == sf::Event::Closed)
            window.close();

        updateKeybinds();
    }

    updateField();
}

/**
 * Keybinds update loop.
 */
void Drawer::updateKeybinds()
{
    //Add particles
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && sfEvent.KeyPressed)
    {
        //To avoid having two particles in the same place
        if(!field.getParticles()->empty())
        {
            if(field.getParticles()->back().x != sf::Mouse::getPosition(window).x || field.getParticles()->back().y != sf::Mouse::getPosition(window).y)
                field.getParticles()->emplace_back(sf::Mouse::getPosition(window).x,
                                                    sf::Mouse::getPosition(window).y, 1);
        }
        else
        {
            field.getParticles()->emplace_back(sf::Mouse::getPosition(window).x,
                                                sf::Mouse::getPosition(window).y, 1);
        }
    }
    else if(sf::Mouse::isButtonPressed(sf::Mouse::Right) && sfEvent.KeyPressed)
    {
        if(!field.getParticles()->empty())
        {
            if(field.getParticles()->back().x != sf::Mouse::getPosition(window).x || field.getParticles()->back().y != sf::Mouse::getPosition(window).y)
                field.getParticles()->emplace_back(sf::Mouse::getPosition(window).x,
                                                    sf::Mouse::getPosition(window).y, -1);
        }
        else
        {
            field.getParticles()->emplace_back(sf::Mouse::getPosition(window).x,
                                                sf::Mouse::getPosition(window).y, -1);
        }
    }

    //Delete particles
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::X) && sfEvent.type == sf::Event::KeyPressed)
    {
        if(sfEvent.key.shift)
            field.getParticles()->clear();
        else if(!field.getParticles()->empty())
            field.getParticles()->pop_back();
    }

    //Drawing style
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) && sfEvent.type == sf::Event::KeyPressed)
    {
        style = Style::UGLY;
        realistic = true;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) && sfEvent.type == sf::Event::KeyPressed)
    {
        style = Style::BEAUTY;
        realistic = false;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) && sfEvent.type == sf::Event::KeyPressed)
    {
        style = Style::TEXT;
        realistic = false;
    }

    //Realism
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::R) && sfEvent.type == sf::Event::KeyPressed)
    {
        realistic ? realistic = false
                  : realistic = true;
    }
}

/**
 * Field update loop.
 */
void Drawer::updateField()
{

    for (int i = 0; i < window.getSize().x; ++i)
    {
        for (int j = 0; j < window.getSize().y; ++j)
        {
            static float E; //To call fieldAtPoint() only once

            if(!field.getParticles()->empty())
            {
                E = field.fieldAtPoint(i, j, realistic);

                switch (style)
                {
                    case Style::UGLY:

                        E > 0 ? fieldImage.setPixel(i, j, sf::Color::Red)
                              : fieldImage.setPixel(i, j, sf::Color::Blue);

                        break;

                    case Style::BEAUTY:

                        if(i % 30 == 0 && j % 30 == 0)
                        {
                            field.getFields()->push_back(E);
                            field.fieldRange();
                        }

                        fieldImage.setPixel(i, j, getThreeGradientColor((E - field.lower) / (field.upper - field.lower)));

                        break;

                    case Style::TEXT:

                        fieldImage.setPixel(i, j, sf::Color::White);

                        if(i % 30 == 0 && j % 20 == 0)
                        {
                            fieldText.emplace_back();
                            fieldText.back().setFont(font);
                            fieldText.back().setCharacterSize(15);

                            realistic ? fieldText.back().setString(std::to_string((int)std::round(E)))
                                      : fieldText.back().setString(std::to_string((int)std::round(100*E)));
                            fieldText.back().setPosition(i, j);
                            fieldText.back().setFillColor(sf::Color::Black);
                        }

                        break;
                }

                E = 0;
            }
            else
            {
                fieldImage.setPixel(i, j, sf::Color::White);

                if(style == Style::TEXT)
                {
                    if(i % 30 == 0 && j % 20 == 0)
                    {
                        fieldText.emplace_back();
                        fieldText.back().setFont(font);
                        fieldText.back().setCharacterSize(15);
                        fieldText.back().setString("0");
                        fieldText.back().setPosition(i, j);
                        fieldText.back().setFillColor(sf::Color::Black);
                    }
                }
            }
        }
    }

    fieldTexture.update(fieldImage);
}

/**
 * Render function.
 */
void Drawer::render()
{
    window.clear();

    switch(style)
    {
        case Style::UGLY:

            window.draw(fieldSprite);
            drawParticles();
            break;

        case Style::BEAUTY:

            window.draw(fieldSprite);
            break;

        case Style::TEXT:

            window.draw(fieldSprite);

            for (auto& t: fieldText)
            {
                window.draw(t);
            }

            fieldText.clear();

            break;
    }

    window.display();
}

/**
 * Particles draw function.
 */
void Drawer::drawParticles()
{
    for (auto& currentParticle: *field.getParticles())
    {
        this->particle.setPosition(currentParticle.x - 10, currentParticle.y - 10);

        currentParticle.q > 0 ? this->particle.setFillColor(sf::Color::White)
                              : this->particle.setFillColor(sf::Color::Black);

        this->particle.setRadius(10);

        window.draw(this->particle);
    }
}

/**
 * Gradient function. By default, gradient goes from blue to
 * white, and finally to red.
 * @param val Normalized value
 * @return The color corresponding to 'val' in the gradient
 */
sf::Color Drawer::getThreeGradientColor(float val)
{
    //The colors of the gradient and their components
    static float color[3][3] = { {0,0,1}, {1,1,1}, {1,0,0} };

    //Two indices that will indicate in which section 'val'
    // is (between red and white or between white and blue)
    int idx1, idx2;
    //The relative value of 'val' in this section
    float localVal = 0;

    if(val <= 0)
        idx1 = idx2 = 0;
    else if(val >= 1)
        idx1 = idx2 = 2;
    else
    {
        val = val * 2; //There are two possible sections, so
                       // val's percentage is doubled (25% of the
                       // whole gradient corresponds to 50% of the
                       // first section)
        idx1  = std::floor(val);
        idx2  = idx1 + 1;
        localVal = val - (float)idx1; //Local percentage
    }

    //The color corresponding to val in the gradient is the local
    // percentage (localVal) between two colors of the gradient
    // (idx1 and idx2), multiplied by 255 to correspond to the
    // range accepted by SFML.
    sf::Uint8 red   = 255*((color[idx2][0] - color[idx1][0])*localVal + color[idx1][0]);
    sf::Uint8 green = 255*((color[idx2][1] - color[idx1][1])*localVal + color[idx1][1]);
    sf::Uint8 blue  = 255*((color[idx2][2] - color[idx1][2])*localVal + color[idx1][2]);

    return sf::Color(red, green, blue);
}

/**
 * Main window loop.
 */
void Drawer::run()
{
    while(window.isOpen())
    {
        update();
        render();
    }
}
