#include "DxManager.h"
#include "DxManager_Events.h"
#include "Exceptions.h"
#include "ResourceManager.h"

DxManager::DxManager( HWND g_hWnd, GraphicsEngineParams params, Camera* cam )
	: zParams(params)
{
	this->hWnd = g_hWnd;
	this->zCamera = cam;

	this->Dx_DeviceContext = NULL;
	this->Dx_DepthStencilView = NULL;
	this->Dx_DepthStencil = NULL;
	this->Dx_RenderTargetView = NULL;

	this->Dx_SwapChain = NULL;
	this->Dx_Device = NULL;

	this->zSkyBox = NULL;
	this->zQuadRenderer = NULL;

	this->zShader_Text = NULL;
	this->zShader_Image = NULL;
	this->zShader_Final = NULL;
	this->zShader_SkyBox = NULL;
	this->zShader_ShadowMap = NULL;
	this->zShader_SRV_Debug = NULL;
	this->zShader_PointLight = NULL;
	this->zShader_DeferredGeo = NULL;
	this->zShader_CubeShadowMap = NULL;
	this->zShader_DeferredTerrainGeo = NULL;

	if (FAILED(this->Init()))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

}

char* FeatureLevelToString(D3D_FEATURE_LEVEL featureLevel)
{
	if(featureLevel == D3D_FEATURE_LEVEL_11_0)
		return "11.0";
	if(featureLevel == D3D_FEATURE_LEVEL_10_1)
		return "10.1";
	if(featureLevel == D3D_FEATURE_LEVEL_10_0)
		return "10.0";

	return "Unknown";
}

HRESULT DxManager::Init()
{
	HRESULT hr = S_OK;;

	RECT rc;
	GetClientRect(hWnd, &rc);
	int screenWidth = rc.right - rc.left;;
	int screenHeight = rc.bottom - rc.top;

	CJen::Debug("Initiating Engine with width: " + CJen::convertNrToString((float)screenWidth) +
		" and height: " + CJen::convertNrToString((float)screenHeight));

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverType;

	D3D_DRIVER_TYPE driverTypes[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = screenWidth;
	sd.BufferDesc.Height = screenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = this->zParams.RefreshRate;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevelsToTry[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	D3D_FEATURE_LEVEL initiatedFeatureLevel;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			driverType,
			NULL,
			createDeviceFlags,
			featureLevelsToTry,
			ARRAYSIZE(featureLevelsToTry),
			D3D11_SDK_VERSION,
			&sd,
			&Dx_SwapChain,
			&Dx_Device,
			&initiatedFeatureLevel,
			&Dx_DeviceContext);

		if( SUCCEEDED( hr ) )
		{
			char title[256];
			sprintf_s(
				title,
				sizeof(title),
				"CJenEngine - Direct3D 11.0 | Direct3D 11.0 device initiated with Direct3D %s feature level",
				FeatureLevelToString(initiatedFeatureLevel)
				);
			SetWindowText(hWnd, title);

			break;
		}
	}
	if( FAILED(hr) )
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer;
	hr = Dx_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return hr;

	hr = Dx_Device->CreateRenderTargetView( pBackBuffer, NULL, &Dx_RenderTargetView );
	pBackBuffer->Release();
	if( FAILED(hr) )
		return hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = screenWidth;
	descDepth.Height = screenHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = Dx_Device->CreateTexture2D( &descDepth, NULL, &Dx_DepthStencil );
	if( FAILED(hr) )
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = Dx_Device->CreateDepthStencilView(Dx_DepthStencil, &descDSV, &Dx_DepthStencilView);
	if( FAILED(hr) )
		return hr;

	Dx_DeviceContext->OMSetRenderTargets(1, &Dx_RenderTargetView, Dx_DepthStencilView);

	// Setup the Dx_Viewport
	Dx_Viewport.Width = (float)screenWidth;
	Dx_Viewport.Height = (float)screenHeight;
	Dx_Viewport.MinDepth = 0.0f;
	Dx_Viewport.MaxDepth = 1.0f;
	Dx_Viewport.TopLeftX = 0;
	Dx_Viewport.TopLeftY = 0;
	Dx_DeviceContext->RSSetViewports(1, &Dx_Viewport);

	this->InitShaders();

	this->InitRenderTargets(COLOR, DXGI_FORMAT_R8G8B8A8_UNORM);
	this->InitRenderTargets(NORMAL, DXGI_FORMAT_R16G16B16A16_UNORM);
	this->InitRenderTargets(LIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	this->InitRenderTargets(DEPTH, DXGI_FORMAT_R32_FLOAT);
	this->InitRenderTargets(FINAL, DXGI_FORMAT_R8G8B8A8_UNORM);

	//Init
	this->zQuadRenderer = new QuadRenderer();
	this->zQuadRenderer->Init(this->Dx_Device, this->Dx_DeviceContext);

	float x = 1.0f / this->zParams.WindowWidth;
	float y = 1.0f / this->zParams.WindowHeight;
	this->zHalfPixel = D3DXVECTOR2(x, y);

	try
	{
		this->InitVertexBufferDebugging();
	}
	catch(VertexBufferException e)
	{
		
	}

	

	ResourceManagerInit(this->Dx_Device, this->Dx_DeviceContext);

	//FpsUtil* util = this->zGameTimer.GetFpsUtility();
	//util->SetFPSUpperLimit(this->zParams.MaxFPS);
	//util->SetFPSLowerLimit(this->zParams.MinFps);

	return S_OK;
}

HRESULT DxManager::InitRenderTargets(unsigned int target, DXGI_FORMAT format)
{
	ID3D11Texture2D* color = NULL;
	D3D11_TEXTURE2D_DESC GBufferTextureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC DescRT;
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;

	ZeroMemory(&GBufferTextureDesc, sizeof(GBufferTextureDesc));
	GBufferTextureDesc.Width = this->zParams.WindowWidth;
	GBufferTextureDesc.Height = this->zParams.WindowHeight;	
	GBufferTextureDesc.MipLevels = 1;
	GBufferTextureDesc.ArraySize = 1;
	GBufferTextureDesc.Format = format;
	GBufferTextureDesc.SampleDesc.Count = 1;
	GBufferTextureDesc.SampleDesc.Quality = 0;
	GBufferTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	GBufferTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	GBufferTextureDesc.CPUAccessFlags = 0;
	GBufferTextureDesc.MiscFlags = 0;

	if(FAILED(this->Dx_Device->CreateTexture2D(&GBufferTextureDesc, NULL, &color)))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	ZeroMemory(&DescRT, sizeof(DescRT));
	DescRT.Format = format;
	DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	DescRT.Texture2DArray.ArraySize = 1;
	DescRT.Texture2DArray.MipSlice = 0;

	if(FAILED(this->Dx_Device->CreateRenderTargetView(color, &DescRT, &this->Dx_GBufferRTV[target])))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	ZeroMemory(&srDesc, sizeof(srDesc));
	srDesc.Format = format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	if(FAILED(this->Dx_Device->CreateShaderResourceView(color, &srDesc, &this->Dx_GBufferSRV[target])))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	return S_OK;
}

HRESULT DxManager::InitShaders()
{
	//Create Input Layouts
	static const D3D11_INPUT_ELEMENT_DESC inputDescVertexNormalMap[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	static const D3D11_INPUT_ELEMENT_DESC inputDescVertex[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	static const D3D11_INPUT_ELEMENT_DESC inputDescVertexTex[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	static const D3D11_INPUT_ELEMENT_DESC inputDescVertexTex3[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	static const D3D11_INPUT_ELEMENT_DESC inputDescVertexTexNorm[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	//Create Shaders

	// Deferred Rendering Geometry pass
	this->zShader_DeferredGeo = new Shader();
	if (FAILED(this->zShader_DeferredGeo->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/DeferredGeometry.fx", 
		inputDescVertexNormalMap, sizeof(inputDescVertexNormalMap) / sizeof(D3D11_INPUT_ELEMENT_DESC))))
	{
		throw(ShaderException(
			__FILE__, 
			__LINE__, 
			__FUNCTION__, 
			"Failed to create fx file Shaders/DeferredGeometry.fx"));
	}

	// Deferred Rendering Geometry pass
	this->zShader_DeferredTerrainGeo = new Shader();
	if (FAILED(this->zShader_DeferredTerrainGeo->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/DeferredGeometryTerrain.fx", 
		inputDescVertexTexNorm, sizeof(inputDescVertexTexNorm) / sizeof(D3D11_INPUT_ELEMENT_DESC))))
	{
		throw(ShaderException(
			__FILE__, 
			__LINE__, 
			__FUNCTION__, 
			"Failed to create fx file Shaders/DeferredGeometryTerrain.fx"));
	}

	//SkyBox Shader
	this->zShader_SkyBox = new Shader();
	if (FAILED(this->zShader_SkyBox->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/SkyBox.fx", 
		inputDescVertex, sizeof(inputDescVertex) / sizeof(D3D11_INPUT_ELEMENT_DESC))))
	{
		throw(ShaderException(
			__FILE__, 
			__LINE__, 
			__FUNCTION__, 
			"Failed to create fx file Shaders/Sky.fx"));
	}

	//Combined Shader
	this->zShader_Final = new Shader();
	if (FAILED(this->zShader_Final->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/FinalShader.fx", 
		inputDescVertexTex, sizeof(inputDescVertexTex) / sizeof(D3D11_INPUT_ELEMENT_DESC))))
	{
		throw(ShaderException(
			__FILE__, 
			__LINE__, 
			__FUNCTION__, 
			"Failed to create fx file Shaders/FinalShader.fx"));
	}

	//RenderTarget Debug Shader
	this->zShader_SRV_Debug = new Shader();
	if (FAILED(this->zShader_SRV_Debug->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/ShaderResourceDebug.fx", 
		inputDescVertexTex, sizeof(inputDescVertexTex) / sizeof(D3D11_INPUT_ELEMENT_DESC))))
	{
		throw(ShaderException(
			__FILE__, 
			__LINE__, 
			__FUNCTION__, 
			"Failed to create fx file Shaders/ShaderResourceDebug.fx"));
	}

	// For Images
	this->zShader_Image = new Shader();
	if(FAILED(this->zShader_Image->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/ImageRenderer.fx", NULL, 0)))
	{
		throw(ShaderException(
			__FILE__, 
			__LINE__, 
			__FUNCTION__, 
			"Failed to create fx file Shaders/ImageRenderer.fx"));
	}

	// For Text
	this->zShader_Text = new Shader();
	if(FAILED(this->zShader_Text->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/TextRenderer.fx", NULL, 0)))
	{
		throw(ShaderException(
			__FILE__, 
			__LINE__, 
			__FUNCTION__, 
			"Failed to create fx file Shaders/TextRenderer.fx"));
	}

	//For PointLight
	this->zShader_PointLight = new Shader();
	if (FAILED(this->zShader_PointLight->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/PointLight.fx", 
		inputDescVertexNormalMap, sizeof(inputDescVertexNormalMap) / sizeof(D3D11_INPUT_ELEMENT_DESC))))
	{
		throw(ShaderException(
			__FILE__, 
			__LINE__, 
			__FUNCTION__, 
			"Failed to create fx file Shaders/PointLight.fx"));
	}

	//For PointLight ShadowMaps
	this->zShader_CubeShadowMap = new Shader();
	if (FAILED(this->zShader_CubeShadowMap->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/CubeShadowMap.fx", 
		inputDescVertexTex, sizeof(inputDescVertexTex) / sizeof(D3D11_INPUT_ELEMENT_DESC))))
	{
		throw(ShaderException(
			__FILE__, 
			__LINE__, 
			__FUNCTION__, 
			"Failed to create fx file Shaders/CubeShadowMap.fx"));
	}

	////For SpotLight ShadowMaps
	//this->zShader_ShadowMap = new Shader();
	//if (FAILED(this->zShader_ShadowMap->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/ShadowMap.fx", 
	//	inputDescVertexTex, sizeof(inputDescVertexTex) / sizeof(D3D11_INPUT_ELEMENT_DESC))))
	//{
	//	throw(ShaderException(
	//		__FILE__, 
	//		__LINE__, 
	//		__FUNCTION__, 
	//		"Failed to create fx file Shaders/ShadowMap.fx"));
	//}

	//For ShadowMap Debugging
	this->zShader_DebugCubeMapSRV = new Shader();
	if (FAILED(this->zShader_DebugCubeMapSRV->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/ShadowMapDebug.fx", 
		inputDescVertexTex3, sizeof(inputDescVertexTex3) / sizeof(D3D11_INPUT_ELEMENT_DESC))))
	{
		throw(ShaderException(
			__FILE__, 
			__LINE__, 
			__FUNCTION__, 
			"Failed to create fx file Shaders/ShadowMapDebug.fx"));
	}

	return S_OK;
}

HRESULT DxManager::InitVertexBufferDebugging()
{
	HRESULT hr;

	//Create Vertex Buffers for render target debugging
	D3DXVECTOR3 vbPosition[MAX_RENDER_TARGET_DEBUG][6];
	D3DXVECTOR2 vbTexCoord[6];

	vbTexCoord[0] = D3DXVECTOR2(1.0f, 1.0f);
	vbTexCoord[1] = D3DXVECTOR2(0.0f, 1.0f);
	vbTexCoord[2] = D3DXVECTOR2(0.0f, 0.0f);

	vbTexCoord[3] = D3DXVECTOR2(0.0f, 0.0f);
	vbTexCoord[4] = D3DXVECTOR2(1.0f, 0.0f);
	vbTexCoord[5] = D3DXVECTOR2(1.0f, 1.0f);

	//Vertex buffer 1
	vbPosition[0][0] = D3DXVECTOR3(1.0f,  0.75f, 0.0f);
	vbPosition[0][1] = D3DXVECTOR3(0.75f, 0.75f, 0.0f);
	vbPosition[0][2] = D3DXVECTOR3(0.75f, 1.0f, 0.0f);

	vbPosition[0][3] = D3DXVECTOR3(0.75f, 1.0f, 0.0f);
	vbPosition[0][4] = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	vbPosition[0][5] = D3DXVECTOR3(1.0f, 0.75f, 0.0f);

	//Vertex Buffer 2
	vbPosition[1][0] = D3DXVECTOR3(0.75f, 0.75f, 0.0f);
	vbPosition[1][1] = D3DXVECTOR3(0.5f,  0.75f, 0.0f);
	vbPosition[1][2] = D3DXVECTOR3(0.5f,  1.0f,  0.0f);

	vbPosition[1][3] = D3DXVECTOR3(0.5f,   1.0f,  0.0f);
	vbPosition[1][4] = D3DXVECTOR3(0.75f,  1.0f,  0.0f);
	vbPosition[1][5] = D3DXVECTOR3(0.75f,  0.75f, 0.0f);

	//Vertex Buffer 3
	vbPosition[2][0] = D3DXVECTOR3(0.5f,  0.75f, 0.0f);
	vbPosition[2][1] = D3DXVECTOR3(0.25f, 0.75f, 0.0f);
	vbPosition[2][2] = D3DXVECTOR3(0.25f, 1.0f,  0.0f);

	vbPosition[2][3] = D3DXVECTOR3(0.25f, 1.0f,  0.0f);
	vbPosition[2][4] = D3DXVECTOR3(0.5f,  1.0f,  0.0f);
	vbPosition[2][5] = D3DXVECTOR3(0.5f,  0.75f, 0.0f);

	//Vertex Buffer 4
	vbPosition[3][0] = D3DXVECTOR3(0.25f, 0.75f, 0.0f);
	vbPosition[3][1] = D3DXVECTOR3(0.0f,  0.75f, 0.0f);
	vbPosition[3][2] = D3DXVECTOR3(0.0f,  1.0f,  0.0f);

	vbPosition[3][3] = D3DXVECTOR3(0.0f,  1.0f,  0.0f);
	vbPosition[3][4] = D3DXVECTOR3(0.25f, 1.0f,  0.0f);
	vbPosition[3][5] = D3DXVECTOR3(0.25f, 0.75f, 0.0f);

	VertexTex vt[MAX_RENDER_TARGET_DEBUG][6];

	for (int i = 0; i < MAX_RENDER_TARGET_DEBUG; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			vt[i][j].position = vbPosition[i][j];
			vt[i][j].texCoord = vbTexCoord[j];
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VertexTex) * 6;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initDesc;
	for (int i = 0; i < MAX_RENDER_TARGET_DEBUG; i++)
	{
		initDesc.pSysMem = &vt[i];
		hr = this->Dx_Device->CreateBuffer(&vbd, &initDesc, &this->zVertexBuffer[i]);

		if (FAILED(hr))
		{
			string errorMsg = "Buffer for render target ";
			CJen::addNrToString(i, errorMsg);
			throw(VertexBufferException(__FILE__, __LINE__, __FUNCTION__, errorMsg));
		}
	}

	//Create Vertex Buffer for Cube Map Debugging

	//Side +Z
	D3DXVECTOR3 pos0[] = 
	{
		D3DXVECTOR3(1.0f,  0.25f, 0.0f), //lower right
		D3DXVECTOR3(0.75f, 0.25f, 0.0f), //lower left
		D3DXVECTOR3(0.75f, 0.5f, 0.0f), //upper left

		D3DXVECTOR3(0.75f, 0.5f, 0.0f),  //upper left
		D3DXVECTOR3(1.0f, 0.5f, 0.0f),	//Upper right
		D3DXVECTOR3(1.0f, 0.25f, 0.0f) //lower right
	};
	D3DXVECTOR3 uv0[] = 
	{
		D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
		D3DXVECTOR3( 1.0f, -1.0f, 1.0f),
		D3DXVECTOR3( 1.0f,  1.0f, 1.0f),

		D3DXVECTOR3( 1.0f,  1.0f, 1.0f),
		D3DXVECTOR3(-1.0f,  1.0f, 1.0f),
		D3DXVECTOR3(-1.0f, -1.0f, 1.0f)
	};
	//Side +X
	D3DXVECTOR3 pos1[] = 
	{
		D3DXVECTOR3(0.25f,  0.25f, 0.0f), //lower right
		D3DXVECTOR3(0.0f, 0.25f, 0.0f), //lower left
		D3DXVECTOR3(0.0f, 0.5f, 0.0f),  //upper left

		D3DXVECTOR3(0.0f, 0.5f, 0.0f),  //upper left
		D3DXVECTOR3(0.25f, 0.5f, 0.0f), //Upper right
		D3DXVECTOR3(0.25f, 0.25f, 0.0f) //lower right
	};
	D3DXVECTOR3 uv1[] = 
	{
		D3DXVECTOR3(-1.0f, -1.0f, -1.0f), 
		D3DXVECTOR3(-1.0f, -1.0f,  1.0f), 
		D3DXVECTOR3(-1.0f,  1.0f,  1.0f),

		D3DXVECTOR3(-1.0f,  1.0f,  1.0f), 
		D3DXVECTOR3(-1.0f,  1.0f, -1.0f),
		D3DXVECTOR3(-1.0f, -1.0f, -1.0f)
	};
	//Side -z
	D3DXVECTOR3 pos2[] = 
	{
		D3DXVECTOR3(0.5f,  0.25f, 0.0f), //lower right
		D3DXVECTOR3(0.25f, 0.25f, 0.0f), //lower left
		D3DXVECTOR3(0.25f, 0.5f, 0.0f),  //upper left

		D3DXVECTOR3(0.25f, 0.5f, 0.0f),  //upper left
		D3DXVECTOR3(0.5f, 0.5f, 0.0f), //Upper right
		D3DXVECTOR3(0.5f, 0.25f, 0.0f) //lower right
	};
	D3DXVECTOR3 uv2[] = 
	{
		D3DXVECTOR3( 1.0f, -1.0f, -1.0f),
		D3DXVECTOR3(-1.0f, -1.0f, -1.0f),
		D3DXVECTOR3(-1.0f,  1.0f, -1.0f),

		D3DXVECTOR3(-1.0f,  1.0f, -1.0f),
		D3DXVECTOR3( 1.0f,  1.0f, -1.0f),
		D3DXVECTOR3( 1.0f, -1.0f, -1.0f)
	};

	//Side -x
	D3DXVECTOR3 pos3[] = 
	{
		D3DXVECTOR3(0.75f,  0.25f, 0.0f), //lower right
		D3DXVECTOR3(0.5f, 0.25f, 0.0f), //lower left
		D3DXVECTOR3(0.5f, 0.5f, 0.0f),  //upper left

		D3DXVECTOR3(0.5f, 0.5f, 0.0f),  //upper left
		D3DXVECTOR3(0.75f, 0.5f, 0.0f), //Upper right
		D3DXVECTOR3(0.75f, 0.25f, 0.0f) //lower right
	};
	D3DXVECTOR3 uv3[] = 
	{
		D3DXVECTOR3( 1.0f, -1.0f,  1.0f), 
		D3DXVECTOR3( 1.0f, -1.0f, -1.0f), 
		D3DXVECTOR3( 1.0f,  1.0f, -1.0f),

		D3DXVECTOR3( 1.0f,  1.0f, -1.0f), 
		D3DXVECTOR3( 1.0f,  1.0f,  1.0f),
		D3DXVECTOR3( 1.0f, -1.0f,  1.0f)
	};
	//Side +Y
	D3DXVECTOR3 pos4[] = 
	{
		D3DXVECTOR3(0.5f,  0.5f, 0.0f), //lower right
		D3DXVECTOR3(0.25f, 0.5f, 0.0f), //lower left
		D3DXVECTOR3(0.25f, 0.75f, 0.0f),  //upper left

		D3DXVECTOR3(0.25f, 0.75f, 0.0f),  //upper left
		D3DXVECTOR3(0.5f, 0.75f, 0.0f), //Upper right
		D3DXVECTOR3(0.5f, 0.5f, 0.0f) //lower right
	};
	D3DXVECTOR3 uv4[] = 
	{
		D3DXVECTOR3( 1.0f,  1.0f, -1.0f),
		D3DXVECTOR3(-1.0f,  1.0f, -1.0f), 
		D3DXVECTOR3(-1.0f,  1.0f,  1.0f),

		D3DXVECTOR3(-1.0f,  1.0f,  1.0f), 
		D3DXVECTOR3( 1.0f,  1.0f,  1.0f),
		D3DXVECTOR3( 1.0f,  1.0f, -1.0f)
	};
	//Side -Y
	D3DXVECTOR3 pos5[] = 
	{
		D3DXVECTOR3(0.5f,  0.0f, 0.0f), //lower right
		D3DXVECTOR3(0.25f, 0.0f, 0.0f), //lower left
		D3DXVECTOR3(0.25f, 0.25f, 0.0f),  //upper left

		D3DXVECTOR3(0.25f, 0.25f, 0.0f),  //upper left
		D3DXVECTOR3(0.5f, 0.25f, 0.0f), //Upper right
		D3DXVECTOR3(0.5f, 0.0f, 0.0f) //lower right
	};
	D3DXVECTOR3 uv5[] = 
	{
		D3DXVECTOR3( 1.0f, -1.0f,  1.0f),
		D3DXVECTOR3(-1.0f, -1.0f,  1.0f), 
		D3DXVECTOR3(-1.0f, -1.0f, -1.0f),

		D3DXVECTOR3(-1.0f, -1.0f, -1.0f), 
		D3DXVECTOR3( 1.0f, -1.0f, -1.0f),
		D3DXVECTOR3( 1.0f, -1.0f,  1.0f)
	};

	VertexTex3 qv0[6];
	VertexTex3 qv1[6];
	VertexTex3 qv2[6];
	VertexTex3 qv3[6];
	VertexTex3 qv4[6];
	VertexTex3 qv5[6];
	for(int i = 0; i < 6; i++)
	{
		qv0[i].position = pos0[i];
		qv0[i].texCoord = uv0[i];

		qv1[i].position = pos1[i];
		qv1[i].texCoord = uv1[i];

		qv2[i].position = pos2[i];
		qv2[i].texCoord = uv2[i];

		qv3[i].position = pos3[i];
		qv3[i].texCoord = uv3[i];

		qv4[i].position = pos4[i];
		qv4[i].texCoord = uv4[i];

		qv5[i].position = pos5[i];
		qv5[i].texCoord = uv5[i];
	}

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(VertexTex3) * 6;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA subRes_Data;

	subRes_Data.pSysMem = qv0;
	hr = this->Dx_Device->CreateBuffer(&bufferDesc, &subRes_Data, &this->zCubeMapVertexBuffer[0]);

	if (FAILED(hr))
		throw(VertexBufferException(__FILE__, __LINE__, __FUNCTION__, "Buffer for cubeMap side 1"));

	subRes_Data.pSysMem = qv1;
	this->Dx_Device->CreateBuffer(&bufferDesc, &subRes_Data, &this->zCubeMapVertexBuffer[1]);

	if (FAILED(hr))
		throw(VertexBufferException(__FILE__, __LINE__, __FUNCTION__, "Buffer for cubeMap side 2"));

	subRes_Data.pSysMem = qv2;
	this->Dx_Device->CreateBuffer(&bufferDesc, &subRes_Data, &this->zCubeMapVertexBuffer[2]);

	if (FAILED(hr))
		throw(VertexBufferException(__FILE__, __LINE__, __FUNCTION__, "Buffer for cubeMap side 3"));

	subRes_Data.pSysMem = qv3;
	this->Dx_Device->CreateBuffer(&bufferDesc, &subRes_Data, &this->zCubeMapVertexBuffer[3]);

	if (FAILED(hr))
		throw(VertexBufferException(__FILE__, __LINE__, __FUNCTION__, "Buffer for cubeMap side 4"));

	subRes_Data.pSysMem = qv4;
	this->Dx_Device->CreateBuffer(&bufferDesc, &subRes_Data, &this->zCubeMapVertexBuffer[4]);

	if (FAILED(hr))
		throw(VertexBufferException(__FILE__, __LINE__, __FUNCTION__, "Buffer for cubeMap side 5"));

	subRes_Data.pSysMem = qv5;
	this->Dx_Device->CreateBuffer(&bufferDesc, &subRes_Data, &this->zCubeMapVertexBuffer[5]);

	if (FAILED(hr))
		throw(VertexBufferException(__FILE__, __LINE__, __FUNCTION__, "Buffer for cubeMap side 6"));

	return S_OK;
}

DxManager::~DxManager()
{
	SAFE_DELETE(this->zSkyBox);
	SAFE_DELETE(this->zQuadRenderer);

	for (auto it = this->zObjects.begin(); it != this->zObjects.end(); it++)
	{
		StaticMesh* mesh = (*it);
		if (mesh)
			delete mesh;
	}
	this->zObjects.clear();

	for (auto it = this->zTerrains.begin(); it != this->zTerrains.end(); it++)
	{
		Terrain* tmp = (*it);
		if (tmp)
			delete tmp;
	}
	this->zTerrains.clear();

	for (auto it = this->zImages.begin(); it != this->zImages.end(); it++)
	{
		Image* obj = (*it);
		if (obj)
			delete obj;
	}
	this->zImages.clear();

	for (auto it_PointLight = this->zPointLights.begin(); it_PointLight != this->zPointLights.end(); it_PointLight++)
	{
		PointLight* obj = (*it_PointLight);
		if (obj)
			delete obj;
	}
	this->zPointLights.clear();

	for (auto it_SpotLight = this->zSpotLights.begin(); it_SpotLight != this->zSpotLights.end(); it_SpotLight++)
	{
		SpotLight* obj = (*it_SpotLight);
		if (obj)
			delete obj;
	}
	this->zPointLights.clear();

	for (auto it = this->zTexts.begin(); it != this->zTexts.end(); it++)
	{
		Text* obj = (*it);
		if (obj)
			delete obj;
	}
	this->zTexts.clear();

	for (int i = 0; i < MAX_RENDER_TARGET_DEBUG; i++)
	{
		SAFE_RELEASE(this->zVertexBuffer[i]);
	}

	for (int i = 0; i < 6; i++)
	{
		SAFE_RELEASE(this->zCubeMapVertexBuffer[i]);
	}

	for (int i = 0; i < NUMBER_OF_VIEWS; i++)
	{
		SAFE_RELEASE(this->Dx_GBufferRTV[i]);
		SAFE_RELEASE(this->Dx_GBufferSRV[i]);
	}

	SAFE_DELETE(this->zShader_Text);
	SAFE_DELETE(this->zShader_Image);
	SAFE_DELETE(this->zShader_Final);
	SAFE_DELETE(this->zShader_SkyBox);
	SAFE_DELETE(this->zShader_SRV_Debug);
	SAFE_DELETE(this->zShader_ShadowMap);
	SAFE_DELETE(this->zShader_PointLight);
	SAFE_DELETE(this->zShader_DeferredGeo);
	SAFE_DELETE(this->zShader_CubeShadowMap);
	SAFE_DELETE(this->zShader_DebugCubeMapSRV);
	SAFE_DELETE(this->zShader_DeferredTerrainGeo);
	
	SAFE_RELEASE(this->Dx_Device);
	SAFE_RELEASE(this->Dx_SwapChain);
	SAFE_RELEASE(this->Dx_DepthStencil);
	SAFE_RELEASE(this->Dx_DeviceContext);
	SAFE_RELEASE(this->Dx_DepthStencilView);
	SAFE_RELEASE(this->Dx_RenderTargetView);
	
	FreeResourceManager();
}

void DxManager::StartRendering()
{
	this->zStartRender = true;
}

void DxManager::ResetPerfLogging()
{
#ifdef PERFORMANCE_TEST
	this->zPerf.ResetAll();
#endif
}

void DxManager::PrintPerfLogging()
{
#ifdef PERFORMANCE_TEST
	this->zPerf.GenerateReport(this->zParams);
#endif
}

void DxManager::Life()
{
	//Engine Start Splash screen.
	Image* img = new Image(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2((float)this->zParams.WindowWidth, (float)this->zParams.WindowHeight));
	TextureResource* tex = NULL;
	tex = GetResourceManager()->CreateTextureResourceFromFile("Media/LoadingScreen/StartingSplash.png");
	img->SetTexture(tex);
	img->SetStrata(10.0f);
	this->zImages.push_back(img);

	while(!this->zStartRender)
	{
		this->Render();
	}
	this->DeleteImage(img);
	img = NULL;
	Sleep(100);

#ifdef PERFORMANCE_TEST
	this->zPerf.ResetAll();
#endif

	this->zGameTimer.Init();

	while (this->stayAlive)
	{
#ifdef PERFORMANCE_TEST
		this->zPerf.PreMeasure("Renderer - Entire Frame", 0);
#endif
		//Listen for events
		if(GetForegroundWindow() != this->hWnd)	// Sleep a little if you're alt tabbed out of the game to prevent desktop lag.
			Sleep((DWORD)10);
		
		this->zGameTimer.Frame();

#ifdef PERFORMANCE_TEST
		this->zPerf.PreMeasure("Renderer - Life Overhead", 1);
#endif

		while (CJen::ProcessEvent* ev = this->PeekEvent())
		{
			//StaticMeshEvent
			if (StaticMeshEvent* SME = dynamic_cast<StaticMeshEvent*>(ev))
			{
				this->HandleStaticMeshEvent(SME);
			}
			//ImageEvent
			else if(ImageEvent* IEV = dynamic_cast<ImageEvent*>(ev))
			{
				this->HandleImageEvent(IEV);
			}
			//LightEvent
			else if (PointLightEvent* PEV = dynamic_cast<PointLightEvent*>(ev))
			{
				this->HandlePointLightEvent(PEV);
			}
			else if (SpotLightEvent* SEV = dynamic_cast<SpotLightEvent*>(ev))
			{
				this->HandleSpotLightEvent(SEV);
			}
			//TerrainEvent
			else if(TerrainEvent* TEV = dynamic_cast<TerrainEvent*>(ev))
			{
				this->HandleTerrainEvent(TEV);
			}
			//SkyBoxEvent
			else if (SkyBoxEvent* SBE = dynamic_cast<SkyBoxEvent*>(ev))
			{
				this->HandleSkyBoxEvent(SBE);
			}

			delete ev;
		}
		this->zCamera->Update(this->zGameTimer.GetDeltaTime());

		if (this->zCamera->GetFollowTerrain())
		{
			CalculateCameraHeight();
		}

#ifdef PERFORMANCE_TEST
		this->zPerf.PostMeasure("Renderer - Life Overhead", 1);
#endif

		this->PreRender();

#ifdef PERFORMANCE_TEST
		this->zPerf.PreMeasure("Renderer - Render Scene", 1);
#endif
		this->Render();

#ifdef PERFORMANCE_TEST
		this->zPerf.PostMeasure("Renderer - Render Scene", 1);
#endif

		this->PostRender();

#ifdef PERFORMANCE_TEST
		this->zPerf.PostMeasure("Renderer - Entire Frame", 0);
#endif

		this->Dx_SwapChain->Present( 0, 0 );
	}
}

void DxManager::CalculateCameraHeight()
{
	Vector3 cameraPos = this->zCamera->GetPosition();

	Vector2 camPos = Vector2(cameraPos.x, cameraPos.z);

	for (auto it = this->zTerrains.cbegin(); it != this->zTerrains.cend(); it++)
	{
		if((*it)->IsInside(camPos))
		{
			cameraPos.y = (*it)->CalcHeightAt(cameraPos.x, cameraPos.z) + 1.0f;
			this->zCamera->SetPosition(cameraPos);
		}
	}
}

void DxManager::SetCamera( Camera* cam )
{
	this->zCamera = cam;
}

Camera* DxManager::GetCamera() const
{
	return this->zCamera;
}

void DxManager::SetMaxFPS( int maxFPS )
{
	this->zParams.MaxFPS = maxFPS;
}

void DxManager::SetMinFPS( int minFPS )
{
	this->zParams.MinFps = minFPS;
}

int DxManager::GetMinFPS() const
{
	return this->zParams.MinFps;
}

int DxManager::GetMaxFPS() const
{
	return this->zParams.MaxFPS;
}

void DxManager::CreateStaticMesh(StaticMesh* mesh)
{
	if (mesh)
	{
		std::string test = mesh->GetFilePath();

		//Per Strip data
		std::vector<MeshStrip*> strips = mesh->GetStrips();

		if (strips.size() > 0)
		{
			//Check if a previous mesh already has set data.
			if (strips[0]->GetRenderObject() == NULL)
			{
				for (auto it_strip = strips.begin(); it_strip != strips.end(); it_strip++)
				{
					MeshStrip* strip = (*it_strip);
					std::vector<VertexNormalMap> tmp = strip->GetVerts();
					BUFFER_INIT_DESC bufferDesc;
					bufferDesc.ElementSize = sizeof(VertexNormalMap);
					bufferDesc.InitData = &tmp[0];

					bufferDesc.NumElements = strip->GetNrOfVerts();

					bufferDesc.Type = VERTEX_BUFFER;
					bufferDesc.Usage = BUFFER_DEFAULT;

					int index = std::distance(strips.begin(), it_strip);
					std::stringstream ss;
					ss << mesh->GetFilePath() << "strip" << index << "vertices";

					std::string resourceNameVertices = ss.str();
					BufferResource* verts = GetResourceManager()->CreateBufferResource(resourceNameVertices.c_str(), bufferDesc);

					BufferResource* inds = NULL;
					if (strip->GetIndicies().size() > 0)
					{
						BUFFER_INIT_DESC bufferInds;
						bufferInds.ElementSize = sizeof(int);
						bufferInds.InitData = &strip->GetIndicies()[0];
						bufferInds.NumElements = strip->GetNrOfIndicies();
						bufferInds.Type = INDEX_BUFFER;
						bufferInds.Usage = BUFFER_DEFAULT;

						ss.clear();

						ss << mesh->GetFilePath() << "strip" << index << "indices";

						std::string resourceNameIndices = ss.str();
						inds = GetResourceManager()->CreateBufferResource(resourceNameIndices.c_str(), bufferInds);
					}
					TextureResource* texture = NULL;
					TextureResource* normalMap = NULL;
					std::string texturePath = strip->GetTexturePath();
					if (texturePath != "")
					{
						texture = GetResourceManager()->CreateTextureResourceFromFile(texturePath.c_str(), true);
						std::string ending = texturePath.substr(texturePath.length()-4);
						std::string first = texturePath.substr(0, texturePath.length()-4);
						std::string normalTexturePath = first + "_n" + ending;
						try
						{
							normalMap = GetResourceManager()->CreateTextureResourceFromFile(normalTexturePath.c_str(), true);
						}
						catch (GraphicalException e)
						{
							CJen::Debug("WARNING: Failed to load texture: " + normalTexturePath);
						}
					}
					Object3D* obj = new Object3D(verts, inds, texture, normalMap, mesh->GetTopology());
					strip->SetRenderObject(obj);
				}
			}
		}

		mesh->RecreateWorldMatrix();
	}

	StaticMeshEvent* re = new StaticMeshEvent(true, mesh);
	this->PutEvent(re);
}

void DxManager::CreateSkyBox( std::string texture )
{
	SkyBox* sb = new SkyBox(this->zCamera->GetPosition(), 10, 10);
	MeshStrip* strip = sb->GetStrip();

	std::vector<VertexNormalMap> vertTmp = strip->GetVerts();

	BUFFER_INIT_DESC BufferDesc;
	BufferDesc.ElementSize = sizeof(VertexNormalMap);
	BufferDesc.InitData = &vertTmp[0];
	BufferDesc.NumElements = strip->GetNrOfVerts();
	BufferDesc.Type = VERTEX_BUFFER;
	BufferDesc.Usage = BUFFER_DEFAULT;

	//Create the Buffer
	BufferResource* VertexBuffer = GetResourceManager()->CreateBufferResource("SkyBoxDefaultVertex", BufferDesc);

	std::vector<int> intTmp = strip->GetIndicies();
	BUFFER_INIT_DESC indicesBufferDesc;
	indicesBufferDesc.ElementSize = sizeof(int);
	indicesBufferDesc.InitData = &intTmp[0];
	indicesBufferDesc.NumElements = strip->GetNrOfIndicies();
	indicesBufferDesc.Type = INDEX_BUFFER;
	indicesBufferDesc.Usage = BUFFER_DEFAULT;

	BufferResource* IndexBuffer = GetResourceManager()->CreateBufferResource("SkyBoxDefaultIndex", indicesBufferDesc);

	TextureResource* tex = NULL;
	if (texture != "")
	{
		tex = GetResourceManager()->CreateCubeTextureResourceFromFile(texture.c_str());
	}
	Object3D* ro = new Object3D(VertexBuffer, IndexBuffer, tex, NULL, sb->GetTopology());
	strip->SetRenderObject(ro);

	SkyBoxEvent* sbe = new SkyBoxEvent(true, sb);
	this->PutEvent(sbe);
}

void DxManager::CreateImage(Image* image, string texture)
{
	TextureResource* tex = NULL;
	if(texture != "")
	{
		tex = GetResourceManager()->CreateTextureResourceFromFile(texture.c_str());
	}

	image->SetTexture(tex);

	ImageEvent* re = new ImageEvent(true, image);
	this->PutEvent(re);
}

void DxManager::CreateTerrain( Terrain* terrain )
{
	TerrainEvent* re = new TerrainEvent(true, terrain);
	this->PutEvent(re);
}

void DxManager::DeleteImage(Image* image)
{
	ImageEvent* re = new ImageEvent(false, image);
	this->PutEvent(re);
}

void DxManager::DeleteStaticMesh( StaticMesh* mesh )
{
	StaticMeshEvent* re = new StaticMeshEvent(false, mesh);
	this->PutEvent(re);
}

void DxManager::DeleteTerrain(Terrain* terrain)
{
	TerrainEvent* re = new TerrainEvent(false, terrain);
	this->PutEvent(re);
}

void DxManager::CreateText( Text* text, std::string font )
{
	TextureResource* tex = NULL;
	if(font != "")
	{
		tex = GetResourceManager()->CreateTextureResourceFromFile((font + ".png").c_str());
	}

	Font* newFont = text->GetFont();
	newFont->textureResource = tex;

	/* Font .txt structure:
	char in int
	chartex
	charwidth

	char in int
	chartex
	charwidth

	example:
	91
	55
	10

	92
	65
	8

	*/

	font += ".txt";
	std::ifstream file;
	file.open(font);
	while(!file.eof())
	{
		std::string line = "";

		std::getline(file, line);
		int character = atoi(line.c_str());

		std::getline(file, line);
		int texpos = atoi(line.c_str());

		std::getline(file, line);
		int width = atoi(line.c_str());
		width = width - texpos;

		newFont->charTexCoords[character] = texpos;
		newFont->charWidth[character] = width;
		std::getline(file, line);
	}
	file.close();

	TextEvent* te = new TextEvent(true, text);
	this->PutEvent(te);
}

void DxManager::DeleteText( Text* text )
{
	TextEvent* te = new TextEvent(false, text);
	this->PutEvent(te);
}

void DxManager::CreatePointLight( PointLight* pLight )
{
	PointLightEvent* le = new PointLightEvent(true, pLight);
	pLight->Init(this->Dx_Device, this->Dx_DeviceContext);
	this->PutEvent(le);
}

void DxManager::DeletePointLight( PointLight* pLight )
{
	PointLightEvent* le = new PointLightEvent(false, pLight);
	this->PutEvent(le);
}

void DxManager::CreateSpotLight( SpotLight* sLight )
{
	SpotLightEvent* le = new SpotLightEvent(true, sLight);
	sLight->Init(this->Dx_Device, this->Dx_DeviceContext);
	this->PutEvent(le);
}

void DxManager::DeleteSpotLight( SpotLight* sLight )
{
	SpotLightEvent* le = new SpotLightEvent(false, sLight);
	this->PutEvent(le);
}
