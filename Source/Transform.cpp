#include "Transform.h"

using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;

Transform::Transform(const Vector3& position,
                     const Vector3& rotation,
                     const Vector3& scale)
    : _position(position),
      _rotation(rotation),
      _scale(scale)
{
}

Transform::~Transform()
{
}

DirectX::SimpleMath::Matrix Transform::GetPositionMatrix() const
{
    return Matrix::CreateTranslation(_position);
}

DirectX::SimpleMath::Matrix Transform::GetRotationMatrix() const
{
    Matrix rotmat = Matrix::Identity;

    rotmat *= Matrix::CreateRotationX(_rotation.x);
    rotmat *= Matrix::CreateRotationY(_rotation.y);
    rotmat *= Matrix::CreateRotationZ(_rotation.z);

    return rotmat;
}

DirectX::SimpleMath::Matrix Transform::GetScaleMatrix() const
{
    return Matrix::CreateScale(_scale);
}

DirectX::SimpleMath::Matrix Transform::GetTransformMatrix() const
{
    Matrix transMat = Matrix::Identity;

    transMat *= GetScaleMatrix();
    transMat *= GetRotationMatrix();
    transMat *= GetPositionMatrix();

    return transMat;
}