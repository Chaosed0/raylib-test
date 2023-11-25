#include "Paddle.hpp"

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

Paddle::Paddle(float x, float y, float width, float height, float maxMovement, float maxHeight)
{
	rect = raylib::Rectangle(x, y, width, height);
	this->maxMovement = maxMovement;
	this->maxHeight = maxHeight;
	_origin = raylib::Vector2(width / 2, height / 2);
	movement = 0;
}

void Paddle::SetMovement(float move)
{
	movement = Clamp(move, -1, 1);
}

void Paddle::Update(float delta)
{
	raylib::Vector2 pos = rect.GetPosition();

	pos.y += movement * maxMovement * delta;
	pos.y = Clamp(pos.y, -maxHeight + rect.height / 2, maxHeight - rect.height / 2);
	rect.SetPosition(pos);
}

void Paddle::Draw(Color color)
{
	DrawRectanglePro(rect, _origin, 0, color);
}