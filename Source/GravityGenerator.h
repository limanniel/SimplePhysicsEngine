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

	void UpdateForce(RigidBody* rigidBody,
					 float deltaTime) override;
};