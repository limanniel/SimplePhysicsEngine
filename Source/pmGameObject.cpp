#include "pmGameObject.h"

pmGameObject::pmGameObject(const DirectX::SimpleMath::Vector3& position,
						   const DirectX::SimpleMath::Vector3& rotation,
						   const DirectX::SimpleMath::Vector3& scale,
						   const Geometry& geometry,
						   const Material& material,
						   const DirectX::SimpleMath::Vector3& initialVelocity,
						   const DirectX::SimpleMath::Vector3& initialAcceleration)
    : GameObject(position, rotation, scale, geometry, material)
{
	_particle = new ParticleModel(*_transform,
								  initialVelocity,
								  initialAcceleration);
}

pmGameObject::~pmGameObject()
{
	delete _particle;
	_particle = nullptr;
}

void pmGameObject::Update(float deltaTime)
{
	_particle->Update(deltaTime);

    _worldMatrix = _transform->CalculateWorldMatrix();
}
