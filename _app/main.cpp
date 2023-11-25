/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and run premake 

*/

#include "raylib-cpp/raylib-cpp.hpp"
#include "Paddle.hpp"
#include "Ball.hpp"
#include "sol/sol.hpp"
#include <rlgl.h>

int main()
{
	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.script_file("lua/config.lua");
	// the type "sol::state" behaves 
	// exactly like a table!
	bool isfullscreen = lua["config"]["fullscreen"]; // can get nested variables
	sol::table config = lua["config"];

	// set up the window
	int windowWidth = config["resolution"]["x"].get<int>();
	int windowHeight = config["resolution"]["y"].get<int>();
	raylib::Window window(windowWidth, windowHeight, "window");

	SetTargetFPS(60);

	float playAreaWidth = 10;
	float playAreaHeight = (float)windowHeight / windowWidth * 10;
	raylib::Rectangle playArea(-playAreaWidth / 2, -playAreaHeight / 2, playAreaWidth, playAreaHeight);
	raylib::Camera2D camera(raylib::Vector2(windowWidth / 2, windowHeight / 2), raylib::Vector2(0, 0), 0, windowWidth / playAreaWidth);

	Paddle playerPaddle(-4, 0, 0.25, 1, 5, playAreaHeight / 2);
	Paddle enemyPaddle(4, 0, 0.25, 1, 5, playAreaHeight / 2);
	Ball ball(0, 0, 0.1, 6, playArea);

	int playerScore = 0;
	int enemyScore = 0;

	ball.SetMoveDirection(raylib::Vector2(1, 0).Rotate(PI / 4 + (GetRandomValue(0, 3) * (PI / 2))));

	// game loop
	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();

		bool upPressed = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W);
		bool downPressed = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
		float movement = (upPressed ? 1 : 0) - (downPressed ? 1 : 0);

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

		playerPaddle.Draw(GREEN);
		enemyPaddle.Draw(RED);
		ball.Draw(YELLOW);
		
		camera.EndMode();

		DrawText(std::to_string(playerScore).c_str(), 0.1 * windowWidth, 50, 32, WHITE);
		DrawText(std::to_string(enemyScore).c_str(), 0.9 * windowWidth, 50, 32, WHITE);

		EndDrawing();
	}

	// cleanup
	CloseWindow();
	return 0;
}