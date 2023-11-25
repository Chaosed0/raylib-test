#include "Ball.hpp"

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

Ball::Ball(float x, float y, float radius, float moveSpeed, raylib::Rectangle playArea)
{
	position = raylib::Vector2(x, y);
	this->radius = radius;
	this->playArea = playArea;
	this->moveSpeed = moveSpeed;
}

void Ball::SetMoveDirection(const raylib::Vector2& moveDirection)
{
	this->moveDirection = moveDirection;// .Normalize();
}

void Ball::Update(float delta)
{
	position = position + moveDirection * moveSpeed * delta;

	if (moveDirection.y > 0 &&
		position.y + radius > playArea.y + playArea.height)
	{
		moveDirection = moveDirection.Reflect(raylib::Vector2(0, -1));
	}

	if (moveDirection.y < 0 &&
		position.y - radius < playArea.y)
	{
		moveDirection = moveDirection.Reflect(raylib::Vector2(0, 1));
	}
}

void Ball::CheckReflectAgainstPaddle(const Paddle& paddle)
{
	raylib::Rectangle rect = paddle.rect;
	raylib::Vector2 rectCenter = rect.GetPosition();
	rect.SetPosition(rect.GetPosition() - (rect.GetSize() / 2));

	raylib::Vector2 towards = rectCenter;
	if (towards.DotProduct(moveDirection) < 0)
	{
		return;
	}

    int recCenterX = (int)(rect.x + rect.width/2.0f);
    int recCenterY = (int)(rect.y + rect.height/2.0f);

    float dx = fabsf(position.x - (float)recCenterX);
    float dy = fabsf(position.y - (float)recCenterY);

	bool colliding = CheckCollisionCircleRec(position, radius, rect);

	if (colliding)
	{
		moveDirection = moveDirection.Reflect(raylib::Vector2(-sgn(towards.x), 0));
	}
}

BallScoreResult Ball::CheckScore()
{
	if (position.x + radius < playArea.x)
	{
		return BallScoreResult::Left;
	}

	if (position.x + radius > playArea.x + playArea.width)
	{
		return BallScoreResult::Right;
	}

	return BallScoreResult::None;
}

void Ball::Draw(Color color)
{
	DrawCircleV(position, radius, color);
}
