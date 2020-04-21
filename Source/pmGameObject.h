#pragma once

#include "GameObject.h"
#include "ParticleModel.h"

class pmGameObject : public GameObject
{
protected:
	ParticleModel* _particle;

public:
	pmGameObject(const DirectX::SimpleMath::Vector3& position,
				 const DirectX::SimpleMath::Vector3& rotation,
				 const DirectX::SimpleMath::Vector3& scale,
				 const Geometry& geometry,
				 const Material& material,
				 const DirectX::SimpleMath::Vector3& initialVelocity = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
				 const DirectX::SimpleMath::Vector3& initialAcceleration = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));

	pmGameObject(Transform* transform,
				 Appearance* appearance,
				 const DirectX::SimpleMath::Vector3& initialVelocity = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
				 const DirectX::SimpleMath::Vector3& initialAcceleration = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));

	~pmGameObject();

	void Update(float deltaTime) override;

	ParticleModel* GetParticleModel() const { return _particle; }
};

