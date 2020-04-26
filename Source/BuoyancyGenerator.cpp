#include "BuoyancyGenerator.h"

#include <d3d11_1.h>
#include <SimpleMath.h>
#include "Transform.h"

using DirectX::SimpleMath::Vector3;

BuoyancyGenerator::BuoyancyGenerator(float maxDepth,
									 float volume,
									 float waterHeight,
									 float liquidDensity)
	: _maxDepth(maxDepth),
	  _volume(volume),
	  _waterHeight(waterHeight),
	  _liquidDensity(liquidDensity)
{

}

BuoyancyGenerator::~BuoyancyGenerator() = default;

void BuoyancyGenerator::UpdateForce(RigidBody* rigidBody, float deltaTime)
{
	float currentDepth = rigidBody->GetTransformRef().GetPosition().y;

	// Early-out, if object is higher than water level
	if (currentDepth >= _waterHeight + _maxDepth) return;

	Vector3 force = Vector3::Zero;

	// Fully Submerged
	if (currentDepth <= _waterHeight - _maxDepth)
	{
		force.y = _liquidDensity * _volume;
		rigidBody->AddForce(force, Vector3());
		return;
	}

	// Partially sumberged update
	force.y = _liquidDensity * _volume * (currentDepth - _maxDepth - _waterHeight) / 2 * _maxDepth;
	rigidBody->AddForce(force, Vector3());
}