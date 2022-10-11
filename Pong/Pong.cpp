#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <functional>
#include <math.h>

#define BIN_STEP_RAND (2*(rand() % 2))-1;

int points = 0;
int maxVelMag = 60;
sf::Vector2f velocityVector(0, 0);

sf::Vector2f generateVelVector() {
    sf::Vector2f newVel;
    newVel.x = ((double)rand() /RAND_MAX)* sqrt(maxVelMag);
    newVel.y = (sqrt(maxVelMag - (newVel.x * newVel.x)));
    newVel.y *= BIN_STEP_RAND;
    if (newVel.x <= 2) {
        return generateVelVector();
    }
    return newVel;
};

void collision(sf::CircleShape* ball, sf::RectangleShape* paddle, sf::Sound* sound) {
    while (true) {
        if (ball->getPosition().x <= 0) {
            if (velocityVector.x < 0) {
                velocityVector.x *= -1;
                sound->play();
                velocityVector = generateVelVector();
                points++;
            }
        }if (ball->getPosition().y <= 0) {
            if (velocityVector.y < 0) {
                velocityVector.y *= -1;
                sound->play();
            }
        }if (ball->getPosition().y > 900 - 2*ball->getRadius()) {
            if (velocityVector.y > 0) {
                velocityVector.y *= -1;
                sound->play();
            }
        }if (ball->getPosition().x > 1000 - 2*ball->getRadius()) {
            ball->setPosition(sf::Vector2f(200, 450));
            velocityVector = generateVelVector();
            points = 0;
        }
        if (ball->getPosition().y <= paddle->getPosition().y+100 && ball->getPosition().y >= paddle->getPosition().y) {
            if (ball->getPosition().x + 2 * ball->getRadius() >= 920 && ball->getPosition().x <= 950) {
                sound->play();
                if (velocityVector.x > 0) {
                    velocityVector.x *= -1;
                    maxVelMag += 3;
                    if (maxVelMag > 150) {
                        maxVelMag = 150;
                    }
                }
            }
        }

    }
    delete ball, paddle, sound;
}

void controls(sf::RectangleShape* paddle) {
    while (true) {
        sf::sleep(sf::milliseconds(1000 / 60));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            paddle->setPosition(sf::Vector2f(
                paddle->getPosition().x,
                paddle->getPosition().y-8
            ));
            if (paddle->getPosition().y <= 0) {
                paddle->setPosition(sf::Vector2f(
                    paddle->getPosition().x,
                    0
                ));
            }
        }if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            paddle->setPosition(sf::Vector2f(
                paddle->getPosition().x,
                paddle->getPosition().y +8
            ));
            if (paddle->getPosition().y >= 800) {
                paddle->setPosition(sf::Vector2f(
                    paddle->getPosition().x,
                    800
                ));
            }
        }
    }
    delete paddle;
}

int main()
{
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(1000, 900), "PONG SFML EDITION");
    window.setVerticalSyncEnabled(true);

    std::ostringstream ss;
    ss << points;

    sf::Font font;
    if (!font.loadFromFile("RobotoMono-Bold.ttf")) { return -1; }
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(60);
    text.setFillColor(sf::Color::White);
    text.setString(ss.str());
    text.setPosition(sf::Vector2f(500 - (text.getLocalBounds().width / 2), 15));

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("bounce-clap.wav")) { return -1; }
    sf::Sound sound;
    sound.setBuffer(buffer);

    sf::RectangleShape playerPaddle(sf::Vector2f(30, 100));
    playerPaddle.setFillColor(sf::Color::White);
    playerPaddle.setPosition(sf::Vector2f(920, 400));  

    sf::CircleShape ball(10);
    ball.setFillColor(sf::Color::White);
    ball.setPosition(sf::Vector2f(200, 450));
    velocityVector = generateVelVector();

    sf::RectangleShape killLine(sf::Vector2f(10, 900));
    killLine.setPosition(sf::Vector2f(990, 0));
    killLine.setFillColor(sf::Color::Red);

    sf::Thread collisionThread(std::bind(&collision, &ball, &playerPaddle, &sound));
    collisionThread.launch();
    sf::Thread controlThread(std::bind(&controls, &playerPaddle));
    controlThread.launch();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        std::ostringstream ss;
        ss << points;
        text.setString(ss.str());
        window.draw(text);
        window.draw(playerPaddle);
        window.draw(ball);
        window.draw(killLine);
        ball.setPosition(sf::Vector2f(
            ball.getPosition().x + velocityVector.x,
            ball.getPosition().y + velocityVector.y
        ));
        window.display();
    }
    collisionThread.terminate();
    controlThread.terminate();
    return 0;
}