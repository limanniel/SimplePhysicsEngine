#include "DragGenerator.h"

#include <d3d11_1.h>
#include <SimpleMath.h>

using DirectX::SimpleMath::Vector3;

float DragGenerator::s_coefficients[10] =
{
	0.0f,
	0.47f,
	0.42f,
	0.50f,
	1.05f,
	0.80f,
	0.082f,
	1.15f,
	0.04f,
	0.09f
};

DragGenerator::DragGenerator(float laminarCoefficient,
                             float turbulentCoefficient)
    : _laminarCoefficient(laminarCoefficient),
      _turbulentCoefficient(turbulentCoefficient)
{
}

DragGenerator::~DragGenerator() = default;

void DragGenerator::UpdateForce(ParticleModel* particle,
                                float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	Vector3 force = particle->GetVelocity();
	float dragCoeff = force.Length();

	dragCoeff = (_laminarCoefficient * dragCoeff) + (_turbulentCoefficient * dragCoeff * dragCoeff);

	force.Normalize();
	force *= -dragCoeff;

	particle->AddForce(force);
}