#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <wrl/client.h>

struct Geometry
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	int numberOfIndices{ 0 };

	UINT vertexBufferStride{ 0 };
	UINT vertexBufferOffset{ 0 };

	inline UINT* GetVertexBufferStrideRef() { return &vertexBufferStride; }
	inline UINT* GetVertexBufferOffsetRef() { return &vertexBufferOffset; }
};

struct Material
{
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 specular;
	float specularPower;
};

class Appearance
{
private:
	Geometry _geometry;
	Material _material;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _textureRV;

public:
	Appearance(const Geometry& geometry,
			   const Material& material);

	Appearance(const Geometry& geometry,
			   const Material& material,
			   ID3D11ShaderResourceView* texture);

	~Appearance();

	inline void SetTextureRV(ID3D11ShaderResourceView* textureRV) { _textureRV = textureRV; }
	inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTextureRV() const { return _textureRV; }
	inline bool HasTexture() const { return _textureRV ? true : false; }

	inline Geometry GetGeometry() const { return _geometry; }
	inline Material GetMaterial() const { return _material; }
};

