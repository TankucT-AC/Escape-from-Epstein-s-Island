#include "Bullet.hpp"
#include "Entity.hpp"
#include "config.hpp"

class Enemy : public Entity
{
private:
    float shootTime;
    float shootDelay;
    float health;
    bool dead;
public:
    Enemy(const sf::Texture& InitTexture, 
        float InitSpeed = config::ENEMY_DEFAULT_SPEED, 
        float InitShootDelay = config::PLAYER_DEFAULT_SHOOT_DELAY,
        float InitHealth = config::ENEMY_DEFAULT_HEALTH
    );

    virtual void move(const sf::Time& dt, const sf::Vector2<float>& offset) override;
    virtual void update(const sf::Time& dt, sf::RenderWindow& window) override {};
    virtual void draw(sf::RenderWindow& window) override;
    virtual sf::Vector2<float> getPosition() const override;

    virtual void update(const sf::Time& dt, const sf::Vector2<float>& playerPos, sf::RenderWindow& window);
    virtual void getReceivedDamage(float damage);
    bool isEnemyAlive() const { return !dead; }
    bool isBulletCollision(const Bullet& bullet);
};