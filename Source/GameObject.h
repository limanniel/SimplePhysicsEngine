#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include "Transform.h"
#include "Appearance.h"

class GameObject
{
protected:
	Transform*  _transform;
	Appearance* _appearance;
	DirectX::SimpleMath::Matrix _worldMatrix;

protected:
	GameObject(const DirectX::SimpleMath::Vector3& position,
			   const DirectX::SimpleMath::Vector3& rotation,
			   const DirectX::SimpleMath::Vector3& scale,
			   const Geometry& geometry,
			   const Material& material);

	GameObject(Transform* transform,
			   Appearance* appearance);

public:
	virtual ~GameObject();

	void Render(ID3D11DeviceContext* context);
	virtual void Update(float deltaTime) = 0;

	inline Transform* GetTransform() const { return _transform; }
	inline Appearance* GetAppearance() const { return _appearance; }
	inline DirectX::SimpleMath::Matrix GetWorldMatrix() const { return _worldMatrix; }
};

