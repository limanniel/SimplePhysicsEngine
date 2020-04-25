#include "GravityGenerator.h"


GravityGenerator::GravityGenerator(const DirectX::SimpleMath::Vector3& gravity)
	: _gravity(gravity)
{
}

GravityGenerator::~GravityGenerator() = default;

void GravityGenerator::UpdateForce(RigidBody* rigidBody,
								   float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	// Abort, if rigidbody has infinite mass
	if (!rigidBody->HasFiniteMass()) return;

	rigidBody->AddForce(_gravity * rigidBody->GetMass(),
						DirectX::SimpleMath::Vector3::Zero);
}
