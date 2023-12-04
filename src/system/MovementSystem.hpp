#pragma once

#include "comp/PositionComp.hpp"
#include "comp/MovementComp.hpp"
#include "util/Time.hpp"
#include <entt.hpp>

void movement(entt::registry& reg)
{
	auto view = reg.view<PositionComp, MovementComp>();

	for (const entt::entity entity : view)
	{
		PositionComp& posComp = view.get<PositionComp>(entity);
		const MovementComp move = view.get<MovementComp>(entity);
		posComp.pos += move.direction.Normalize() * move.maxSpeed * Time.deltaTime;
	}
}