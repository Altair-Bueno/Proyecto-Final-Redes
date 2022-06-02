#include "Ball.h"
#include "Transform.h"
#include "Renderer.h"
#include "../Entity.h"
#include "../../SDLUtils/SDLApp.h"
#include <iostream>

Ball::Ball(bool main) : Component(typeid(Ball).name())
{
    state = BallState::IDLE;
    this->mainBall = main;
    friction = 0.9995f;
    mininumThreshold = 1.5f;
}

Ball::~Ball() {}

void Ball::init()
{
    transform = this->ent->GetTransform();
    renderer = this->ent->GetComponent<Renderer>();
    multiplicador = 10.0f;
}

void Ball::update(float deltaTime)
{
    handleMain();

    if (state != BallState::MOVING)
        return;

    if (isOutOfBoundsX(deltaTime))
    {
        velocity.x = -velocity.x;
        this->playSound("collision");
    }
    if (isOutOfBoundsY(deltaTime))
    {
        velocity.y = -velocity.y;
        this->playSound("collision");
    }

    if (state != BallState::MOVING)
        return;

    // Update position
    transform->GetPosition().x += velocity.x * deltaTime;
    transform->GetPosition().y += velocity.y * deltaTime;

    // Simulate friction
    if (velocity.Magnitude() > mininumThreshold)
    {
        velocity.x *= friction;
        velocity.y *= friction;
    }
    else
    {
        velocity.x = 0.0f;
        velocity.y = 0.0f;
        state = BallState::IDLE;

        playerTurn = false;
        NetworkMessage msg(TURN_END);
        sendNetworkMessage(msg);
    }

    // handleBallScale();
}

void Ball::handleMain()
{
    if (!this->mainBall || !playerTurn)
        return;

    if (Input()->IsMouseButtonDown(0) && state == BallState::IDLE)
    {
        state = BallState::PRESSED;
        startPressPoint = Input()->GetMousePosition();
        // std::cout << "startPressPoint: " << startPressPoint.x << " " << startPressPoint.y << std::endl;
    }

    if (!Input()->IsMouseButtonDown(0) && state == BallState::PRESSED)
    {
        state = BallState::MOVING;
        setVelocity(startPressPoint, Input()->GetMousePosition());

        if (velocity.Magnitude() > mininumThreshold)
        {
            this->playSound("swing");
            BallHitMessage hitMsg(velocity.x, velocity.y);
            sendNetworkMessage(hitMsg);
        }
        // std::cout << "endPressPoint: " << Input()->GetMousePosition().x << " " << Input()->GetMousePosition().y << std::endl;
        // std::cout << "setVelocity: " << velocity.x << " " << velocity.y << std::endl;
    }
}

bool Ball::isOutOfBoundsX(float deltaTime)
{
    int screenWidth = this->ent->GetGame()->getWidth();
    int posToCheck = transform->GetPosition().x + velocity.x * deltaTime;
    int posToCheckRight = posToCheck + renderer->getWidth() / 2 * transform->GetScale().x;
    int posToCheckLeft = posToCheck - renderer->getWidth() / 2 * transform->GetScale().x;

    // If we are mainBall, we are retriscted to the left half of the screen
    if (mainBall)
        return posToCheckRight > screenWidth / 2 || posToCheckLeft < 0;
    else
        return posToCheckRight < screenWidth / 2 || posToCheckLeft > screenWidth;
}

bool Ball::isOutOfBoundsY(float deltaTime)
{
    int screenHeight = this->ent->GetGame()->getHeight();
    int posToCheck = transform->GetPosition().y + velocity.y * deltaTime;
    int posToCheckUP = posToCheck - transform->GetScale().y * renderer->getHeight() / 2;
    int posToCheckDOWN = posToCheck + transform->GetScale().y * renderer->getHeight() / 2;

    return posToCheckDOWN > screenHeight || posToCheckUP < 0;
}

float Ball::getDistance(Vector2D startPoint, Vector2D endPoint)
{
    return SDL_sqrt(SDL_pow(endPoint.x - startPoint.x, 2) + SDL_pow(endPoint.y - startPoint.y, 2));
}

void Ball::setVelocity(Vector2D startPoint, Vector2D endPoint)
{
    float distance = getDistance(startPoint, endPoint) * multiplicador;
    Vector2D dir = (endPoint - startPoint).Normalized();
    dir *= -distance;
    velocity = dir;
}

void Ball::receiveNetworkMessage(NetworkMessage& msg)
{
    if (msg.type == BALL_HIT)
    {
        BallHitMessage *hitMsg = &static_cast<BallHitMessage &>(msg);
        velocity.x = hitMsg->xForce;
        velocity.y = hitMsg->yForce;
        state = BallState::MOVING;
    }
    else if(msg.type == TURN_END) //Esto significa que el otro jugador ha acabado su turno
        playerTurn = true;
}