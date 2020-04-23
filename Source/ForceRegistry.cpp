#include "ForceRegistry.h"

ForceRegistry::ForceRegistry() = default;

ForceRegistry::~ForceRegistry()
{
	Clear();
}

void ForceRegistry::Update(float deltaTime)
{
	for (const auto& entry : _registry)
	{
		entry.forceGenerator->UpdateForce(entry.rigidBody,
										  deltaTime);
	}
}

void ForceRegistry::Add(RigidBody* rigidBody, ForceGenerator* generator)
{
	_registry.emplace_back(rigidBody, generator);
}

void ForceRegistry::Remove(RigidBody* rigidBody, ForceGenerator* generator)
{
	for (auto it = _registry.begin(); it != _registry.end(); ++it)
	{
		ForceRegister entry = *it;

		if (entry.rigidBody == rigidBody &&
			entry.forceGenerator == generator)
		{
			delete entry.forceGenerator;
			entry.forceGenerator = nullptr;

			_registry.erase(it);
			return;
		}
	}
}

void ForceRegistry::Clear()
{
	if (!_registry.empty())
	{
		for (auto entry : _registry)
		{
			delete entry.forceGenerator;
			entry.forceGenerator = nullptr;
		}
	}

	_registry.clear();
}