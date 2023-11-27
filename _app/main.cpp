/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and run premake 

*/

#include <unordered_map>

#include "raylib-cpp/raylib-cpp.hpp"
#include "Paddle.hpp"
#include "Ball.hpp"
//#include "sol/sol.hpp"
#include "lua.hpp"
#include "ldtkimport/LdtkDefFile.h"
#include "ldtkimport/Level.h"
#include "wfc.hpp"
#include <rlgl.h>

struct TileSetImage
{
	std::shared_ptr<raylib::Texture> tex;
	std::unordered_map<ldtkimport::tileid_t, raylib::Rectangle> tiles;

	TileSetImage()
	{
		tex = std::make_shared<raylib::Texture>();
	}
};

struct LdtkAssets
{
	ldtkimport::LdtkDefFile ldtk;
	std::unordered_map<ldtkimport::uid_t, ::TileSetImage> tilesetImages;

	bool load(std::string filename)
	{
		ldtk.loadFromFile(filename.c_str(), false);

		for (auto tileset = ldtk.tilesetBegin(), end = ldtk.tilesetEnd(); tileset != end; ++tileset)
		{
			if (tileset->imagePath.empty())
			{
				continue;
			}

			std::string imagePath = "assets/" + tileset->imagePath;
			::TileSetImage tileSetImage;
			tileSetImage.tex->Load(imagePath);

			ldtkimport::uid_t uid = tileset->uid;
			auto pair = std::make_pair(uid, tileSetImage);
			tilesetImages.insert(pair);
		}

		for (auto layer = ldtk.layerBegin(), end = ldtk.layerEnd(); layer != end; ++layer)
		{
			const ldtkimport::TileSet* tileset = ldtk.getTileset(layer->tilesetDefUid);

			if (tileset == nullptr)
			{
				std::cerr << "TileSet " << layer->tilesetDefUid << " was not found in ldtk file" << std::endl;
				continue;
			}

			if (tilesetImages.count(tileset->uid) == 0)
			{
				std::cerr << "TileSet " << tileset->uid << " was not found in tilesetImages" << std::endl;
				continue;
			}

			auto& tilesetImage = tilesetImages[tileset->uid];
			const float cellPixelSize = layer->cellPixelSize;

			for (auto ruleGroup : layer->ruleGroups)
			{
				for (auto rule : ruleGroup.rules)
				{
					for (auto tile : rule.tileIds)
					{
						if (tilesetImage.tiles.count(tile) > 0)
						{
							continue;
						}

						int16_t tileX, tileY;
						tileset->getCoordinates(tile, tileX, tileY);
						raylib::Rectangle rect(tileX * cellPixelSize, tileY * cellPixelSize, cellPixelSize, cellPixelSize);
						tilesetImage.tiles.insert(std::make_pair(tile, rect));
					}
				}
			}
		}

		return true;
	}

	void Draw(raylib::Vector2 pos, float cellSize, const ldtkimport::Level& level)
	{
		auto cellCountX = level.getWidth();
		auto cellCountY = level.getHeight();

		for (size_t layerNum = ldtk.getLayerCount(); layerNum > 0; --layerNum)
		{
			const auto& layer = ldtk.getLayerByIdx((int)layerNum - 1);
			const auto& tileGrid = level.getTileGridByIdx((int)layerNum - 1);

			ldtkimport::TileSet* tileset = ldtk.getTileset(layer.tilesetDefUid);
			if (tileset == nullptr)
			{
				continue;
			}

			if (tilesetImages.count(tileset->uid) == 0)
			{
				continue;
			}

			auto& tilesetImage = tilesetImages[tileset->uid];

			for (int cellY = 0; cellY < cellCountY; ++cellY)
			{
				for (int cellX = 0; cellX < cellCountX; ++cellX)
				{
					// these are the tiles in this cell
					auto& tiles = tileGrid(cellX, cellY);

					for (auto tile = tiles.crbegin(), tileEnd = tiles.crend(); tile != tileEnd; ++tile)
					{
						DrawSingleTile(tilesetImage, *tile, cellSize, pos, cellX, cellY);
					}

				}
			}
		}
	}

	void DrawTiles(TileSetImage& tilesetImage, const ldtkimport::tiles_t* tilesToDraw, uint8_t idxToStartDrawing, float cellSize, raylib::Vector2 pos, int cellX, int cellY)
	{
		for (int tileIdx = idxToStartDrawing; tileIdx >= 0; --tileIdx)
		{
			const auto& tile = (*tilesToDraw)[tileIdx];
			DrawSingleTile(tilesetImage, tile, cellSize, pos, cellX, cellY);
		}
	}

	void DrawSingleTile(const TileSetImage& tilesetImage, const ldtkimport::TileInCell& tile, float cellSize, raylib::Vector2 pos, int cellX, int cellY)
	{
		float offsetX = tile.getOffsetX(cellSize * 0.5f);
		float offsetY = tile.getOffsetY(cellSize * 0.5f);
		float scaleX;
		float scaleY;
		float pivotX;
		float pivotY;

		if (tile.isFlippedX())
		{
			scaleX = -1;
			pivotX = cellSize;
		}
		else
		{
			scaleX = 1;
			pivotX = 0;
		}

		if (tile.isFlippedY())
		{
			scaleY = -1;
			pivotY = cellSize;
		}
		else
		{
			scaleY = 1;
			pivotY = 0;
		}

		raylib::Rectangle spriteRec = tilesetImage.tiles.at(tile.tileId);
		raylib::Rectangle destRec = raylib::Rectangle(pos.x + cellX * cellSize + offsetX, pos.y + cellY * cellSize + offsetY, cellSize, cellSize);
		tilesetImage.tex->Draw(spriteRec, destRec, raylib::Vector2(0,0), 0, WHITE);
	}
};

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
	ldtkAssets.load("assets/test.ldtk");

	float playAreaWidth = 10;
	float playAreaHeight = (float)windowHeight / windowWidth * 10;
	raylib::Rectangle playArea(-playAreaWidth / 2, -playAreaHeight / 2, playAreaWidth, playAreaHeight);
	raylib::Camera2D camera(raylib::Vector2((float)windowWidth / 2, (float)windowHeight / 2), raylib::Vector2(0, 0), 0, (float)windowWidth / playAreaWidth);

	Paddle playerPaddle(-4, 0, 0.25, 1, 5, playAreaHeight / 2);
	Paddle enemyPaddle(4, 0, 0.25, 1, 5, playAreaHeight / 2);
	Ball ball(0, 0, 0.1f, 6, playArea);

	int playerScore = 0;
	int enemyScore = 0;

	ball.SetMoveDirection(raylib::Vector2(1, 0).Rotate(PI / 4 + (GetRandomValue(0, 3) * (PI / 2))));

	int levelWidth;
	int levelHeight;

	auto levelIter = ldtkAssets.ldtk.levelBegin();
	ldtkimport::Level level;

	if (levelIter != ldtkAssets.ldtk.levelEnd())
	{
		level = *levelIter;
		ldtkAssets.ldtk.runRules(level);
	}

	// game loop
	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();

		bool upPressed = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W);
		bool downPressed = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
		float movement = (upPressed ? 1.f : 0) - (downPressed ? 1.f : 0);

		playerPaddle.SetMovement(-movement);
		enemyPaddle.SetMovement((ball.position.y - enemyPaddle.rect.y) * 5);

		playerPaddle.Update(deltaTime);
		enemyPaddle.Update(deltaTime);
		ball.Update(deltaTime);

		ball.CheckReflectAgainstPaddle(playerPaddle);
		ball.CheckReflectAgainstPaddle(enemyPaddle);
		BallScoreResult result = ball.CheckScore();

		if (result == BallScoreResult::Left)
		{
			enemyScore++;
			ball.position = raylib::Vector2(0, 0);
			ball.SetMoveDirection(raylib::Vector2(1, 0).Rotate(PI / 4 + (GetRandomValue(0, 3) * (PI / 2))));
		}
		else if (result == BallScoreResult::Right)
		{
			playerScore++;
			ball.position = raylib::Vector2(0, 0);
			ball.SetMoveDirection(raylib::Vector2(1, 0).Rotate(PI / 4 + (GetRandomValue(0, 3) * (PI / 2))));
		}

		// drawing
		BeginDrawing();
		ClearBackground(BLACK);
		camera.BeginMode();

		ldtkAssets.Draw(raylib::Vector2(-playAreaWidth / 2, -playAreaHeight / 2), 0.1f, level);

		playerPaddle.Draw(GREEN);
		enemyPaddle.Draw(RED);
		ball.Draw(YELLOW);

		camera.EndMode();

		DrawText(std::to_string(playerScore).c_str(), (int)(0.1f * windowWidth), 50, 32, WHITE);
		DrawText(std::to_string(enemyScore).c_str(), (int)(0.9f * windowWidth), 50, 32, WHITE);

		EndDrawing();
	}

	// cleanup
	CloseWindow();
	lua_close(luaState);
	return 0;
}