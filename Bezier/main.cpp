#include <iostream>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>

//   Interpolation function   ..................................................
int interpolate(int from, int to, float percent)
{
    int difference = to - from;
    return from + (difference * percent);
}

//   MAIN   ////////////////////////////////////////////////////////////////////
int main()
{
    //   Create a window   .....................................................
    sf::RenderWindow window(sf::VideoMode(800,600), "Bezier");

    //   Get X and Y for the middle of the window   ............................
    int midX = window.getSize().x/2,
        midY = window.getSize().y/2;

    //   Create a red circle to represent END point   ..........................
    sf::CircleShape pointA(7, 10);
    pointA.setOrigin(pointA.getRadius(), pointA.getRadius());
    pointA.setFillColor(sf::Color::Red);

    //   Create a green circle to represent CURVATURE point   ..................
    sf::CircleShape pointB(7, 10);
    pointB.setOrigin(pointB.getRadius(), pointB.getRadius());
    pointB.setPosition(100, 100);
    pointB.setFillColor(sf::Color::Green);
    
    //   main loop   ...........................................................
    while (window.isOpen())
    {
        //   Read the inputs   .................................................
        sf::Event event;
        while (window.pollEvent(event))
        {
            //   Close program input   .........................................
            if (event.type == sf::Event::Closed)
                window.close();

            //   Control END and CURVATURE point with Left and Right
            //   mouse buttons respectively   ..................................
            if (event.type == sf::Event::MouseButtonPressed)
            {
                switch (event.key.code)
                {
                    case sf::Mouse::Left:
                    {
                        int mouseX = sf::Mouse::getPosition(window).x,
                            mouseY = sf::Mouse::getPosition(window).y;
                        pointA.setPosition(mouseX, mouseY);
                        break;
                    }
                    case sf::Mouse::Right:
                    {
                        int mouseX = sf::Mouse::getPosition(window).x,
                            mouseY = sf::Mouse::getPosition(window).y;
                        pointB.setPosition(mouseX, mouseY);
                        break;
                    }
                    default: continue;
                }
            }
        }

        //   Get the distance between START point (middle of the window)
        //   and END point   ...................................................
        int pointA_x = pointA.getPosition().x,
            pointA_y = pointA.getPosition().y;
        sf::Vector2f diff(pointA_x - midX, pointA_y - midY);
        float dist = sqrtf(diff.x * diff.x + diff.y * diff.y);

        //   Create vector with circles that will form the line   ..............
        std::vector<sf::CircleShape> verts;
        for (int i(0); i < dist; i++)
            verts.push_back(sf::CircleShape(5, 10));

        //   Clear the window before drawing anything   ........................
        window.clear(sf::Color::White);

        int   count = 0;   // counts the number of processed circles in line   .
        float increment = (float)verts.size() / 100.f,   // get 1% of line   ...
              countPlus = increment,   // tells when to increase percentage   ..
              percents = 0.f;   // stores percentage of distance processed   ...

        //   Draw a bezier curve   .............................................
        for (auto& v : verts)
        {
            //   Interpolate from middle of the window (START point)
            //   to pointB (CURVATURE point)   .................................
            int xa = interpolate(midX, pointB.getPosition().x, percents),
                ya = interpolate(midY, pointB.getPosition().y, percents);
            //   Interpolate from pointB to pointA (END point)   ...............
            int xb = interpolate(pointB.getPosition().x, pointA_x, percents),
                yb = interpolate(pointB.getPosition().y, pointA_y, percents);
            //   Interpolate between two interpolated vectors   ................
            int x = interpolate(xa, xb, percents),
                y = interpolate(ya, yb, percents);

            //   Set color and position for each circle in the line   ..........
            v.setOrigin(v.getRadius(), v.getRadius());
            v.setFillColor(sf::Color::Black);
            v.setPosition(x, y);

            //   Track the percentage of processed line   ......................
            if (count >= countPlus)
            {
                percents += 0.01f;
                countPlus += increment;
            }
            count ++;

            //   Draw each circle of the line   ................................
            window.draw(v);
        }

        //   Draw END and CURVATURE points and display the window   ............
        window.draw(pointA);
        window.draw(pointB);
        window.display();
    }

    return 0;
}
////////////////////////////////////////////////////////////////////////////////
