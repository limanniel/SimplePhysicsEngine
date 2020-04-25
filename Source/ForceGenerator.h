#pragma once

#include "RigidBody.h"

// Interface for Force Generators
class ForceGenerator
{
public:
	ForceGenerator() = default;
	virtual ~ForceGenerator() = default;

	virtual void UpdateForce(RigidBody* rigidBody,
							 float deltaTime) = 0;
};