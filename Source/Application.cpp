#pragma warning (disable : 26812)

#include "Application.h"
#include "spdlog.h"
#include "sinks/msvc_sink.h"

using namespace DirectX;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;
using Microsoft::WRL::ComPtr;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

bool Application::HandleKeyboard(MSG msg)
{
	XMFLOAT3 cameraPosition = _camera->GetPosition();

	switch (msg.wParam)
	{
	case VK_UP:
		_cameraOrbitRadius = max(_cameraOrbitRadiusMin, _cameraOrbitRadius - (_cameraSpeed * 0.2f));
		return true;
		break;

	case VK_DOWN:
		_cameraOrbitRadius = min(_cameraOrbitRadiusMax, _cameraOrbitRadius + (_cameraSpeed * 0.2f));
		return true;
		break;

	case VK_RIGHT:
		_cameraOrbitAngleXZ -= _cameraSpeed;
		return true;
		break;

	case VK_LEFT:
		_cameraOrbitAngleXZ += _cameraSpeed;
		return true;
		break;
	}

	return false;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_WindowWidth = 0;
	_WindowHeight = 0;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pPlaneVertexBuffer = nullptr;
	_pPlaneIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;

	_CCWcullMode = nullptr;
	_CWcullMode = nullptr;
	_DSLessEqual = nullptr;
	_RSCullNone = nullptr;

	_basicLight = {};
	_camera = nullptr;
	_cameraOrbitRadius = 7.0f;
	_cameraOrbitRadiusMin = 2.0f;
	_cameraOrbitRadiusMax = 50.0f;
	_cameraOrbitAngleXZ = -90.0f;
	_cameraSpeed = 2.0f;

	_particleSystem = nullptr;

	// Set-up spdlog
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
	auto logger = std::make_shared<spdlog::logger>("LOGGER", sink);
	spdlog::register_logger(logger);
}

Application::~Application()
{
	_pd3dDevice.Reset();
	_pImmediateContext.Reset();
	_pSwapChain.Reset();
	_pRenderTargetView.Reset();
	_pVertexShader.Reset();
	_pPixelShader.Reset();
	_pVertexLayout.Reset();
	_pSamplerLinear.Reset();
	_pConstantBuffer.Reset();
	_depthStencilView.Reset();
	_depthStencilBuffer.Reset();
	_DSLessEqual.Reset();
	_RSCullNone.Reset();
	_CCWcullMode.Reset();
	_CWcullMode.Reset();
	_pTextureRV.Reset();
	_pGroundTextureRV.Reset();
	_pVertexBuffer.Reset();
	_pIndexBuffer.Reset();
	_pPlaneVertexBuffer.Reset();
	_pPlaneIndexBuffer.Reset();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Cleanup();
}

#pragma region Initialise

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	CreateDDSTextureFromFile(_pd3dDevice.Get(), L"Resources\\stone.dds", nullptr, _pTextureRV.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(_pd3dDevice.Get(), L"Resources\\floor.dds", nullptr, _pGroundTextureRV.ReleaseAndGetAddressOf());

    // Setup Camera
	XMFLOAT3 eye = XMFLOAT3(0.0f, 2.0f, -1.0f);
	XMFLOAT3 at = XMFLOAT3(0.0f, 2.0f, 0.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	_camera = new Camera(eye, at, up, (float)_WindowWidth, (float)_WindowHeight, 0.01f, 200.0f);

	// Setup the scene's light
	_basicLight.AmbientLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	_basicLight.DiffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_basicLight.SpecularLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	_basicLight.SpecularPower = 20.0f;
	_basicLight.LightVecW = XMFLOAT3(0.0f, 1.0f, -1.0f);

	PrepareObjects();

	// Set Fixed-Time Step for framerate (locked 60fps)
	_stepTimer.SetFixedTimeStep(true);
	_stepTimer.SetTargetElapsedSeconds(1.0f / 60.0f);

	InitImGUI();

	_debugDraw = new DebugDraw(_pd3dDevice.Get(), _pImmediateContext.Get());

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

	// Compile the vertex shader
	ComPtr<ID3DBlob> pVSBlob;
	hr = CompileShaderFromFile(L"Source/Shaders/DX11 Framework.fx", "VS", "vs_4_0", pVSBlob.ReleaseAndGetAddressOf());

	if (FAILED(hr))
	{
		MessageBox(nullptr,
				   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, _pVertexShader.ReleaseAndGetAddressOf());

	if (FAILED(hr))
	{
		pVSBlob.Reset();
		return hr;
	}

	// Compile the pixel shader
	ComPtr<ID3DBlob> pPSBlob;
	hr = CompileShaderFromFile(L"Source/Shaders/DX11 Framework.fx", "PS", "ps_4_0", pPSBlob.ReleaseAndGetAddressOf());

	if (FAILED(hr))
	{
		MessageBox(nullptr,
				   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		pPSBlob.Reset();
		return hr;
	}

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, _pPixelShader.ReleaseAndGetAddressOf());
	pPSBlob.Reset();

	if (FAILED(hr))
		return hr;

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
										pVSBlob->GetBufferSize(), _pVertexLayout.ReleaseAndGetAddressOf());
	pVSBlob->Release();

	if (FAILED(hr))
		return hr;

	// Set the input layout
	_pImmediateContext->IASetInputLayout(_pVertexLayout.Get());

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _pd3dDevice->CreateSamplerState(&sampDesc, _pSamplerLinear.ReleaseAndGetAddressOf());

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, _pVertexBuffer.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		return hr;

	// Create vertex buffer
	SimpleVertex planeVertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 5.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 5.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeVertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, _pPlaneVertexBuffer.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

	// Create index buffer
	WORD indices[] =
	{
		3, 1, 0,
		2, 1, 3,

		6, 4, 5,
		7, 4, 6,

		11, 9, 8,
		10, 9, 11,

		14, 12, 13,
		15, 12, 14,

		19, 17, 16,
		18, 17, 19,

		22, 20, 21,
		23, 20, 22
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, _pIndexBuffer.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		return hr;

	// Create plane index buffer
	WORD planeIndices[] =
	{
		0, 3, 1,
		3, 2, 1,
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeIndices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, _pPlaneIndexBuffer.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 960, 540};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"FGGC Semester 2 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ComPtr<ID3DBlob> pErrorBlob;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
							dwShaderFlags, 0, ppBlobOut, pErrorBlob.ReleaseAndGetAddressOf());

	if (FAILED(hr))
	{
		if (pErrorBlob != nullptr)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

		if (pErrorBlob)
			pErrorBlob.Reset();

		return hr;
	}

	if (pErrorBlob)
		pErrorBlob.Reset();

	return S_OK;
}

HRESULT Application::InitDevice()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	UINT sampleCount = 4;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = _WindowWidth;
	sd.BufferDesc.Height = _WindowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = sampleCount;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
										   D3D11_SDK_VERSION, &sd, _pSwapChain.ReleaseAndGetAddressOf(), _pd3dDevice.ReleaseAndGetAddressOf(), &_featureLevel, _pImmediateContext.ReleaseAndGetAddressOf());
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Create a render target view
	ComPtr<ID3D11Texture2D> pBackBuffer;
	hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)pBackBuffer.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		return hr;

	hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, _pRenderTargetView.ReleaseAndGetAddressOf());
	pBackBuffer.Reset();

	if (FAILED(hr))
		return hr;

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)_WindowWidth;
	vp.Height = (FLOAT)_WindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	_pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();
	InitVertexBuffer();
	InitIndexBuffer();

	// Set primitive topology
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, _pConstantBuffer.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		return hr;

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = sampleCount;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, _depthStencilBuffer.ReleaseAndGetAddressOf());
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer.Get(), nullptr, _depthStencilView.ReleaseAndGetAddressOf());

	_pImmediateContext->OMSetRenderTargets(1, _pRenderTargetView.GetAddressOf(), _depthStencilView.Get());

	// Rasterizer
	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, _RSCullNone.ReleaseAndGetAddressOf());

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	_pd3dDevice->CreateDepthStencilState(&dssDesc, _DSLessEqual.ReleaseAndGetAddressOf());

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, _CCWcullMode.ReleaseAndGetAddressOf());

	cmdesc.FrontCounterClockwise = false;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, _CWcullMode.ReleaseAndGetAddressOf());

	return S_OK;
}

void Application::InitImGUI()
{
	// Set-up Context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//Style
	ImGui::StyleColorsDark();

	// Render/Platform Bindings
	ImGui_ImplWin32_Init(_hWnd);
	ImGui_ImplDX11_Init(_pd3dDevice.Get(), _pImmediateContext.Get());
}

#pragma endregion Initialise

void Application::Cleanup()
{
	delete _debugDraw;
	_debugDraw = nullptr;

	delete _particleSystem;
	_particleSystem = nullptr;

	if (_camera)
	{
		delete _camera;
		_camera = nullptr;
	}

	for (auto gameObject : _gameObjects)
	{
		if (gameObject)
		{
			delete gameObject;
			gameObject = nullptr;
		}
	}
}

void Application::Tick()
{
	_stepTimer.Tick([&]()
	{
		Update(_stepTimer);
	});

	Draw();
}

void Application::PrepareObjects()
{
#pragma region ObjectResources
	Geometry cubeGeometry;
	cubeGeometry.indexBuffer = _pIndexBuffer.Get();
	cubeGeometry.vertexBuffer = _pVertexBuffer.Get();
	cubeGeometry.numberOfIndices = 36;
	cubeGeometry.vertexBufferOffset = 0;
	cubeGeometry.vertexBufferStride = sizeof(SimpleVertex);

	Geometry planeGeometry;
	planeGeometry.indexBuffer = _pPlaneIndexBuffer.Get();
	planeGeometry.vertexBuffer = _pPlaneVertexBuffer.Get();
	planeGeometry.numberOfIndices = 6;
	planeGeometry.vertexBufferOffset = 0;
	planeGeometry.vertexBufferStride = sizeof(SimpleVertex);

	Material shinyMaterial;
	shinyMaterial.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	shinyMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shinyMaterial.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	shinyMaterial.specularPower = 10.0f;

	Material noSpecMaterial;
	noSpecMaterial.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	noSpecMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	noSpecMaterial.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	noSpecMaterial.specularPower = 0.0f;
#pragma endregion ObjectResources

#pragma region FloorInit
	pmGameObject* planeObject = new pmGameObject(Vector3(0.0f, 0.0f, 0.0f),
												Vector3(XMConvertToRadians(90.0f), 0.0f, 0.0f),
												Vector3(15.0f, 15.0f, 15.0f),
												planeGeometry,
												noSpecMaterial);

	planeObject->GetParticleModel()->SetInverseMass(0.0f);
	planeObject->GetAppearance()->SetTextureRV(_pGroundTextureRV.Get());

	_gameObjects.push_back(planeObject);
#pragma endregion FloorInit

#pragma region CubesInit
	Matrix cubeTensor = Matrix::Identity;
	cubeTensor._11 = (10.0f * ((0.5f * 0.5f) + (0.5f * 0.5f))) / 12.0f;
	cubeTensor._22 = (10.0f * ((0.5f * 0.5f) + (0.5f * 0.5f))) / 12.0f;
	cubeTensor._33 = (10.0f * ((0.5f * 0.5f) + (0.5f * 0.5f))) / 12.0f;


	for (auto i = 0; i < AMOUNT_OF_CUBES; ++i)
	{
		rbGameObject* cubeObject = new rbGameObject(Vector3(-4.0f + (i * 2.0f), 0.5f, 10.0f),
													Vector3(0.0f, 0.0f, 0.0f),
													Vector3(0.5f, 0.5f, 0.5f),
													cubeGeometry,
													shinyMaterial);

		cubeObject->GetRigidBody()->SetMass(10.0f);
		cubeObject->GetRigidBody()->SetInertiaTensor(cubeTensor);
		cubeObject->SetBoudningSphereRadius(0.5f);
		cubeObject->GetAppearance()->SetTextureRV(_pTextureRV.Get());

		_gameObjects.push_back(cubeObject);
	}
#pragma endregion CubesInit

	_particleSystem = new ParticleSystem(50,
										 2.0f,
										 new Transform(Vector3(), Vector3(), Vector3(0.25f, 0.25f, 0.25f)),
										 new Appearance(cubeGeometry, shinyMaterial, _pTextureRV.Get()));
}

void Application::moveObject(int objectNumber,
							 const DirectX::SimpleMath::Vector3& force)
{
	rbGameObject* rbObject = static_cast<rbGameObject*>(_gameObjects[objectNumber]);
	rbObject->GetRigidBody()->AddForce(force, Vector3(0.0f, 0.0f, 0.0f));
}

void Application::Update(const DX::StepTimer& timer)
{
	float deltaTime = static_cast<float>(timer.GetElapsedSeconds());

	// Move gameobject
	if (GetAsyncKeyState('1'))
	{
		moveObject(1, Vector3(3.0f, 0.0f, 0.0f));
	}

	if (GetAsyncKeyState('2'))
	{
		moveObject(1, Vector3(-3.0f, 0.0f, 0.0f));
	}

	UpdateCamera();

	// Update objects
	for (auto gameObject : _gameObjects)
	{
		gameObject->Update(deltaTime);
	}

	_particleSystem->Update(deltaTime);
}

void Application::UpdateCamera()
{
	float angleAroundZ = XMConvertToRadians(_cameraOrbitAngleXZ);
	float x = _cameraOrbitRadius * cosf(angleAroundZ);
	float z = _cameraOrbitRadius * sinf(angleAroundZ);

	XMFLOAT3 cameraPos = _camera->GetPosition();
	cameraPos.x = x;
	cameraPos.z = z;

	_camera->SetPosition(cameraPos);
	_camera->Update();
}

void Application::PrepareDraw()
{
	// Clear buffers
	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // red,green,blue,alpha
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView.Get(), ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ImGUI New Frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Setup buffers and render scene
	_pImmediateContext->IASetInputLayout(_pVertexLayout.Get());

	_pImmediateContext->VSSetShader(_pVertexShader.Get(), nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShader.Get(), nullptr, 0);

	_pImmediateContext->VSSetConstantBuffers(0, 1, _pConstantBuffer.GetAddressOf());
	_pImmediateContext->PSSetConstantBuffers(0, 1, _pConstantBuffer.GetAddressOf());
	_pImmediateContext->PSSetSamplers(0, 1, _pSamplerLinear.GetAddressOf());
}

ConstantBuffer Application::PrepareConstantBuffer()
{
	ConstantBuffer cb;

	XMFLOAT4X4 viewAsFloats = _camera->GetView();
	XMFLOAT4X4 projectionAsFloats = _camera->GetProjection();
	XMMATRIX view = XMLoadFloat4x4(&viewAsFloats);
	XMMATRIX projection = XMLoadFloat4x4(&projectionAsFloats);

	cb.View = XMMatrixTranspose(view);
	cb.Projection = XMMatrixTranspose(projection);

	return cb;
}

void Application::Draw()
{
	// Skip render, if coming before first update
	if (_stepTimer.GetFrameCount() == 0)
	{
		return;
	}

	PrepareDraw();

	ConstantBuffer cb = PrepareConstantBuffer();

	cb.light = _basicLight;
	cb.EyePosW = _camera->GetPosition();

	// Render all scene objects
	for (auto gameObject : _gameObjects)
	{
		// Get render material
		Material material = gameObject->GetAppearance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material.ambient;
		cb.surface.DiffuseMtrl = material.diffuse;
		cb.surface.SpecularMtrl = material.specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(gameObject->GetWorldMatrix());

		// Set texture
		if (gameObject->GetAppearance()->HasTexture())
			cb.HasTexture = 1.0f;
		else
			cb.HasTexture = 0.0f;

		// Update constant buffer
		_pImmediateContext->UpdateSubresource(_pConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);

		// Draw object
		gameObject->Render(_pImmediateContext.Get());
	}

	// Draw Debug Bounding Spheres
	//for (int i = 1; i <= AMOUNT_OF_CUBES; ++i)
	//{
	//	_debugDraw->DrawBoundingSphere(_pImmediateContext.Get(), cb, _gameObjects[i]->GetTransform()->GetPosition(), _gameObjects[i]->GetTransform()->GetScale());
	//}

	_particleSystem->Render(_pImmediateContext.Get(), cb, _pConstantBuffer.Get());

	// ImGUI Window
	ImGui::Begin("Debug Console");
	ImGui::Text("FPS: %d", _stepTimer.GetFramesPerSecond());
	ImGui::End();

	// Render ImGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present our back buffer to our front buffer
    _pSwapChain->Present(0, 0);
}