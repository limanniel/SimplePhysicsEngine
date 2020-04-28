#pragma once

#include "ForceGenerator.h"
#include <d3d11_1.h>
#include <SimpleMath.h>

class GravityGenerator : public ForceGenerator
{
private:
	DirectX::SimpleMath::Vector3 _gravity;

public:
	GravityGenerator(const DirectX::SimpleMath::Vector3& gravity);
	~GravityGenerator();

	void UpdateForce(ParticleModel* particle,
					 float deltaTime) override;

	inline void ChangeGravity(const DirectX::SimpleMath::Vector3& newGravity) { _gravity = newGravity; }
	inline DirectX::SimpleMath::Vector3 GetCurrentGravity() const { return _gravity; }
};