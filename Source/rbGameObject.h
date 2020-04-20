#pragma once

#include "GameObject.h"
#include "RigidBody.h"

class rbGameObject : public GameObject
{
private:
	RigidBody* _rigidBody;

public:
	rbGameObject(const DirectX::SimpleMath::Vector3& position,
				 const DirectX::SimpleMath::Vector3& rotation,
				 const DirectX::SimpleMath::Vector3& scale,
				 const Geometry& geometry,
				 const Material& material,
				 const DirectX::SimpleMath::Vector3& initialVelocity = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
				 const DirectX::SimpleMath::Vector3& initialAcceleration = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
	~rbGameObject();

	void Update(float deltaTime) override;

	RigidBody* GetRigidBody() const { return _rigidBody; }
};

