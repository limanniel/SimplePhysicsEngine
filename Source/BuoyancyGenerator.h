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

	void UpdateForce(ParticleModel* particle,
					 float deltaTime) override;

	inline void ChangeMaxDepth(float newMaxDepth) { _maxDepth = newMaxDepth; }
	inline float GetMaxDepth() const { return _maxDepth; }

	inline void ChangeVolume(float newVolume) { _volume = newVolume; }
	inline float GetVolume() const { return _volume; }

	inline void ChangeWaterHeight(float newWaterHeight) { _waterHeight = newWaterHeight; }
	inline float GetWaterHeight() const { return _waterHeight; }

	inline void ChangeLiquidDensity(float newLiquidDensity) { _liquidDensity = newLiquidDensity; }
	inline float GetLiquidDensity() const { return _liquidDensity; }
};