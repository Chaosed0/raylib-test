#pragma once

#include "raylib-cpp/raylib-cpp.hpp"
#include "Paddle.hpp"

enum class BallScoreResult
{
	None = 0,
	Left = 1,
	Right = 2,
};

class Ball
{
public:
	raylib::Vector2 position;
	raylib::Rectangle playArea;
	float radius;
	float moveSpeed;

	raylib::Vector2 moveDirection;

	Ball(float x, float y, float radius, float moveSpeed, raylib::Rectangle playArea);
	void SetMoveDirection(const raylib::Vector2& moveDirection);
	void CheckReflectAgainstPaddle(const Paddle& paddle);
	BallScoreResult CheckScore();
	void Update(float delta);
	void Draw(Color color);
private:
};
