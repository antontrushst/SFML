#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

// Window properties:
int WindowWidth = 0, WindowHeight = 0;

// Font properties:
std::string fontPath = {};
int fontSize = 0;
int fontColor[3] = {0, 0, 0};

std::vector<std::string> circleNames;
std::vector<sf::Vector2f> circleSpeed;

std::vector<std::string> rectangleNames;
std::vector<sf::Vector2f> rectangleSpeed;

void ReadConfigFile(std::vector<sf::CircleShape> &Circles,
                    std::vector<sf::RectangleShape> &Rectangles);

int main()
{
    std::vector<sf::CircleShape> Circles;
    std::vector<sf::RectangleShape> Rectangles;
    ReadConfigFile(Circles, Rectangles);
    
    sf::Font font;
    if (!font.loadFromFile(fontPath))
    {
        std::cerr << "Coldn't load the font!\n";
        exit(1);
    }

    std::vector<sf::Text> circlesText;
    std::vector<sf::Text> rectanglesText;
    for (int i = 0; i < Circles.size(); i++)
    {
        sf::Text text(circleNames[i], font, fontSize);
        text.setFillColor(sf::Color(fontColor[0], fontColor[1], fontColor[2]));
        auto bounds = text.getGlobalBounds();
        text.setOrigin(bounds.width / 2, bounds.height / 2);
        circlesText.push_back(text);
    }

    for (int i = 0; i < Rectangles.size(); i++)
    {
        sf::Text text(rectangleNames[i], font, fontSize);
        text.setFillColor(sf::Color(fontColor[0], fontColor[1], fontColor[2]));
        auto bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2, text.getCharacterSize() / 2);
        rectanglesText.push_back(text);
    }

    sf::RenderWindow window(sf::VideoMode(WindowWidth, WindowHeight), "My SFML window!");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        int counterC = 0;
        for (auto &i : Circles)
        {
            i.setPosition(i.getPosition().x - circleSpeed[counterC].x
                        , i.getPosition().y - circleSpeed[counterC].y);
            
            if (i.getPosition().x <= i.getRadius())
            {
                i.setPosition(i.getRadius(), i.getPosition().y);
                circleSpeed[counterC].x *= -1;
            }

            if ((i.getPosition().x + i.getRadius()) >= WindowWidth)
            {
                i.setPosition(WindowWidth - i.getRadius(), i.getPosition().y);
                circleSpeed[counterC].x *= -1;
            }

            if (i.getPosition().y <= i.getRadius())
            {
                i.setPosition(i.getPosition().x, i.getRadius());
                circleSpeed[counterC].y *= -1;
            }

            if ((i.getPosition().y + i.getRadius()) >= WindowHeight)
            {
                i.setPosition(i.getPosition().x, WindowHeight - i.getRadius());
                circleSpeed[counterC].y *= -1;
            }

            circlesText[counterC].setPosition(i.getPosition().x, i.getPosition().y);

            window.draw(i);
            window.draw(circlesText[counterC]);

            counterC++;
        }

        int counterR = 0;
        for (auto &i : Rectangles)
        {
            i.setPosition(i.getPosition().x - rectangleSpeed[counterR].x
                        , i.getPosition().y - rectangleSpeed[counterR].y);
            rectanglesText[counterR].setPosition(i.getPosition().x, i.getPosition().y);

            if (i.getPosition().x - (i.getSize().x / 2) <= 0)
            {
                i.setPosition(0 + (i.getSize().x / 2), i.getPosition().y);
                rectangleSpeed[counterR].x *= -1;
            }

            if ((i.getPosition().x + (i.getSize().x / 2)) >= WindowWidth)
            {
                i.setPosition(WindowWidth - (i.getSize().x / 2), i.getPosition().y);
                rectangleSpeed[counterR].x *= -1;
            }

            if (i.getPosition().y - (i.getSize().y / 2) <= 0)
            {
                i.setPosition(i.getPosition().x, 0 + (i.getSize().y / 2));
                rectangleSpeed[counterR].y *= -1;
            }

            if ((i.getPosition().y + (i.getSize().y / 2)) >= WindowHeight)
            {
                i.setPosition(i.getPosition().x, WindowHeight - (i.getSize().y / 2));
                rectangleSpeed[counterR].y *= -1;
            }


            window.draw(i);
            window.draw(rectanglesText[counterR]);
            counterR++;
        }
        window.display();

    }

    return 0;
}

void ReadConfigFile(std::vector<sf::CircleShape> &Circles
                   ,std::vector<sf::RectangleShape> &Rectangles)
{
    // Create temporary rectangle and circle objects to get properties
    // and fill vectors
    sf::CircleShape aCircle;
    sf::RectangleShape aRectangle;

    // Get the list of key words
    enum KeyWord { Window, Font, Circle, Rectangle };
    std::map<std::string, KeyWord> mp;
    mp["Window"] = KeyWord::Window;
    mp["Font"] = KeyWord::Font;
    mp["Circle"] = KeyWord::Circle;
    mp["Rectangle"] = KeyWord::Rectangle;

    // Exit program with error if cannot open the file
    std::ifstream inputFile("config.txt");
    if (!inputFile.is_open())
    {
        std::cerr << "Couldn't open \"config.txt\" file!\n";
        exit(1);
    }
    
    std::string temp;
    while (inputFile >> temp)
    {
        std::map<std::string, KeyWord>::iterator it;
        it = mp.find(temp);
        if (it != mp.end())
        {
            switch (it->second)
            {
                case KeyWord::Window:
                {
                    inputFile >> WindowWidth >> WindowHeight; break;
                }
                case KeyWord::Font:
                {
                    inputFile >> fontPath >> fontSize
                    >> fontColor[0] >> fontColor[1] >> fontColor[2]; break;
                }
                case KeyWord::Circle:
                {
                    std::string name;
                    int X = 0, Y = 0, Rad = 0;
                    float sX = 0.f, sY = 0.f;
                    int R = 0, G = 0, B= 0;
                    inputFile >> name >> X >> Y >> sX >> sY >> R >> G >> B >> Rad;
                    circleNames.push_back(name);
                    aCircle.setPosition(X, Y);
                    sf::Vector2f speed(sX, sY);
                    circleSpeed.push_back(speed);
                    aCircle.setFillColor(sf::Color(R, G, B));
                    aCircle.setRadius(Rad);
                    auto bounds = aCircle.getLocalBounds();
                    aCircle.setOrigin(bounds.width / 2, bounds.height / 2);
                    Circles.push_back(aCircle);
                    break;
                }
                case KeyWord::Rectangle:
                {
                    std::string name;
                    int X = 0, Y = 0;
                    float W = 0.f, H = 0.f;
                    float sX = 0.f, sY = 0.f;
                    int R = 0, G = 0, B = 0;
                    inputFile >> name >> X >> Y >> sX >> sY >> R >> G >> B
                              >> W >> H;
                    sf::Vector2f size(W, H);
                    rectangleNames.push_back(name);
                    aRectangle.setPosition(X, Y);
                    sf::Vector2f speed(sX, sY);
                    rectangleSpeed.push_back(speed);
                    aRectangle.setFillColor(sf::Color(R, G, B));
                    aRectangle.setSize(size);
                    auto bounds = aRectangle.getLocalBounds();
                    aRectangle.setOrigin(bounds.width / 2, bounds.height / 2);
                    Rectangles.push_back(aRectangle);
                    break;
                }
                default: continue;
            }
        }
    }
}
