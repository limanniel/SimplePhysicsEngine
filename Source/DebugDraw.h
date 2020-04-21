#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include <CommonStates.h>
#include <GeometricPrimitive.h>

struct ConstantBuffer;

class DebugDraw
{
private:
	bool										 _active;
	std::unique_ptr<DirectX::CommonStates>		 _commonStates;
	std::unique_ptr<DirectX::GeometricPrimitive> _boundingSphere;

public:
	DebugDraw(ID3D11Device* device,
			  ID3D11DeviceContext* context);
	~DebugDraw();

	void DrawBoundingSphere(ID3D11DeviceContext* context,
							ConstantBuffer cb,
							const DirectX::SimpleMath::Vector3& position,
							const DirectX::SimpleMath::Vector3& radius,
							const DirectX::XMVECTORF32& colour = DirectX::Colors::Green);

	inline void SetActive(bool state) { _active = state; }
	inline bool IsActive() const { return _active; }
};