#include <iostream>
#include <SFML/Graphics.hpp>
#include "game.h"

int main()
{
    Game g("config.txt");
    g.run();
    return 0;
}
