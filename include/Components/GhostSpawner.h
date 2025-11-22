#ifndef GHOSTSPAWNER_H
#define GHOSTSPAWNER_H

#include <SFML/Graphics.hpp>
#include <deque>
#include <memory>

class Ghost : public sf::Drawable
{
  public:
    Ghost(float x, float y);
    float lifetime;
    sf::FloatRect getBounds() const;
    void update(const sf::Time& deltaTime);
    sf::Vector2f getPositionPercentage() const;

  private:
    sf::Sprite sprite;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

class GhostSpawner
{
  public:
    GhostSpawner(float minX, float maxX, float fixedY);
    void update(const sf::Time& deltaTime,
                const sf::Vector2f& carPosition,
                const float carWidthPercentage);
    void spawnGhost(const sf::Vector2f& carPosition,
                    const float carWidthPercentage);
    const std::deque<std::unique_ptr<Ghost>>& getGhosts() const;

  private:
    std::deque<std::unique_ptr<Ghost>> ghosts;
    float minX;
    float maxX;
    float fixedY;
    sf::Clock spawnClock;
    float spawnInterval;
};

#endif // GHOSTSPAWNER_H
