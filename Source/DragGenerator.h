#pragma once

#include "ForceGenerator.h"

enum class DRAG_COEFFICIENTS
{
	NONE = 0,
	SPHERE,
	HALF_SPHERE,
	CONE,
	CUBE,
	ANGLED_CUBE,
	LONG_CYLINDER,
	SHORT_CYLINDER,
	STREAMLINED_BODY,
	STREAMLINED_HALF_BODY
};

class DragGenerator : public ForceGenerator
{
private:
	float _laminarCoefficient;
	float _turbulentCoefficient;

public:
	DragGenerator(float laminarCoefficient,
				  float turbulentCoefficient);
	~DragGenerator();

	void UpdateForce(ParticleModel* particle,
					 float deltaTime) override;

	static float s_coefficients[10];

	inline void ChangeLaminarCoeff(float newLaminarCoeff) { _laminarCoefficient = newLaminarCoeff; }
	inline void ChangeTurbulentCoeff(float newTurbulentCoeff) { _turbulentCoefficient = newTurbulentCoeff; }
};