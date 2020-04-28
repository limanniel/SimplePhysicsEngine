#include "ForceRegistry.h"

ForceRegistry::ForceRegistry() = default;

ForceRegistry::~ForceRegistry() = default;

void ForceRegistry::Update(float deltaTime)
{
	for (const auto& entry : _registry)
	{
		entry.forceGenerator->UpdateForce(entry.particle,
										  deltaTime);
	}
}

void ForceRegistry::Add(ParticleModel* particle, ForceGenerator* generator)
{
	_registry.emplace_back(particle, generator);
}

void ForceRegistry::Remove(ParticleModel* particle, ForceGenerator* generator)
{
	for (auto it = _registry.begin(); it != _registry.end(); ++it)
	{
		ForceRegister entry = *it;

		if (entry.particle == particle &&
			entry.forceGenerator == generator)
		{
			delete entry.forceGenerator;
			entry.forceGenerator = nullptr;

			_registry.erase(it);
			return;
		}
	}
}

void ForceRegistry::Unregister(ParticleModel* particle, const ForceGenerator& generator)
{
	for (auto it = _registry.begin(); it != _registry.end(); ++it)
	{
		ForceRegister entry = *it;

		if (entry.particle == particle &&
			entry.forceGenerator == &generator)
		{
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
			if (entry.forceGenerator != nullptr)
				delete entry.forceGenerator;

			entry.forceGenerator = nullptr;
		}
	}

	_registry.clear();
}