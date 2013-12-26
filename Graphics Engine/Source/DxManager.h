#pragma once

#define PERFORMANCE_TEST

#include "Camera.h"
#include "Vertex.h"
#include "StaticMesh.h"
#include "Process.h"
#include "Shader.h"
#include "Image.h"
#include "Text.h"
#include "GameTimer.h"
#include "Object3D.h"
#include "SkyBox.h"
#include "QuadRenderer.h"
#include "PointLight.h"

#ifdef PERFORMANCE_TEST
#include "MaloWPerformance.h"
#endif

class TerrainEvent;
class SkyBoxEvent;
class StaticMeshEvent;
class ImageEvent;
class TextEvent;
class LightEvent;

#if defined(DEBUG) || defined(_DEBUG)
	#include <vld.h>
#endif

#define NUMBER_OF_VIEWS 5
#define MAX_RENDER_TARGET_DEBUG 4

enum RenderTargets
{
	COLOR,
	NORMAL,
	DEPTH,
	LIGHT,
	FINAL
};

class DxManager : public CJen::Process
{
private:
	//Timer
	GameTimer zGameTimer;

	//Shaders
	Shader* zShader_DeferredGeo;
	Shader* zShader_DeferredTerrainGeo;
	Shader* zShader_SkyBox;
	Shader* zShader_Final;
	Shader* zShader_SRV_Debug;
	Shader* zShader_Image;
	Shader* zShader_Text;
	Shader* zShader_PointLight;
	Shader* zShader_CubeShadowMap;
	Shader* zShader_ShadowMap;
	Shader* zShader_DebugCubeMapSRV;

	std::vector<Terrain*> zTerrains;
	std::vector<Image*> zImages;
	std::vector<StaticMesh*> zObjects;
	std::vector<Text*> zTexts;
	std::vector<PointLight*> zPointLights;

	D3DXVECTOR2 zHalfPixel;

	// Standard stuff
	ID3D11Device* Dx_Device;
	IDXGISwapChain* Dx_SwapChain;
	ID3D11Texture2D* Dx_DepthStencil;
	ID3D11DeviceContext* Dx_DeviceContext;
	ID3D11DepthStencilView* Dx_DepthStencilView;
	ID3D11RenderTargetView* Dx_RenderTargetView;

	/*! Color, normal, depth, light, combined*/
	ID3D11RenderTargetView* Dx_GBufferRTV[NUMBER_OF_VIEWS];
	ID3D11ShaderResourceView* Dx_GBufferSRV[NUMBER_OF_VIEWS];

	HWND hWnd;
	D3D11_VIEWPORT Dx_Viewport;

	GraphicsEngineParams &zParams;
	Camera* zCamera;

	SkyBox* zSkyBox;
	QuadRenderer* zQuadRenderer;

	bool zStartRender;
	
	//Debugging Render Target
	ID3D11Buffer* zVertexBuffer[MAX_RENDER_TARGET_DEBUG];
	ID3D11Buffer* zCubeMapVertexBuffer[6];

#ifdef PERFORMANCE_TEST
	MaloWPerformance zPerf;
#endif

private:
	HRESULT Init();
	HRESULT InitRenderTargets(unsigned int target, DXGI_FORMAT format);
	HRESULT InitShaders();

	HRESULT InitVertexBufferDebugging();

	/*! Clears the scene(rendertargets & viewports)*/
	void PreRender();
	/*! */
	void Render();
	/*! Render Stuff to Directly to the Backbuffer.*/
	void PostRender();

	/*! Renders Skybox.*/
	void RenderDeferredSkybox();
	/*! Render Static Meshes.*/
	void RenderMeshes();
	/*! Render Images.*/
	void RenderImages();
	/*! Render Text.*/
	void RenderText();
	/*! Render Scene to Backbuffer.*/
	void RenderFinalImage();
	/*! Render Terrain.*/
	void RenderTerrain();
	/*! Render Lights.*/
	void RenderLights();
	/*! Create ShadowMaps.*/
	void CreateShadowMapsMultiPass();
	void CreateShadowMapsSinglePass();
	/*! Render RenderTargets.*/
	void DebugRenderTargets();

	/*! Event Handling functions.*/
	void HandleStaticMeshEvent(StaticMeshEvent* sme);
	void HandleTerrainEvent(TerrainEvent* te);
	void HandleSkyBoxEvent(SkyBoxEvent* sbe);
	void HandleImageEvent(ImageEvent* iev);
	void HandleTextEvent(TextEvent* te);
	void HandlePointLightEvent(LightEvent* lev);

	void CalculateCameraHeight();
public:
	DxManager(HWND g_hWnd, GraphicsEngineParams params, Camera* cam);
	virtual ~DxManager();

	void StartRendering();

	virtual void Life();

	void SetCamera(Camera* cam);
	Camera* GetCamera() const;

	void SetMaxFPS(int maxFPS);
	void SetMinFPS(int minFPS );
	int GetMinFPS() const;
	int GetMaxFPS() const;

	void CreateTerrain(Terrain* terrain); 
	void CreateStaticMesh(StaticMesh* mesh);
	void CreateSkyBox(std::string texture);
	void CreateImage(Image* img, std::string texture);
	void CreateText(Text* text, string font);
	void CreatePointLight(PointLight* pLight);

	void DeleteStaticMesh(StaticMesh* mesh);
	void DeleteTerrain(Terrain* terrain);
	void DeleteImage(Image* img);
	void DeleteText(Text* text);
	void DeletePointLight(PointLight* pLight);

	void ResetPerfLogging();
	void PrintPerfLogging();
};