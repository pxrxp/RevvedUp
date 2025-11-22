#include "Components/GhostSpawner.h"
#include "Core/TextureManager.h"
#include "Core/WindowManager.h"

namespace {
std::size_t MAX_GHOSTS = 8;
float SPAWN_NEAR_CAR_BIAS = 0.1f;
int MAX_ATTEMPTS = 30;
float MAX_LIFETIME_GHOST = 1.0f;
float OFFSET_CONST = 0.5f;
}

Ghost::Ghost(float x, float y)
{
    auto& texManager = TextureManager::getInstance();
    auto windowSize = WindowManager::getWindow().getSize();
    sprite.setTexture(texManager.getTexture(TextureID::GHOST));
    sprite.setPosition(x * windowSize.x, y * windowSize.y);
}

void
Ghost::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(sprite);
}

void
Ghost::update(const sf::Time& deltaTime)
{
    lifetime += deltaTime.asSeconds();
}

sf::Vector2f
Ghost::getPositionPercentage() const
{
    sf::Vector2u windowSize = WindowManager::getWindow().getSize();
    return sf::Vector2f(sprite.getPosition().x / windowSize.x,
                        sprite.getPosition().y / windowSize.y);
}

sf::FloatRect
Ghost::getBounds() const
{
    return sprite.getGlobalBounds();
}

GhostSpawner::GhostSpawner(float minX, float maxX, float fixedY)
  : minX(minX)
  , maxX(maxX)
  , fixedY(fixedY)
  , spawnInterval(2.0f)
{
}

void
GhostSpawner::update(const sf::Time& deltaTime,
                     const sf::Vector2f& carPositionPercentage,
                     const float carWidthPercentage)
{
    if (spawnClock.getElapsedTime().asSeconds() >= spawnInterval) {
        spawnGhost(carPositionPercentage, carWidthPercentage);
        spawnClock.restart();
    }
    for (auto it = ghosts.begin(); it != ghosts.end();) {
        (*it)->update(deltaTime);
        if ((*it)->lifetime >= MAX_LIFETIME_GHOST) {
            it = ghosts.erase(it);
        } else {
            ++it;
        }
    }
}

const std::deque<std::unique_ptr<Ghost>>&
GhostSpawner::getGhosts() const
{
    return ghosts;
}

void
GhostSpawner::spawnGhost(const sf::Vector2f& carPositionPercentage,
                         const float carWidth)
{
    if (spawnClock.getElapsedTime().asSeconds() >= spawnInterval) {
        float x;
        bool validPosition = false;
        auto windowSize = WindowManager::getWindow().getSize();
        float carX = carPositionPercentage.x * windowSize.x;

        for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
            float randomOffset = (static_cast<float>(rand()) / RAND_MAX) *
                                 (maxX - minX) * SPAWN_NEAR_CAR_BIAS;
            x = carX + randomOffset;
            x = std::max(minX * windowSize.x, std::min(x, maxX * windowSize.x));

            validPosition = true;
            for (const auto& ghost : ghosts) {
                if (std::abs(ghost->getBounds().left - x) <
                    OFFSET_CONST * carWidth * windowSize.x) {
                    validPosition = false;
                    break;
                }
            }

            if (validPosition) {
                ghosts.push_back(
                  std::make_unique<Ghost>(x / windowSize.x, fixedY));
                if (ghosts.size() > MAX_GHOSTS) {
                    ghosts.pop_front();
                }
                return;
            }
        }
    }
}
