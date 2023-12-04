#include <unordered_map>
#include <iostream>

#include <raylib-cpp/raylib-cpp.hpp>
#include <util/Time.hpp>
#include <lua.hpp>
#include <entt.hpp>
#include <overlapping_wfc.hpp>
#include <rlgl.h>

#include "level/LdtkAssets.hpp"
#include "comp/PlayerComp.hpp"
#include "comp/SpriteComp.hpp"
#include "comp/MovementComp.hpp"
#include "comp/PositionComp.hpp"
#include "system/MovementSystem.hpp"
#include "system/PlayerInputSystem.hpp"
#include "system/SpriteDrawSystem.hpp"

int main()
{
	lua_State* luaState = luaL_newstate();
	luaL_openlibs(luaState);

	auto status = luaL_dofile(luaState, "assets/lua/config.lua");
	if (status)
	{
		std::cerr << "Couldn't load lua file lua/config.lua: " << lua_tostring(luaState, -1);
	}

	std::cout << "load file status: " << status << std::endl;

	int windowWidth = 1024;
	int windowHeight = 768;

	lua_getglobal(luaState, "config");
	lua_getfield(luaState, 1, "resolution");
	lua_getfield(luaState, 2, "x");
	lua_getfield(luaState, 2, "y");
	windowWidth = (int)lua_tonumber(luaState, 3);
	windowHeight = (int)lua_tonumber(luaState, 4);
	lua_settop(luaState, 0);

	// set up the window
	raylib::Window window(windowWidth, windowHeight, "window");

	SetTargetFPS(60);

	LdtkAssets ldtkAssets;
	ldtkAssets.load("assets/level/test.ldtk");

	float playAreaWidth = 10;
	float playAreaHeight = (float)windowHeight / windowWidth * 10;
	raylib::Rectangle playArea(-playAreaWidth / 2, -playAreaHeight / 2, playAreaWidth, playAreaHeight);
	raylib::Camera2D camera(raylib::Vector2((float)windowWidth / 2, (float)windowHeight / 2), raylib::Vector2(0, 0), 0, (float)windowWidth / playAreaWidth);

	int levelWidth;
	int levelHeight;

	auto levelIter = ldtkAssets.ldtk.levelBegin();
	ldtkimport::Level level;

	if (levelIter != ldtkAssets.ldtk.levelEnd())
	{
		ldtkimport::Level seedLevel = *levelIter;

		fastwfc::OverlappingWFCOptions options;
		options.periodic_input = false;
		options.periodic_output = false;
		options.out_height = 31;
		options.out_width = 41;
		options.symmetry = 8;
		options.ground = false;
		options.pattern_size = 3;

		int width = seedLevel.getWidth();
		int height = seedLevel.getHeight();

		fastwfc::Array2D<ldtkimport::intgridvalue_t> arr(height, width);

		for (int i = 0; i < width * height; i++)
		{
			arr.data[i] = seedLevel.getIntGrid()(i);
		}

		fastwfc::OverlappingWFC<ldtkimport::intgridvalue_t> wfc(arr, options, time(0));
		std::optional<fastwfc::Array2D<ldtkimport::intgridvalue_t>> result = wfc.run();

		if (result.has_value())
		{
			level.setIntGrid(options.out_width, options.out_height, std::move(result->data));
			ldtkAssets.ldtk.runRules(level);
		}
		else
		{
			std::cerr << "WFC failed!" << std::endl;
			level = seedLevel;
			ldtkAssets.ldtk.runRules(level);
		}
	}

	entt::registry registry;
	entt::entity player = registry.create();
	raylib::Texture charTex;
	charTex.Load("assets/image/char.png");

	registry.emplace<PlayerComp>(player);
	registry.emplace<PositionComp>(player);
	registry.emplace<MovementComp>(player, 5.f);
	registry.emplace<SpriteComp>(player, std::make_shared<raylib::Texture>(std::move(charTex)), raylib::Rectangle(0., 0., 128., 160.), raylib::Vector2(0.5, 1), 256.f);

	// game loop
	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();
		Time.deltaTime = deltaTime;

		// update
		playerInput(registry);
		movement(registry);

		// drawing
		BeginDrawing();
		ClearBackground(BLACK);
		camera.BeginMode();

		ldtkAssets.Draw(raylib::Vector2(-playAreaWidth / 2, -playAreaHeight / 2), 0.2f, level);
		drawSprites(registry);

		camera.EndMode();
		EndDrawing();
	}

	// cleanup
	CloseWindow();
	lua_close(luaState);
	return 0;
}