#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>

class Transform
{
private:
	DirectX::SimpleMath::Vector3 _position;
	DirectX::SimpleMath::Vector3 _rotation;
	DirectX::SimpleMath::Vector3 _scale;

public:
	Transform(const DirectX::SimpleMath::Vector3& position,
			  const DirectX::SimpleMath::Vector3& rotation,
			  const DirectX::SimpleMath::Vector3& scale);
	~Transform();

	DirectX::SimpleMath::Matrix CalculateWorldMatrix() const;

	inline void SetPosition(const DirectX::SimpleMath::Vector3& position) { _position = position; }
	inline void SetRotation(const DirectX::SimpleMath::Vector3& rotation) { _rotation = rotation; }
	inline void SetScale(const DirectX::SimpleMath::Vector3& scale) { _scale = scale; }

	inline DirectX::SimpleMath::Vector3 GetPosition() const { return _position; }
	inline DirectX::SimpleMath::Vector3 GetRotation() const { return _rotation; }
	inline DirectX::SimpleMath::Vector3 GetScale() const { return _scale; }
};

