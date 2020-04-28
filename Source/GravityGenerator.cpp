#include "GravityGenerator.h"

using DirectX::SimpleMath::Vector3;

GravityGenerator::GravityGenerator(const DirectX::SimpleMath::Vector3& gravity)
	: _gravity(gravity)
{
}

GravityGenerator::~GravityGenerator() = default;

void GravityGenerator::UpdateForce(ParticleModel* particle,
								   float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	// Abort, if rigidbody has infinite mass
	if (!particle->HasFiniteMass()) return;

	Vector3 gravityForce = _gravity / particle->GetInverseMass();
	particle->AddForce(gravityForce);
}
