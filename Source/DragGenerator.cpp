#include "DragGenerator.h"

#include <d3d11_1.h>
#include <SimpleMath.h>

DragGenerator::DragGenerator(float laminarCoefficient,
                             float turbulentCoefficient)
    : _laminarCoefficient(laminarCoefficient),
      _turbulentCoefficient(turbulentCoefficient)
{
}

DragGenerator::~DragGenerator() = default;

void DragGenerator::UpdateForce(RigidBody* rigidBody,
                                float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	DirectX::SimpleMath::Vector3 force = rigidBody->GetVelocity();
	float dragCoeff = force.Length();

	dragCoeff = (_laminarCoefficient * dragCoeff) + (_turbulentCoefficient * dragCoeff * dragCoeff);

	force.Normalize();
	force *= -dragCoeff;

	rigidBody->AddForce(force, DirectX::SimpleMath::Vector3::Zero);
}