#include "States/PlayState.h"
#include "Core/EventManager.h"
#include "Core/FontManager.h"
#include "Core/SoundManager.h"
#include "Core/StateManager.h"
#include "Core/TextureManager.h"
#include "Core/WindowManager.h"
#include "States/GameOverState.h"
#include "States/PauseState.h"
#include "Util/Path.h"
#include <cstdlib>
#include <fstream>
#include <functional>
#include <memory>

namespace {
float DISTANCE_TO_WIN = 1000;
float DISTANCE_CONST = 100;
float GHOST_ABOVE_THRESHOLD = 0.6f;
}

PlayState::PlayState()
  : State(StateID::Play)
  , car()
  , background()
  , distanceCovered(0.0f)
  , ghostSpawner(0.1f, 0.9f, 0.6f)
{
    gameClock.restart();

    auto& textFont = FontManager::getInstance().getFont(FontID::TEXT);

    timeText.setFont(textFont);
    timeText.setCharacterSize(20);
    timeText.setFillColor(sf::Color::White);
    sf::Vector2u windowSize = WindowManager::getWindow().getSize();
    timeText.setPosition(0.05f * windowSize.x,
                         0.05f * windowSize.y); // 5% padding from top left

    distanceText.setFont(textFont);
    distanceText.setCharacterSize(20);
    distanceText.setFillColor(sf::Color::White);
    distanceText.setPosition(0.05f * windowSize.x, 0.1f * windowSize.y);
}

void
PlayState::init()
{
    auto& eventManager = EventManager::getInstance();
    auto& soundManager = SoundManager::getInstance();
    auto& textureManager = TextureManager::getInstance();

    textureManager.loadTexture(TextureID::ATLAS0,
                               Util::getExecutablePath() / "assets/atlas0.png");
    textureManager.loadTexture(TextureID::ATLAS1,
                               Util::getExecutablePath() / "assets/atlas1.png");
    textureManager.loadTexture(TextureID::ATLAS2,
                               Util::getExecutablePath() / "assets/atlas2.png");
    textureManager.loadTexture(TextureID::GHOST,
                               Util::getExecutablePath() / "assets/ghost.png");
    textureManager.loadTexture(TextureID::CAR0,
                               Util::getExecutablePath() / "assets/car0.png");

    soundManager.loadSound(SoundID::THREE,
                           Util::getExecutablePath() / "assets/three.ogg");
    soundManager.loadSound(SoundID::TWO,
                           Util::getExecutablePath() / "assets/two.ogg");
    soundManager.loadSound(SoundID::ONE,
                           Util::getExecutablePath() / "assets/one.ogg");
    soundManager.loadSound(SoundID::GO,
                           Util::getExecutablePath() / "assets/go.ogg");

    soundManager.getMusic().openFromFile(Util::getExecutablePath() /
                                         "assets/music.ogg");
    soundManager.getMusic().setLoop(true);

    eventManager.addListener(
      StateID::Play,
      sf::Event::Resized,
      std::bind(&WindowManager::resizeWindow, std::placeholders::_1));

    eventManager.addListener(
      StateID::Play,
      sf::Event::KeyPressed,
      std::bind(&WindowManager::toggleFullScreen, std::placeholders::_1));

    eventManager.addListener(
      StateID::Play,
      sf::Event::Closed,
      std::bind(&PlayState::handleEvents, this, std::placeholders::_1));

    eventManager.addListener(
      StateID::Play,
      sf::Event::KeyPressed,
      std::bind(&PlayState::handleEvents, this, std::placeholders::_1));

    eventManager.addListener(
      StateID::Play,
      sf::Event::KeyPressed,
      std::bind(&PlayerCar::handleEvents, &car, std::placeholders::_1));

    eventManager.addListener(
      StateID::Play,
      sf::Event::KeyReleased,
      std::bind(&PlayerCar::handleEvents, &car, std::placeholders::_1));

    eventManager.addListener(
      StateID::Play,
      sf::Event::Resized,
      std::bind(&PlayerCar::handleEvents, &car, std::placeholders::_1));

    eventManager.addListener(
      StateID::Play,
      sf::Event::Resized,
      std::bind(&Background::handleEvents, &background, std::placeholders::_1));

    background.init();
    car.init();

    soundManager.playSound(SoundID::THREE);
    soundManager.playSound(SoundID::TWO);
    soundManager.playSound(SoundID::ONE);
    soundManager.playSound(SoundID::GO);

    soundManager.getMusic().play();
}

void
PlayState::update(const sf::Time& deltaTime)
{
    car.update(deltaTime);
    background.update(deltaTime, car);
    ghostSpawner.update(
      deltaTime, car.getPositionPercentage(), car.getWidthPercentage());

    bool ghostDetected = false;

    for (const auto& ghost : ghostSpawner.getGhosts()) {
        sf::Vector2f ghostPosition = ghost->getPositionPercentage();
        sf::Vector2f carPosition = car.getPositionPercentage();

        if (ghostPosition.x >= carPosition.x - car.getWidthPercentage() / 6 &&
            ghostPosition.x <= carPosition.x + car.getWidthPercentage() / 6 &&
            ghostPosition.y < carPosition.y) {
            ghostDetected = true;
            break;
        }
    }

    if (ghostDetected) {
        if (!isGhostAbove) {
            isGhostAbove = true;
            ghostAboveTimer.restart();
        } else if (ghostAboveTimer.getElapsedTime().asSeconds() >=
                   GHOST_ABOVE_THRESHOLD) {
            car.setVelocity(-3 * car.getForwardVelocity());
        }
    } else {
        isGhostAbove = false;
    }

    sf::Time elapsedTime = gameClock.getElapsedTime();
    int minutes = static_cast<int>(elapsedTime.asSeconds()) / 60;
    int seconds = static_cast<int>(elapsedTime.asSeconds()) % 60;

    float sign;
    if (car.getForwardVelocity() > 0) sign = +1;
    if (car.getForwardVelocity() < 0) sign = -3;
    if (car.getForwardVelocity() == 0) sign = 0;
    distanceCovered += sign * 
      deltaTime.asSeconds() * DISTANCE_CONST;

    if (distanceCovered >= DISTANCE_TO_WIN) {
        snapshot(TextureManager::getInstance().getRenderTexture());
        StateManager::getInstance().pushState(
          std::make_unique<GameOverState>());
    }

    timeText.setString("Time: " + std::to_string(minutes) + ":" +
                       std::to_string(seconds));
    float remainingDistance = DISTANCE_TO_WIN - distanceCovered;
    distanceText.setString(
      "Distance Remaining: " + std::to_string(static_cast<int>(remainingDistance)) + " m");
}

void
PlayState::render(sf::RenderTarget& target)
{
    target.draw(background);
    target.draw(car);

    for (const auto& ghost : ghostSpawner.getGhosts()) {
        target.draw(*ghost);
    }

    target.draw(timeText);
    target.draw(distanceText);
}

void
PlayState::snapshot(sf::RenderTexture& renderTexture)
{
    auto windowSize = WindowManager::getWindow().getSize();
    renderTexture.create(windowSize.x, windowSize.y);
    render(renderTexture);
    renderTexture.display();
}

void
PlayState::handleEvents(const sf::Event& event)
{
    switch (event.type) {
        case sf::Event::KeyPressed:
            switch (event.key.code) {
                case sf::Keyboard::Escape:
                    snapshot(TextureManager::getInstance().getRenderTexture());
                    StateManager::getInstance().pushState(
                      std::make_unique<PauseState>());
                    break;
                default:
                    break;
            }
            break;
        case sf::Event::Closed:
            WindowManager::getWindow().close();
            break;
        default:
            break;
    }
}

void
PlayState::exit()
{
    auto& eventManager = EventManager::getInstance();
    eventManager.removeAllListeners(StateID::Play, sf::Event::KeyPressed);
    eventManager.removeAllListeners(StateID::Play, sf::Event::Resized);
    eventManager.removeAllListeners(StateID::Play, sf::Event::Closed);

    std::ofstream file(Util::getExecutablePath() / "highscores.txt",
                       std::ios::app);
    if (file.is_open()) {
        sf::Time elapsedTime = gameClock.getElapsedTime();
        int minutes = static_cast<int>(elapsedTime.asSeconds()) / 60;
        int seconds = static_cast<int>(elapsedTime.asSeconds()) % 60;
        file << "Time: " << minutes << ":" << (seconds < 10 ? "0" : "")
             << seconds << "\n";
        file.close();
    }
}
