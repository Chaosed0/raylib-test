#pragma once

#include <raylib-cpp/raylib-cpp.hpp>
#include <entt.hpp>

#include "comp/PositionComp.hpp"
#include "comp/MovementComp.hpp"
#include "comp/PlayerComp.hpp"
#include "util/Time.hpp"

void playerInput(entt::registry& reg)
{
	auto view = reg.view<PlayerComp, MovementComp>();

	for (const entt::entity entity : view)
	{
		raylib::Vector2 vec;

		if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
		{
			vec.y -= 1;
		}

		if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
		{
			vec.y += 1;
		}

		if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
		{
			vec.x -= 1;
		}

		if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
		{
			vec.x += 1;
		}

		MovementComp& moveComp = view.get<MovementComp>(entity);
		moveComp.direction = vec;
	}
}
