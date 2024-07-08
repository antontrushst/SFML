#include "game.h"
#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include "genFloat.h"

Game::Game(const std::string& config) { init(config); }

void Game::init(const std::string& config)
{
    enum KeyWord { Window, Font, Player, Enemy, Bullet };
    std::map<std::string, KeyWord> mp;
    mp["Window"] = KeyWord::Window;
    mp["Font"]   = KeyWord::Font;
    mp["Player"] = KeyWord::Player;
    mp["Enemy"]  = KeyWord::Enemy;
    mp["Bullet"] = KeyWord::Bullet;

    std::ifstream fin("config.txt");
    if (!fin.is_open())
    {
        std::cerr << "Couldn't open \"config.txt\" file!\n";
        exit(1);
    }

    std::string temp;
    while (fin >> temp)
    {
        std::map<std::string, KeyWord>::iterator it;
        it = mp.find(temp);
        switch (it->second)
        {
            case KeyWord::Window:
                fin >> m_windowConfig.WindowWidth >> m_windowConfig.WindowHeight
                    >> m_windowConfig.FrameLimit  >> m_windowConfig.FullScreen;
                break;
            case KeyWord::Font:
                fin >> m_fontConfig.Path   >> m_fontConfig.Size
                    >> m_fontConfig.ColorR >> m_fontConfig.ColorG
                    >> m_fontConfig.ColorB;
                break;
            case KeyWord::Player:
                fin >> m_playerConfig.SR >> m_playerConfig.CR
                    >> m_playerConfig.FR >> m_playerConfig.FG
                    >> m_playerConfig.FB >> m_playerConfig.OR
                    >> m_playerConfig.OG >> m_playerConfig.OB
                    >> m_playerConfig.OT >> m_playerConfig.V
                    >> m_playerConfig.S;
                break;
            case KeyWord::Enemy:
                fin >> m_enemyConfig.SR   >> m_enemyConfig.CR
                    >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX
                    >> m_enemyConfig.OR   >> m_enemyConfig.OG
                    >> m_enemyConfig.OB   >> m_enemyConfig.OT
                    >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX
                    >> m_enemyConfig.L    >> m_enemyConfig.SI;
                break;
            case KeyWord::Bullet:
                fin >> m_bulletConfig.SR >> m_bulletConfig.CR
                    >> m_bulletConfig.FR >> m_bulletConfig.FG
                    >> m_bulletConfig.FB >> m_bulletConfig.OR
                    >> m_bulletConfig.OG >> m_bulletConfig.OB
                    >> m_bulletConfig.OT >> m_bulletConfig.V
                    >> m_bulletConfig.L  >> m_bulletConfig.S;
                break;
            default: continue;
        }
    }

    if (m_windowConfig.FullScreen)
    {
        m_window.create(sf::VideoMode(m_windowConfig.WindowWidth,
                                    m_windowConfig.WindowHeight), 
                                    "Assignment 2", sf::Style::Fullscreen);
    }
    else
    {
        m_window.create(sf::VideoMode(m_windowConfig.WindowWidth,
                                    m_windowConfig.WindowHeight), 
                                    "Assignment 2");
    }
    m_window.setFramerateLimit(m_windowConfig.FrameLimit);

    spawnPlayer();
    // spawnEnemy();
    // spawnEnemy();
    // m_entities.update();
    // m_entities.getEntities("enemy")[1]->cTransform->position.x += 300;
    // m_entities.getEntities("enemy")[1]->cTransform->position.y += 30;
    // m_entities.getEntities("enemy")[1]->cTransform->velocity = 0.f;
}

void Game::run()
{
    while (m_running)
    {        
        m_entities.update();

        if (!m_paused)
        {
            sEnemySpawner();
            sMovement();
            // sCollision();
        }

        sUserInput();
        sRender();

        m_currentFrame++;
    }
    
}

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

void Game::spawnPlayer()
{
    auto entity = m_entities.addEntity("player");

    float mid_x = m_window.getSize().x / 2.f,
          mid_y = m_window.getSize().y / 2.f;
    entity->cTransform = std::make_shared<CTransform>(Vec2(mid_x, mid_y),
                                                      Vec2(0, 0), 0.f);
    entity->cShape = std::make_shared<CShape>(m_playerConfig.SR,
                                              m_playerConfig.V,
                                              sf::Color(m_playerConfig.FR,
                                                        m_playerConfig.FG,
                                                        m_playerConfig.FB),
                                              sf::Color(m_playerConfig.OR,
                                                        m_playerConfig.OG,
                                                        m_playerConfig.OB),
                                              m_playerConfig.OT);
    entity->cInput = std::make_shared<CInput>();
    m_player = entity;
}

void Game::spawnEnemy()
{
    m_lastEnemySpawnTime = m_currentFrame;
    auto entity = m_entities.addEntity("enemy");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<>distribX(m_enemyConfig.CR,
                                            m_window.getSize().x - m_enemyConfig.CR);
    std::uniform_int_distribution<>distribY(m_enemyConfig.CR,
                                            m_window.getSize().y - m_enemyConfig.CR);
    std::uniform_int_distribution<>distribV(m_enemyConfig.VMIN, m_enemyConfig.VMAX);

    float randPosX = (float)distribX(gen),
          randPosY = (float)distribY(gen),
          randSpdX = genFloat(m_enemyConfig.SMIN, m_enemyConfig.SMAX),
          randSpdY = genFloat(m_enemyConfig.SMIN, m_enemyConfig.SMAX);
    int   randVert = distribV(gen);

    entity->cTransform = std::make_shared<CTransform>(Vec2(randPosX, randPosY),
                                                      Vec2(randSpdX, randSpdY),
                                                      0.f);
    entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, randVert,
                                              sf::Color(m_enemyConfig.OR - 150,
                                                        m_enemyConfig.OG,
                                                        m_enemyConfig.OB),
                                              sf::Color(m_enemyConfig.OR,
                                                        m_enemyConfig.OG,
                                                        m_enemyConfig.OB),
                                              m_enemyConfig.OT);
}

// void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
// {

// }

void Game::sRender()
{
    m_window.clear();

    m_player->cShape->circle.setPosition(m_player->cTransform->position.x,
                                         m_player->cTransform->position.y);
    m_player->cTransform->angle += 1.f;
    m_player->cShape->circle.setRotation(m_player->cTransform->angle);

    if (m_entities.getEntities("enemy").size() != 0)
    {
        for (auto& enemy : m_entities.getEntities("enemy"))
        {
            enemy->cTransform->angle += 1.f;
            enemy->cShape->circle.setRotation(enemy->cTransform->angle);
            m_window.draw(enemy->cShape->circle);
        }
    }

    m_window.draw(m_player->cShape->circle);
    m_window.display();
}

void Game::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            m_running = false;

        if (event.type == sf::Event::KeyPressed    &&
            event.key.code == sf::Keyboard::Escape &&
            m_player->cInput->pause == false)
        {
            m_player->cInput->pause = true;
            if (!m_paused)
                setPaused(true);
            else
                setPaused(false);
        }
        else if (event.type == sf::Event::KeyReleased   &&
                 event.key.code == sf::Keyboard::Escape &&
                 m_player->cInput->pause == true)
        {
            m_player->cInput->pause = false;
        }

        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W: m_player->cInput->up    = true; continue;
                case sf::Keyboard::S: m_player->cInput->down  = true; continue;
                case sf::Keyboard::A: m_player->cInput->left  = true; continue;
                case sf::Keyboard::D: m_player->cInput->right = true; continue;
                default: break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W: m_player->cInput->up    = false; continue;
                case sf::Keyboard::S: m_player->cInput->down  = false; continue;
                case sf::Keyboard::A: m_player->cInput->left  = false; continue;
                case sf::Keyboard::D: m_player->cInput->right = false; continue;
                default: break;
            }
        }

        // if (event.type == sf::Event::MouseButtonPressed)
        // {
        //     if (event.mouseButton.button == sf::Mouse::Left)
        //     {
        //         std::cout << "Left Mouse Button clicked\n";
        //         spawnBullet(m_player, Vec2(event.mouseButton.x,
        //                     event.mouseButton.y));
        //     }

        //     if (event.mouseButton.button == sf::Mouse::Right)
        //         std::cout << "Right Mouse Button clicked\n";
        // }
    }
}

void Game::sMovement()
{
    m_player->cTransform->velocity = { 0.f, 0.f };

    if (m_player->cInput->up    && !m_player->cInput->down)
        m_player->cTransform->velocity.y = m_playerConfig.S * -1;
    if (m_player->cInput->down  && !m_player->cInput->up)
        m_player->cTransform->velocity.y = m_playerConfig.S;
    if (m_player->cInput->left  && !m_player->cInput->right)
        m_player->cTransform->velocity.x = m_playerConfig.S * -1;
    if (m_player->cInput->right && !m_player->cInput->left)
        m_player->cTransform->velocity.x = m_playerConfig.S;

    m_player->cTransform->position.x += m_player->cTransform->velocity.x;
    m_player->cTransform->position.y += m_player->cTransform->velocity.y;

    if (m_entities.getEntities("enemy").size() != 0)
    {
        for (auto& enemy : m_entities.getEntities("enemy"))
        {
            if (enemy->cTransform->position.x <= enemy->cShape->circle.getRadius())
            {
                enemy->cTransform->position.x = enemy->cShape->circle.getRadius() + 1;
                enemy->cTransform->velocity.x *= -1;
            }
            else if (enemy->cTransform->position.x >= 
                     m_window.getSize().x - enemy->cShape->circle.getRadius())
            {
                enemy->cTransform->position.x =
                m_window.getSize().x - (enemy->cShape->circle.getRadius() + 1);
                enemy->cTransform->velocity.x *= -1;
            }
            else if (enemy->cTransform->position.y <= enemy->cShape->circle.getRadius())
            {
                enemy->cTransform->position.y = enemy->cShape->circle.getRadius() + 1;
                enemy->cTransform->velocity.y *= -1;
            }
            else if (enemy->cTransform->position.y >=
                     m_window.getSize().y - enemy->cShape->circle.getRadius())
            {
                enemy->cTransform->position.y =
                m_window.getSize().y - (enemy->cShape->circle.getRadius() + 1);
                enemy->cTransform->velocity.y *= -1;
            }

            enemy->cTransform->position.x += enemy->cTransform->velocity.x;
            enemy->cTransform->position.y += enemy->cTransform->velocity.y;
            enemy->cShape->circle.setPosition(enemy->cTransform->position.x,
                                              enemy->cTransform->position.y);
        }
    }
}

// void Game::sLifespan()
// {

// }

void Game::sEnemySpawner()
{
    if (m_currentFrame > m_lastEnemySpawnTime + m_enemyConfig.SI)
        spawnEnemy();
}

void Game::sCollision()
{
    float minDist = (float)m_enemyConfig.CR * 2;

    for (auto e1 : m_entities.getEntities("enemy"))
    {
        for (auto e2 : m_entities.getEntities("enemy"))
        {
            if (e1 == e2)
                continue;

            Vec2 distVec = { std::fabs(e1->cTransform->position.x -
                                           e2->cTransform->position.x),
                                 std::fabs(e1->cTransform->position.y -
                                           e2->cTransform->position.y) };
            float pythagorus = sqrtf(distVec.x * distVec.x + distVec.y * distVec.y);
            float theta = atan2f(distVec.y, distVec.x);
            float momX = e1->cTransform->mass * e1->cTransform->velocity.x;
            float momY = 0;
            std::cout << "momX: " << momX << "\tmomY: " << momY << "\n\n";

            if (pythagorus <= minDist)
                e1->cTransform->velocity *= -1;

        }
    }
}

// void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos)
// {
//     auto bullet = m_entities.addEntity("bullet");
//     //bullet->cTransform = std::make_shared<CTransform>(target, Vec2(0, 0), 0);
//     bullet->cShape = std::make_shared<CShape>(10, 8, sf::Color(255, 255, 255),
//                                               sf::Color(255, 0, 0), 2);
// }

// void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
// {

// }
