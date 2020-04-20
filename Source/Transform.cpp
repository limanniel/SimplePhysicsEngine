#include "Transform.h"

using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;

Transform::Transform(const Vector3& position,
                     const Vector3& rotation,
                     const Vector3& scale)
    : _position(position),
      _rotation(rotation),
      _scale(scale),
      _worldMatrix(_worldMatrix.Identity)
{
}

Transform::~Transform()
{
}

void Transform::Update(float deltaTime)
{
	Matrix scale = Matrix::CreateScale(_scale);
	Matrix rotation = Matrix::CreateRotationX(_rotation.x) * Matrix::CreateRotationY(_rotation.y) * Matrix::CreateRotationZ(_rotation.z);
	Matrix translation = Matrix::CreateTranslation(_position);

    _worldMatrix = scale * rotation * translation;
}