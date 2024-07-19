#pragma once

#include "entity.h"
#include "entityManager.h"
#include <SFML/Graphics.hpp>

struct WindowConfig  { int WindowWidth, WindowHeight, FrameLimit, FullScreen; };
struct FontConfig   { std::string Path; int Size, ColorR, ColorG, ColorB; };
struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig  { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
                      float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
    sf::RenderWindow    m_window;
    sf::Font            m_font;
    sf::Text            m_text;
    EntityManager       m_entities;
    WindowConfig        m_windowConfig;
    FontConfig          m_fontConfig;
    PlayerConfig        m_playerConfig;
    EnemyConfig         m_enemyConfig;
    BulletConfig        m_bulletConfig;
    
    int                 m_score = 0,
                        m_currentFrame = 0,
                        m_lastEnemySpawnTime = 0;

    bool                m_paused = false,
                        m_running = true;

    std::shared_ptr<Entity> m_player;

    void init(const std::string& config);
    void setPaused(bool paused);

    void sMovement(),
         sUserInput(),
         sLifespan(),
         sRender(),
         sEnemySpawner(),
         sCollision(),
         
         setScore(),
         spawnPlayer(),
         spawnEnemy(),
         spawnSmallEnemies(std::shared_ptr<Entity> entity),
         spawnBullet(),
         spawnSpecialWeapon();

public:
    Game(const std::string& config);
    void run();
};
