#pragma once
#include "ForceGenerator.h"


class BuoyancyGenerator : public ForceGenerator
{
private:
	float _maxDepth;
	float _volume;
	float _waterHeight;
	float _liquidDensity;

public:
	BuoyancyGenerator(float maxDepth,
					  float volume,
					  float waterHeight,
					  float liquidDensity = 1000.0f);

	~BuoyancyGenerator();

	void UpdateForce(RigidBody* rigidBody,
					 float deltaTime) override;

};

