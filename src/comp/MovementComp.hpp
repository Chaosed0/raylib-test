#pragma once

#include <raylib-cpp/Vector2.hpp>

struct MovementComp
{
	float maxSpeed;
	raylib::Vector2 direction;
};
