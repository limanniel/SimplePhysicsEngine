#include "DebugDraw.h"
#include "Commons.h"

using namespace DirectX;

DebugDraw::DebugDraw(ID3D11Device* device,
                     ID3D11DeviceContext* context)
{
    _active = true;

    _commonStates = std::make_unique<CommonStates>(device);
    _boundingSphere = GeometricPrimitive::CreateSphere(context, 1.0f, 4, false);
}

DebugDraw::~DebugDraw()
{

}

void DebugDraw::DrawBoundingSphere(ID3D11DeviceContext* context,
                                   ConstantBuffer cb,
                                   const DirectX::SimpleMath::Vector3& position,
                                   const DirectX::SimpleMath::Vector3& radius,
                                   const DirectX::XMVECTORF32& colour)
{
    if (_active)
    {
        XMMATRIX world = XMMatrixIdentity();
        world *= XMMatrixScaling(1.0f + radius.x, 1.0f + radius.y, 1.0f + radius.z);
        world *= XMMatrixTranslation(position.x, position.y, position.z);

	    XMMATRIX view = XMMatrixTranspose(cb.View);
	    XMMATRIX proj = XMMatrixTranspose(cb.Projection);

	    _boundingSphere->Draw(world, view, proj, colour, nullptr, true);
	    context->RSSetState(_commonStates->CullCounterClockwise());
    }
}