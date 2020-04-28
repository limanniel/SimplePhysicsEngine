#pragma once

#include <vector>

#include "DragGenerator.h"
#include "GravityGenerator.h"
#include "BuoyancyGenerator.h"

class ForceRegistry
{
	struct ForceRegister
	{
		ParticleModel* particle;
		ForceGenerator* forceGenerator;

		ForceRegister(ParticleModel* p,
					  ForceGenerator* fg)
			: particle(p),
			  forceGenerator(fg)
		{ }
	};

private:
	std::vector<ForceRegister> _registry;

public:
	ForceRegistry();
	~ForceRegistry();

	// Update registered rigid bodies
	void Update(float deltaTime);

	// Add particle and force generator to the registry
	void Add(ParticleModel* particle, ForceGenerator* generator);

	// Remove force generator from registry
	void Remove(ParticleModel* particle, ForceGenerator* generator);

	// Unregister particle from selected force generator (doesn't remove pointers)
	void Unregister(ParticleModel* particle, const ForceGenerator& generator);

	// Clear registry
	void Clear();
};