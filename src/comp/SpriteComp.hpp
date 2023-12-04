#pragma once

#include <raylib-cpp/raylib-cpp.hpp>

struct SpriteComp
{
	std::shared_ptr<raylib::Texture> tex;
	raylib::Rectangle spriteRect;
	raylib::Vector2 pivot;
	float pixelsPerUnit;
};