#pragma once

#include <raylib-cpp/raylib-cpp.hpp>
#include <entt.hpp>

#include "comp/PositionComp.hpp"
#include "comp/SpriteComp.hpp"
#include "util/Time.hpp"

void drawSprites(entt::registry& reg)
{
	auto view = reg.view<PositionComp, SpriteComp>();

	for (const entt::entity entity : view)
	{
		const PositionComp& posComp = view.get<PositionComp>(entity);
		const SpriteComp& spriteComp = view.get<SpriteComp>(entity);
		const raylib::Rectangle destRect(
				posComp.pos.x,
				posComp.pos.y,
				spriteComp.spriteRect.width / spriteComp.pixelsPerUnit,
				spriteComp.spriteRect.height / spriteComp.pixelsPerUnit);

		spriteComp.tex->Draw(spriteComp.spriteRect, destRect, spriteComp.pivot);
	}
}
