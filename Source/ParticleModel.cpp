#include "ParticleModel.h"
#include "Transform.h"

#include <cmath>

using DirectX::SimpleMath::Vector3;

ParticleModel::ParticleModel(Transform& transform,
                             const DirectX::SimpleMath::Vector3& initialVelocity,
                             const DirectX::SimpleMath::Vector3& initialAcceleration)
    : _transform(transform),
      _velocity(initialVelocity),
      _acceleration(initialAcceleration),
      _inverseMass(0.0f),
      _linearDamping(0.995f)
{
}

ParticleModel::~ParticleModel()
{
}

void ParticleModel::Integrate(float deltaTime)
{
    // Skip integration, particle has infinite mass
    if (_inverseMass <= 0.0f) return;

    // Calculate Acceleration
    _acceleration = _forceAccumulator * _inverseMass;

    // Calculate Velocity
    _velocity += _acceleration * deltaTime;

    // Apply linear damping to velocity
    _velocity *= powf(_linearDamping, deltaTime);

    // Update Position
    _transform.SetPosition(_transform.GetPosition() + (_velocity * deltaTime));

    // Reset Forces
    ResetForceAccumulator();
}

void ParticleModel::Update(float deltaTime)
{
    Integrate(deltaTime);
}

void ParticleModel::AddForce(const DirectX::SimpleMath::Vector3& force)
{
    _forceAccumulator += force;
}

void ParticleModel::ResetForceAccumulator()
{
    _forceAccumulator.Zero;
}

void ParticleModel::MoveConstVelocity(float deltaTime)
{
    _transform.SetPosition(_transform.GetPosition() + (_velocity * deltaTime));
}

void ParticleModel::MoveConstAcceleration(float deltaTime)
{
    _transform.SetPosition(_transform.GetPosition() + (_velocity * deltaTime + (_acceleration * 0.5f * deltaTime)));
    _velocity += _acceleration * deltaTime;
}