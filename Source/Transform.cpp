#include "Transform.h"

using DirectX::SimpleMath::Matrix;

Transform::Transform(const DirectX::SimpleMath::Vector3& position,
                     const DirectX::SimpleMath::Vector3& rotation,
                     const DirectX::SimpleMath::Vector3& scale)
    : _position(position),
      _rotation(rotation),
      _scale(scale)
{
}

Transform::~Transform()
{
}

DirectX::SimpleMath::Matrix Transform::CalculateWorldMatrix() const
{
    Matrix scale = Matrix::CreateScale(_scale);
    Matrix rotation = Matrix::CreateRotationX(_rotation.x) * Matrix::CreateRotationY(_rotation.y) * Matrix::CreateRotationZ(_rotation.z);
    Matrix translation = Matrix::CreateTranslation(_position);

    return Matrix(scale * rotation * translation);
}
