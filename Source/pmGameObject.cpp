#include "pmGameObject.h"

pmGameObject::pmGameObject(const DirectX::SimpleMath::Vector3& position,
                           const DirectX::SimpleMath::Vector3& rotation,
                           const DirectX::SimpleMath::Vector3& scale,
                           const Geometry& geometry,
                           const Material& material)
    : GameObject(position, rotation, scale, geometry, material)
{

}

pmGameObject::~pmGameObject()
{

}

void pmGameObject::Update(double deltaTime)
{
    _worldMatrix = _transform->CalculateWorldMatrix();
}
