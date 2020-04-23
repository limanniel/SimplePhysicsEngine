#pragma once

#include <vector>

#include "DragGenerator.h"
#include "GravityGenerator.h"

class ForceRegistry
{
	struct ForceRegister
	{
		RigidBody* rigidBody;
		ForceGenerator* forceGenerator;

		ForceRegister(RigidBody* rb,
					  ForceGenerator* fg)
			: rigidBody(rb),
			  forceGenerator(fg)
		{ }
	};

private:
	std::vector<ForceRegister> _registry;

	// Clear registry
	void Clear();

public:
	ForceRegistry();
	~ForceRegistry();

	// Update registered rigid bodies
	void Update(float deltaTime);

	// Add rigidbody and force generator to the registry
	void Add(RigidBody* rigidBody, ForceGenerator* generator);

	// Remove rigid body from registry
	void Remove(RigidBody* rigidBody, ForceGenerator* generator);
};