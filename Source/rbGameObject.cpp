#include "rbGameObject.h"

rbGameObject::rbGameObject(const DirectX::SimpleMath::Vector3& position,
                           const DirectX::SimpleMath::Vector3& rotation,
                           const DirectX::SimpleMath::Vector3& scale,
                           const Geometry& geometry, const Material& material,
                           const DirectX::SimpleMath::Vector3& initialVelocity,
                           const DirectX::SimpleMath::Vector3& initialAcceleration)
    : GameObject(position, rotation, scale, geometry, material)
{
    _rigidBody = new RigidBody(*_transform,
                               initialVelocity,
                               initialAcceleration);

    _boundingSphereRadius = 0.0f;
}

rbGameObject::~rbGameObject()
{
    delete _rigidBody;
    _rigidBody = nullptr;
}

void rbGameObject::Update(float deltaTime)
{
    _rigidBody->Update(deltaTime);

    _worldMatrix = _transform->GetScaleMatrix() * _rigidBody->GetTransformMatrix() * _transform->GetPositionMatrix();
}
