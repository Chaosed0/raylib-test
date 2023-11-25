#pragma once

#include "raylib-cpp/raylib-cpp.hpp"
#include <iostream>

class Paddle
{
public:
	raylib::Rectangle rect;
	float movement;
	float maxMovement;
	float maxHeight;

	Paddle(float x, float y, float width, float height, float maxMovement, float maxHeight);
	void SetMovement(float move);
	void Update(float delta);
	void Draw(Color color);
private:
	raylib::Vector2 _origin;
};
