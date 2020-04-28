#pragma once

#include "ParticleModel.h"

// Interface for Force Generators
class ForceGenerator
{
public:
	ForceGenerator() = default;
	virtual ~ForceGenerator() = default;

	virtual void UpdateForce(ParticleModel* particle,
							 float deltaTime) = 0;
};