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


// class Circle
// {
//     std::string itsName;
//     int itsPosition_X = 0, itsPosition_Y = 0, itsRadius = 0;
//     float itsSpeed_X = 0.f, itsSpeed_Y = 0.f;
//     int itsColor[3] = {0, 0, 0};

// public:
//     Circle(std::string name, int x, int y, float sx, float sy, int r, int g
//                             ,int b, int rad)
//         : itsName(name), itsPosition_X(x), itsPosition_Y(y), itsSpeed_X(sx)
//         , itsSpeed_Y(sy), itsColor{r, g, b}, itsRadius(rad) {}
//     ~Circle() {}

//     void Print() const
//     {
//         std::cout << "Name: " << itsName << "\tX: " << itsPosition_X
//         << "\tY: " << itsPosition_Y << "\tSX: " << itsSpeed_X
//         << "\tSY: " << itsSpeed_Y << "\nR: " << itsColor[0] << "\tG: " << itsColor[1]
//         << "\tB: " << itsColor[2] << "\tRadius: " << itsRadius << "\n\n";
//     }

//     void SetProperties(std::string name, int x, int y, float sx, float sy,
//                        int r, int g, int b, int rad)
//     {
//         itsName = name;
//         itsPosition_X = x; itsPosition_Y = y;
//         itsSpeed_X = sx; itsSpeed_Y = sy;
//         itsColor[0] = r; itsColor[1] = g; itsColor[2] = b;
//         itsRadius = rad;
//     }

//     std::string GetName() const { return itsName; }
//     int GetPosition_X()  { return itsPosition_X; }
//     int GetPosition_Y()  { return itsPosition_Y; }
//     float GetSpeed_X() const { return itsSpeed_X; }
//     float GetSpeed_Y() const { return itsSpeed_Y; }
//     int GetColorR() const { return itsColor[0]; }
//     int GetColorG() const { return itsColor[1]; }
//     int GetColorB() const { return itsColor[2]; }
//     int GetRadius() const { return itsRadius; }

//     void ReverseSpeed_X() { itsSpeed_X *= -1; }
//     void ReverseSpeed_Y() { itsSpeed_Y *= -1; }
// };

// class Rectangle
// {
//     std::string itsName = {};
//     int itsPosition_X = 0, itsPosition_Y = 0, itsWidth = 0, itsHeight = 0;
//     float itsSpeed_X = 0.f, itsSpeed_Y = 0.f;
//     int itsColor[3] = {0, 0, 0};

// public:
//     Rectangle(std::string name, int x, int y, float sx, float sy, int r, int g
//                                ,int b, int w, int h)
//         : itsName(name), itsPosition_X(x), itsPosition_Y(y), itsSpeed_X(sx)
//         , itsSpeed_Y(sy), itsColor{r, g, b}, itsWidth(w), itsHeight(h) {}
//     ~Rectangle() {}

//     void Print() const
//     {
//         std::cout << "Name: " << itsName << "\tX: " << itsPosition_X
//                   << "\tY: " << itsPosition_Y << "\tSX: " << itsSpeed_X
//                   << "\tSY: " << itsSpeed_Y << "\nR: " << itsColor[0]
//                   << "\tG: " << itsColor[1] << "\tB: " << itsColor[2]
//                   << "\nWidth: " << itsWidth << "\tHeight: " << itsHeight << "\n\n";
//     }

//     void SetProperties(std::string name, int x, int y, float sx, float sy
//                                         ,int r, int g, int b, int w, int h)
//     {
//         itsName = name;
//         itsPosition_X = x; itsPosition_Y = y;
//         itsSpeed_X = sx; itsSpeed_Y = sy;
//         itsColor[0] = r; itsColor[1] = g, itsColor[2] = b;
//         itsWidth = w; itsHeight = h;
//     }
// };

void ReadConfigFile(std::vector<Circle> &Circles, std::vector<Rectangle> &Rectangles);

int main()
{
    std::vector<Circle> Circles;
    std::vector<Rectangle> Rectangles;

    ReadConfigFile(Circles, Rectangles);

    sf::RenderWindow window(sf::VideoMode(WindowWidth, WindowHeight), "My SFML window!");
    //window.setFramerateLimit(60);

    const int numOfCircles = Circles.size();
    const int numOfRectangles = Rectangles.size();

    std::vector<sf::CircleShape> CirclesArr;

    sf::CircleShape testCircle;
    testCircle.setRadius(Circles[0].GetRadius());
    auto bounds = testCircle.getLocalBounds();
    testCircle.setOrigin(bounds.width / 2, bounds.height / 2);
    testCircle.setPosition(Circles[0].GetPosition_X(), Circles[0].GetPosition_Y());
    testCircle.setFillColor(sf::Color(Circles[0].GetColorR(), Circles[0].GetColorG(), Circles[0].GetColorB()));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if ((testCircle.getPosition().x + testCircle.getRadius()) >= WindowWidth
            || testCircle.getPosition().x <= testCircle.getRadius())
        {
            Circles[0].ReverseSpeed_X();
        }
        if ((testCircle.getPosition().y + testCircle.getRadius()) >= WindowHeight
            || testCircle.getPosition().y <= testCircle.getRadius())
        {
            Circles[0].ReverseSpeed_Y();
        }

        window.clear();
        window.draw(testCircle);
        testCircle.setPosition(testCircle.getPosition().x - Circles[0].GetSpeed_X()
                             , testCircle.getPosition().y - Circles[0].GetSpeed_Y());
        window.display();
    }

    // std::cout << WindowWidth << "\t" << WindowHeight << "\n";
    // std::cout << fontPath << "\t" << fontSize << "\t" << fontColor[1] << "\n\n";

    // for (auto i : Circles)
    // {
    //     i.Print();
    //     std::cout << std::endl;
    // }

    //     for (auto i : Rectangles)
    // {
    //     i.Print();
    //     std::cout << std::endl;
    // }

    return 0;
}

void ReadConfigFile(std::vector<Circle> &Circles
                   ,std::vector<Rectangle> &Rectangles)
{
    Circle aCircle("temp", 0, 0, 0.f, 0.f, 0, 0, 0, 0);
    Rectangle aRectangle("temp", 0, 0, 0.f, 0.f, 0, 0, 0, 0, 0);

    // Get the list of key words:
    enum KeyWord { Window, Font, Circle, Rectangle };
    std::map<std::string, KeyWord> mp;
    mp["Window"] = KeyWord::Window;
    mp["Font"] = KeyWord::Font;
    mp["Circle"] = KeyWord::Circle;
    mp["Rectangle"] = KeyWord::Rectangle;

    std::ifstream inputFile("config.txt");
    if (!inputFile.is_open())
    {
        std::cerr << "Couldn't open \"config.txt\" file!\n";
        return;
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
                    int X = 0, Y = 0, R = 0;
                    float sX = 0.f, sY = 0.f;
                    int cR = 0, cG = 0, cB= 0;
                    inputFile >> name >> X >> Y >> sX >> sY >> cR >> cG >> cB >> R;
                    aCircle.SetProperties(name, X, Y, sX, sY, cR, cG, cB, R);
                    Circles.push_back(aCircle);
                    break;
                }
                case KeyWord::Rectangle:
                {
                    std::string name;
                    int X = 0, Y = 0, W = 0, H = 0;
                    float sX = 0.f, sY = 0.f;
                    int cR = 0, cG = 0, cB = 0;
                    inputFile >> name >> X >> Y >> sX >> sY >> cR >> cG >> cB
                              >> W >> H;
                    aRectangle.SetProperties(name, X, Y, sX, sY, cR, cG, cB, W, H);
                    Rectangles.push_back(aRectangle);
                    break;
                }
                default: continue;
            }
        }
    }
}
