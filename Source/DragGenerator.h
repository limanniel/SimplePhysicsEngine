#pragma once

#include "ForceGenerator.h"

// namespace containing pre-calculated drag coefficients
namespace DragCoefficients
{
	constexpr float Sphere = 0.47f;
	constexpr float Half_Sphere = 0.42f;
	constexpr float Cone = 0.50f;
	constexpr float Cube = 1.05f;
	constexpr float Angled_Cube = 0.80f;
	constexpr float Long_Cylinder = 0.82f;
	constexpr float Short_Cylinder = 1.15f;
	constexpr float Streamlined_Body = 0.04f;
	constexpr float Streamlined_Half_Body = 0.09f;
}

class DragGenerator : public ForceGenerator
{
private:
	float _laminarCoefficient;
	float _turbulentCoefficient;

public:
	DragGenerator(float laminarCoefficient,
				  float turbulentCoefficient);
	~DragGenerator();

	void UpdateForce(RigidBody* rigidBody,
					 float deltaTime) override;
};