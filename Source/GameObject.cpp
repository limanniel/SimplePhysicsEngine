#include "GameObject.h"

using DirectX::SimpleMath::Vector3;

GameObject::GameObject(const Vector3& position,
                       const Vector3& rotation,
                       const Vector3& scale,
                       const Geometry& geometry,
                       const Material& material)
{
    _transform = new Transform(position,
                               rotation,
                               scale);

    _appearance = new Appearance(geometry,
                                 material);
}

GameObject::GameObject(Transform* transform,
                       Appearance* appearance)
    : _transform(transform),
      _appearance(appearance)
{
}

GameObject::~GameObject()
{
    delete _transform;
    delete _appearance;
}

void GameObject::Render(ID3D11DeviceContext* context)
{
    context->PSSetShaderResources(0,
                                  1,
                                  _appearance->GetTextureRV().GetAddressOf());

    context->IASetVertexBuffers(0,
                                1,
                                _appearance->GetGeometry().vertexBuffer.GetAddressOf(),
                                _appearance->GetGeometry().GetVertexBufferStrideRef(),
                                _appearance->GetGeometry().GetVertexBufferOffsetRef());

    context->IASetIndexBuffer(_appearance->GetGeometry().indexBuffer.Get(),
                              DXGI_FORMAT_R16_UINT,
                              0);

    context->DrawIndexed(_appearance->GetGeometry().numberOfIndices, 0, 0);
}
