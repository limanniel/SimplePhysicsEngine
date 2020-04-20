#include "pmGameObject.h"

using DirectX::SimpleMath::Vector3;

pmGameObject::pmGameObject(const Vector3& position,
						   const Vector3& rotation,
						   const Vector3& scale,
						   const Geometry& geometry,
						   const Material& material,
						   const Vector3& initialVelocity,
						   const Vector3& initialAcceleration)
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
	_transform->Update(deltaTime);
}
