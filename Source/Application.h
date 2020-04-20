#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include <vector>
#include <StepTimer.h>
#include <SimpleMath.h>
#include <DDSTextureLoader.h>
#include <wrl/client.h>

#include "resource.h"
#include "Camera.h"
#include "pmGameObject.h"
#include "rbGameObject.h"

#pragma region Structs
struct SimpleVertex
{
    XMFLOAT3 PosL;
	XMFLOAT3 NormL;
	XMFLOAT2 Tex;
};

struct SurfaceInfo
{
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 SpecularMtrl;
};

struct Light
{
	XMFLOAT4 AmbientLight;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 SpecularLight;

	float SpecularPower;
	XMFLOAT3 LightVecW;
};

struct ConstantBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;
	
	SurfaceInfo surface;

	Light light;

	XMFLOAT3 EyePosW;
	float HasTexture;
};
#pragma endregion Structs

constexpr int AMOUNT_OF_CUBES = 5;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	UINT					_WindowHeight;
	UINT					_WindowWidth;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	DX::StepTimer			_stepTimer;

	// Core
	Microsoft::WRL::ComPtr<ID3D11Device>            _pd3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>     _pImmediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>          _pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  _pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>      _pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>       _pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>       _pVertexLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>	    _pSamplerLinear;
	Microsoft::WRL::ComPtr<ID3D11Buffer>            _pConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  _depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _DSLessEqual;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>		    _depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>   _RSCullNone;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>   _CCWcullMode;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>   _CWcullMode;

	// Objects
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pTextureRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pGroundTextureRV;
	Microsoft::WRL::ComPtr<ID3D11Buffer>             _pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>             _pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>             _pPlaneVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>             _pPlaneIndexBuffer;
	std::vector<GameObject*>						 _gameObjects;

	Light	 _basicLight;
	Camera * _camera;
	float	 _cameraOrbitRadius;
	float	 _cameraOrbitRadiusMin;
	float	 _cameraOrbitRadiusMax;
	float	 _cameraOrbitAngleXZ;
	float	 _cameraSpeed;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	void Update(const DX::StepTimer& timer);
	void UpdateCamera();

	void Draw();
	void PrepareDraw();
	ConstantBuffer PrepareConstantBuffer();

	void PrepareObjects();
	void moveObject(int objectNumber,
					const DirectX::SimpleMath::Vector3& force);

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	bool HandleKeyboard(MSG msg);
	void Tick();
};

