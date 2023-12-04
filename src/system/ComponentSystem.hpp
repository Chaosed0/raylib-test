#include <entt.hpp>

class ComponentSystem
{
public:
	ComponentSystem(entt::registry& registry) : _reg(registry) { }

	virtual void OnCreate() { }
	virtual void OnStart() { }
	virtual void OnReady() { }
	virtual void Update() { }
private:
	entt::registry& _reg;
};