#include "DxManager.h"
#include "Exceptions.h"
#include "ResourceManager.h"

bool once = false;

void DxManager::PreRender()
{
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	static float ClearColor1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	static float ClearColor2[4] = {0.5f, 0.5f, 0.5f, 0.0f};
	static float ClearColor3[4] = {1.0f, 1.0f, 1.0f, 1.0f};

	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_RenderTargetView, ClearColor1);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GBufferRTV[COLOR], ClearColor1);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GBufferRTV[NORMAL], ClearColor2);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GBufferRTV[DEPTH], ClearColor3);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GBufferRTV[LIGHT], ClearColor1);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GBufferRTV[FINAL], ClearColor1);

	this->Dx_DeviceContext->RSSetViewports(1, &Dx_Viewport);
}

void DxManager::Render()
{
#ifdef PERFORMANCE_TEST
	this->zPerf.PreMeasure("Renderer - Render Deferred Geo", 2);
#endif

	this->RenderMeshes();

#ifdef PERFORMANCE_TEST
	this->zPerf.PostMeasure("Renderer - Render Deferred Geo", 2);
#endif

#ifdef PERFORMANCE_TEST
	this->zPerf.PreMeasure("Renderer - Render Deferred Geo Terrain", 2);
#endif

	this->RenderTerrain();
	this->RenderTessellatedTerrain();

#ifdef PERFORMANCE_TEST
	this->zPerf.PostMeasure("Renderer - Render Deferred Geo Terrain", 2);
#endif

#ifdef PERFORMANCE_TEST
	this->zPerf.PreMeasure("Renderer - Render Deferred SkyBox", 2);
#endif

	this->RenderDeferredSkybox();

#ifdef PERFORMANCE_TEST
	this->zPerf.PostMeasure("Renderer - Render Deferred SkyBox", 2);
#endif

#ifdef PERFORMANCE_TEST
	this->zPerf.PreMeasure("Renderer - Generating Shadows", 2);
#endif

 	if(false)
 		this->CreateShadowMapsSinglePass();
 	else
 		this->CreateShadowMapsMultiPass();

#ifdef PERFORMANCE_TEST
	this->zPerf.PostMeasure("Renderer - Generating Shadows", 2);
#endif

#ifdef PERFORMANCE_TEST
	this->zPerf.PreMeasure("Renderer - Render Lights", 2);
#endif

	this->RenderPointLights();

#ifdef PERFORMANCE_TEST
	this->zPerf.PostMeasure("Renderer - Render Lights", 2);
#endif


#ifdef PERFORMANCE_TEST
	this->zPerf.PreMeasure("Renderer - Render Combined Image", 2);
#endif

	this->RenderFinalImage();

#ifdef PERFORMANCE_TEST
	this->zPerf.PostMeasure("Renderer - Render Combined Image", 2);
#endif

#ifdef PERFORMANCE_TEST
	this->zPerf.PreMeasure("Renderer - Render Water Planes", 2);
#endif

	//this->RenderWaterPlanes();

#ifdef PERFORMANCE_TEST
	this->zPerf.PostMeasure("Renderer - Render Water Planes", 2);
#endif

#ifdef PERFORMANCE_TEST
	this->zPerf.PreMeasure("Renderer - Render Image", 2);
#endif

	this->RenderImages();

#ifdef PERFORMANCE_TEST
	this->zPerf.PostMeasure("Renderer - Render Image", 2);
#endif

#ifdef PERFORMANCE_TEST
	this->zPerf.PreMeasure("Renderer - Render Text", 2);
#endif

	this->RenderText();

#ifdef PERFORMANCE_TEST
	this->zPerf.PostMeasure("Renderer - Render Text", 2);
#endif

#ifdef PERFORMANCE_TEST
	this->zPerf.PreMeasure("Renderer - Render RenderTargets", 2);
#endif

	this->DebugRenderTargets();

#ifdef PERFORMANCE_TEST
	this->zPerf.PostMeasure("Renderer - Render RenderTargets", 2);
#endif
}

void DxManager::PostRender()
{
	ID3D11RenderTargetView* rtv[] = {0};
	this->Dx_DeviceContext->OMSetRenderTargets(1, rtv, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DxManager::RenderDeferredSkybox()
{
	if (!this->zSkyBox)
		return;

	ID3D11RenderTargetView* rtv[1] = {this->Dx_GBufferRTV[COLOR]};
	this->Dx_DeviceContext->OMSetRenderTargets(1, rtv, this->Dx_DepthStencilView);

	D3DXMATRIX world, worldViewProj, view, proj;
	float fogHeight = this->zCamera->GetPosition().y + this->zParams.FarClip * 0.1f;

	view = this->zCamera->GetViewMatrix();
	proj = this->zCamera->GetProjMatrix();
	world = this->zSkyBox->GetSkyboxWorldMatrix(this->zCamera->GetPosition(), this->zParams.FarClip);
	worldViewProj = world * view * proj;

	this->zShader_SkyBox->SetMatrix("mWorldViewProjection", worldViewProj);
	this->zShader_SkyBox->SetMatrix("mWorld", world);
	this->zShader_SkyBox->SetFloat("FogHeight", fogHeight);
	this->zShader_SkyBox->SetFloat("CameraY", this->zCamera->GetPosition().y);
	this->zShader_SkyBox->SetFloat3("FogColor", D3DXVECTOR3(0.6f, 0.6f, 0.6f));

	MeshStrip* strip = this->zSkyBox->GetStrip();
	Object3D* obj = strip->GetRenderObject();
	this->Dx_DeviceContext->IASetPrimitiveTopology(obj->GetTopology());

	obj->GetVertexBuffer()->GetBufferPointer()->Apply();
	obj->GetIndexBuffer()->GetBufferPointer()->Apply();

	if (obj->GetTextureResource() != NULL)
		this->zShader_SkyBox->SetResource("gSkyMap", obj->GetTextureResource()->GetSRVPointer());

	this->zShader_SkyBox->Apply(0);

	this->Dx_DeviceContext->DrawIndexed(strip->GetNrOfIndicies(), 0, 0);

	this->zShader_SkyBox->SetResource("gSkyMap", NULL);
	this->zShader_SkyBox->Apply(0);
}

void DxManager::RenderMeshes()
{
	ID3D11RenderTargetView* rtv[3] = {this->Dx_GBufferRTV[COLOR], this->Dx_GBufferRTV[NORMAL], this->Dx_GBufferRTV[DEPTH]};
	this->Dx_DeviceContext->OMSetRenderTargets(3, rtv, this->Dx_DepthStencilView);
	
	for (auto it_meshes = this->zObjects.begin(); it_meshes != this->zObjects.cend(); it_meshes++)
	{
		StaticMesh* staticMesh = (*it_meshes);

		D3DXMATRIX mWorld, mWorldInverseTranspose;

		mWorld = staticMesh->GetWorldMatrix();

		D3DXMatrixInverse(&mWorldInverseTranspose, 0, &mWorld);
		D3DXMatrixTranspose(&mWorldInverseTranspose, &mWorldInverseTranspose);

		this->Dx_DeviceContext->IASetPrimitiveTopology(staticMesh->GetTopology());

		std::vector<MeshStrip*> strips = staticMesh->GetStrips();

		for (auto it_strip = strips.begin(); it_strip != strips.end(); it_strip++)
		{
			bool indexBuffer = false;
			MeshStrip* strip = (*it_strip);

			Object3D* object = strip->GetRenderObject();

			bool bNormalMap = false;//(object->GetNormalMapResource() != NULL);

			this->zShader_DeferredGeo->SetMatrix("mView", this->zCamera->GetViewMatrix());
			this->zShader_DeferredGeo->SetMatrix("mProjection", this->zCamera->GetProjMatrix());
			this->zShader_DeferredGeo->SetBool("useNormalMap", bNormalMap);
			this->zShader_DeferredGeo->SetBool("useSpecMap", false);
			this->zShader_DeferredGeo->SetResource("tex2D", object->GetTextureResource()->GetSRVPointer());
			if (bNormalMap)
			{
				this->zShader_DeferredGeo->SetResource("normalMap", object->GetNormalMapResource()->GetSRVPointer());
			}
			this->zShader_DeferredGeo->SetMatrix("mWorld", mWorld);
			this->zShader_DeferredGeo->SetMatrix("mWorldInverseTranspose", mWorldInverseTranspose);

			BufferResource* indResource = object->GetIndexBuffer();
			BufferResource* vertResource = object->GetVertexBuffer();
			if (indResource)
			{
				indexBuffer = true;
				indResource->GetBufferPointer()->Apply();
			}
			if(vertResource)
			{
				vertResource->GetBufferPointer()->Apply();
			}

			this->zShader_DeferredGeo->Apply(0);

			if (indexBuffer)
			{
				this->Dx_DeviceContext->DrawIndexed(strip->GetNrOfIndicies(), 0, 0);
			}
			else
			{
				this->Dx_DeviceContext->Draw(strip->GetNrOfVerts(), 0);
			}
		}
	}
}

void DxManager::RenderTerrain()
{
	ID3D11RenderTargetView* rtv[3] = {this->Dx_GBufferRTV[COLOR], this->Dx_GBufferRTV[NORMAL], this->Dx_GBufferRTV[DEPTH]};
	this->Dx_DeviceContext->OMSetRenderTargets(3, rtv, this->Dx_DepthStencilView);

	D3DXMATRIX mWorld, mWorldInverseTranspose;

	for (auto it_Terrains = this->zTerrains.begin(); it_Terrains != this->zTerrains.cend(); it_Terrains++)
	{
		Terrain* terrain = (*it_Terrains);

		mWorld = terrain->GetWorldMatrix();

		D3DXMatrixInverse(&mWorldInverseTranspose, 0, &mWorld);
		D3DXMatrixTranspose(&mWorldInverseTranspose, &mWorldInverseTranspose);

		this->Dx_DeviceContext->IASetPrimitiveTopology(terrain->GetTopology());

		this->zShader_DeferredTerrainGeo->SetMatrix("mView", this->zCamera->GetViewMatrix());
		this->zShader_DeferredTerrainGeo->SetMatrix("mProjection", this->zCamera->GetProjMatrix());
		this->zShader_DeferredTerrainGeo->SetBool("useSpecMap", false);

		this->zShader_DeferredTerrainGeo->SetResource("tex2D", terrain->GetTextureResourcePtr()->GetSRVPointer());
		this->zShader_DeferredTerrainGeo->SetMatrix("mWorld", mWorld);
		this->zShader_DeferredTerrainGeo->SetMatrix("mWorldInverseTranspose", mWorldInverseTranspose);

		Buffer* indBuffer = terrain->GetIndexBufferPtr()->GetBufferPointer();
		Buffer* vertBuffer = terrain->GetVertexBufferPtr()->GetBufferPointer();

		vertBuffer->Apply();
		indBuffer->Apply();

		this->zShader_DeferredTerrainGeo->Apply(0);

		UINT32 numElements = indBuffer->GetElementCount();

		this->Dx_DeviceContext->DrawIndexed(numElements, 0, 0);
	}
}

void DxManager::RenderTessellatedTerrain()
{

}


void DxManager::CreateShadowMapsMultiPass()
{
	this->zShader_CubeShadowMap->SetTechnique(1);
	for(auto it_pointLights = this->zPointLights.begin(); it_pointLights != this->zPointLights.end(); it_pointLights++)
	{
		PointLight* pLight = (*it_pointLights);
		if(pLight->IsShadowsEnabled())
		{
			for(int i = 0; i < 6; i++)
			{
				ID3D11RenderTargetView* currentRenderTarget = pLight->GetRTVArray(i);
				ID3D11RenderTargetView* renderTargets[1] = {currentRenderTarget};
				this->Dx_DeviceContext->OMSetRenderTargets(1, renderTargets, pLight->GetDSVArray());

				if(currentRenderTarget)
				{
					float zMax = 1.e4f;
					float clearcolor[4] = { zMax, 1.0f, 1.0f, 1.0f };
					this->Dx_DeviceContext->ClearRenderTargetView(currentRenderTarget, clearcolor);
				}
				this->Dx_DeviceContext->ClearDepthStencilView(pLight->GetDSVArray(), D3D11_CLEAR_DEPTH, 1.0f, 0);

				D3DXMATRIX view = pLight->GetViewMatrix(i);

				this->zShader_CubeShadowMap->SetMatrix("mView", view);
				this->zShader_CubeShadowMap->SetMatrix("mProj", pLight->GetProjMatrix());
				this->zShader_CubeShadowMap->SetFloat3("gLightPos", pLight->GetPositionD3D());

				for(auto it_mesh = this->zObjects.begin(); it_mesh != this->zObjects.end(); it_mesh++)
				{
					StaticMesh* staticMesh = (*it_mesh);

					this->zShader_CubeShadowMap->SetMatrix("mWorld", staticMesh->GetWorldMatrix());

					this->Dx_DeviceContext->IASetPrimitiveTopology(staticMesh->GetTopology());

					std::vector<MeshStrip*> strips = staticMesh->GetStrips();

					for (auto it_strip = strips.begin(); it_strip != strips.end(); it_strip++)
					{
						bool indexBuffer = false;
						MeshStrip* strip = (*it_strip);

						Object3D* object = strip->GetRenderObject();

						BufferResource* indResource = object->GetIndexBuffer();
						BufferResource* vertResource = object->GetVertexBuffer();
						if (indResource)
						{
							indexBuffer = true;
							indResource->GetBufferPointer()->Apply();
						}
						if(vertResource)
						{
							vertResource->GetBufferPointer()->Apply();
						}

						this->zShader_CubeShadowMap->Apply(0);

						if (indexBuffer)
						{
							this->Dx_DeviceContext->DrawIndexed(strip->GetNrOfIndicies(), 0, 0);
						}
						else
						{
							this->Dx_DeviceContext->Draw(strip->GetNrOfVerts(), 0);
						}
					}
				}

				/*for (auto it_Terrains = this->zTerrains.begin(); it_Terrains != this->zTerrains.cend(); it_Terrains++)
				{
					Terrain* terrain = (*it_Terrains);

					this->Dx_DeviceContext->IASetPrimitiveTopology(terrain->GetTopology());

					this->zShader_CubeShadowMap->SetMatrix("mWorld", terrain->GetWorldMatrix());

					Buffer* indBuffer = terrain->GetIndexBufferPtr()->GetBufferPointer();
					Buffer* vertBuffer = terrain->GetVertexBufferPtr()->GetBufferPointer();

					vertBuffer->Apply();
					indBuffer->Apply();

					this->zShader_CubeShadowMap->Apply(0);

					UINT32 numElements = indBuffer->GetElementCount();

					this->Dx_DeviceContext->DrawIndexed(numElements, 0, 0);
				}*/
			}
			if(pLight->GetSRV())
			{
				this->Dx_DeviceContext->GenerateMips(pLight->GetSRV());
			}
		}	
	}

	for(auto it_SpotLights = this->zSpotLights.begin(); it_SpotLights != this->zSpotLights.end(); it_SpotLights++)
	{

	}
}

void DxManager::CreateShadowMapsSinglePass()
{
	this->zShader_CubeShadowMap->SetTechnique(0);
	for(auto it_pointLights = this->zPointLights.begin(); it_pointLights != this->zPointLights.end(); it_pointLights++)
	{
		PointLight* pLight = (*it_pointLights);
		if(pLight->IsShadowsEnabled())
		{
			ID3D11RenderTargetView* currentRenderTarget = pLight->GetRTV();
			ID3D11RenderTargetView* renderTargets[1] = {currentRenderTarget};
			this->Dx_DeviceContext->OMSetRenderTargets(1, renderTargets, pLight->GetDSV());

			if(currentRenderTarget)
			{
				float zMax = 1.e4f;
				float clearcolor[4] = { zMax, 1.0f, 1.0f, 1.0f };
				this->Dx_DeviceContext->ClearRenderTargetView(currentRenderTarget, clearcolor);
			}
			this->Dx_DeviceContext->ClearDepthStencilView(pLight->GetDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);

			D3DXMATRIX views[6];
			for (int n = 0; n < 6; n++)
			{
				views[n] = pLight->GetViewMatrix(n);
			}

			this->zShader_CubeShadowMap->SetMatrix("mLightView", views);
			this->zShader_CubeShadowMap->SetMatrix("mProj", pLight->GetProjMatrix());
			this->zShader_CubeShadowMap->SetFloat3("gLightPos", pLight->GetPositionD3D());

			for(auto it_mesh = this->zObjects.begin(); it_mesh != this->zObjects.end(); it_mesh++)
			{
				StaticMesh* staticMesh = (*it_mesh);

				this->zShader_CubeShadowMap->SetMatrix("mWorld", staticMesh->GetWorldMatrix());

				std::vector<MeshStrip*> strips = staticMesh->GetStrips();

				for (auto it_strip = strips.begin(); it_strip != strips.end(); it_strip++)
				{
					bool indexBuffer = false;
					MeshStrip* strip = (*it_strip);

					Object3D* object = strip->GetRenderObject();

					BufferResource* indResource = object->GetIndexBuffer();
					BufferResource* vertResource = object->GetVertexBuffer();
					if (indResource)
					{
						indexBuffer = true;
						indResource->GetBufferPointer()->Apply();
					}
					if(vertResource)
					{
						vertResource->GetBufferPointer()->Apply();
					}

					this->zShader_CubeShadowMap->Apply(0);

					if (indexBuffer)
					{
						this->Dx_DeviceContext->DrawIndexed(strip->GetNrOfIndicies(), 0, 0);
					}
					else
					{
						this->Dx_DeviceContext->Draw(strip->GetNrOfVerts(), 0);
					}
				}
			}

			/*for (auto it_Terrains = this->zTerrains.begin(); it_Terrains != this->zTerrains.cend(); it_Terrains++)
			{
			Terrain* terrain = (*it_Terrains);

			this->Dx_DeviceContext->IASetPrimitiveTopology(terrain->GetTopology());

			this->zShader_CubeShadowMap->SetMatrix("mWorld", terrain->GetWorldMatrix());

			Buffer* indBuffer = terrain->GetIndexBufferPtr()->GetBufferPointer();
			Buffer* vertBuffer = terrain->GetVertexBufferPtr()->GetBufferPointer();

			vertBuffer->Apply();
			indBuffer->Apply();

			this->zShader_CubeShadowMap->Apply(0);

			UINT32 numElements = indBuffer->GetElementCount();

			this->Dx_DeviceContext->DrawIndexed(numElements, 0, 0);
			}*/

			if(pLight->GetSRV())
			{
				this->Dx_DeviceContext->GenerateMips(pLight->GetSRV());
			}
		}	
	}
}

void DxManager::RenderPointLights()
{
	ID3D11RenderTargetView* rtv[1] = {this->Dx_GBufferRTV[LIGHT]};
	this->Dx_DeviceContext->OMSetRenderTargets(1, rtv, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);

	this->zShader_PointLight->SetResource("tColorMap", this->Dx_GBufferSRV[COLOR]);
	this->zShader_PointLight->SetResource("tNormalMap", this->Dx_GBufferSRV[NORMAL]);
	this->zShader_PointLight->SetResource("tDepthMap", this->Dx_GBufferSRV[DEPTH]);
	D3DXVECTOR3 camPos = this->zCamera->GetPositionD3D();

	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXMATRIX view = this->zCamera->GetViewMatrix();
	D3DXMATRIX proj = this->zCamera->GetProjMatrix();
	D3DXMATRIX viewProj = this->zCamera->GetViewProjMatrix();

	D3DXMATRIX invViewProj;
	D3DXMatrixInverse(&invViewProj, 0, &viewProj);

	this->zShader_PointLight->SetMatrix("mView", view);
	this->zShader_PointLight->SetMatrix("mProj", proj);
	this->zShader_PointLight->SetMatrix("mIVP", invViewProj);

	for (auto it_PointLight = this->zPointLights.cbegin(); it_PointLight != this->zPointLights.cend(); it_PointLight++)
	{
		PointLight* light = (*it_PointLight);

		ID3D11ShaderResourceView* cubeSrv = light->GetSRV();
		this->zShader_PointLight->SetResource("cShadowCubeMap", cubeSrv);

		D3DXVECTOR3 lightPos = light->GetPositionD3D();

		this->zShader_PointLight->SetBool("castShadows", light->IsShadowsEnabled());
		this->zShader_PointLight->SetFloat3("gCameraPos", camPos);
		this->zShader_PointLight->SetFloat3("gLightPos", lightPos);
		this->zShader_PointLight->SetFloat3("gColor", light->GetColor());
		this->zShader_PointLight->SetFloat("gLightRadius", light->GetRadius());
		this->zShader_PointLight->SetFloat("gLightIntensity", light->GetIntensity());

		this->zShader_PointLight->SetMatrix("mWorld", light->GetWorldMatrix());

		Buffer* vertBuffer = light->GetVertexBuffer()->GetBufferPointer();
		Buffer* indBuffer = light->GetIndexBuffer()->GetBufferPointer();

		vertBuffer->Apply();
		indBuffer->Apply();

		//Check if inside Sphere
		//float camToCenter = D3DXVec3Length(&(camPos - lightPos));

		//if(camToCenter <= light->GetRadius())
		//{
		//	//this->zShader_PointLight->SetTechnique(0);
		//}
		//else
		//{
		//	//this->zShader_PointLight->SetTechnique(1);
		//}

#ifdef PERFORMANCE_TEST
		this->zPerf.PreMeasure("Renderer - Render Lights Pass 1", 3);
#endif
		this->zShader_PointLight->Apply(3);

		this->Dx_DeviceContext->DrawIndexed(indBuffer->GetElementCount(), 0, 0);

#ifdef PERFORMANCE_TEST
		this->zPerf.PostMeasure("Renderer - Render Lights Pass 1", 3);
#endif

#ifdef PERFORMANCE_TEST
		this->zPerf.PreMeasure("Renderer - Render Lights Pass 2", 3);
#endif
		this->zShader_PointLight->Apply(1);

		this->Dx_DeviceContext->DrawIndexed(indBuffer->GetElementCount(), 0, 0);

#ifdef PERFORMANCE_TEST
		this->zPerf.PostMeasure("Renderer - Render Lights Pass 2", 3);
#endif

		//D3DXVECTOR3 dist = light->GetPositionD3D() - this->zCamera->GetPositionD3D();
		//float distToCam = D3DXVec3Length(&(dist));
		//if(distToCam <= light->GetRadius())
		//{
		//	this->zShader_PointLight->Apply(2);
		//
		//	this->Dx_DeviceContext->DrawIndexed(indBuffer->GetElementCount(), 0, 0);
		//}
		//else
		//{
		//	this->zShader_PointLight->Apply(3);
		//
		//	this->Dx_DeviceContext->DrawIndexed(indBuffer->GetElementCount(), 0, 0);
		//}
		
		ID3D11ShaderResourceView* pSRV = NULL;

		this->zShader_PointLight->SetResource("cShadowCubeMap", pSRV);
		this->zShader_PointLight->SetResource("tColorMap", pSRV);
		this->zShader_PointLight->SetResource("tNormalMap", pSRV);
		this->zShader_PointLight->SetResource("tDepthMap", pSRV);

		this->zShader_PointLight->Apply(1);
	}

	
}

void DxManager::RenderSpotLights()
{
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_GBufferRTV[LIGHT], this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);

	this->zShader_PointLight->SetResource("tColorMap", this->Dx_GBufferSRV[COLOR]);
	this->zShader_PointLight->SetResource("tNormalMap", this->Dx_GBufferSRV[NORMAL]);
	this->zShader_PointLight->SetResource("tDepthMap", this->Dx_GBufferSRV[DEPTH]);
	D3DXVECTOR3 camPos = this->zCamera->GetPositionD3D();

	for(auto it_SpotLights = this->zSpotLights.begin(); it_SpotLights != this->zSpotLights.end(); it_SpotLights++)
	{

	}
}


void DxManager::RenderWaterPlanes()
{
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);

	this->zShader_Water->SetResource("tNormalMap", this->Dx_GBufferSRV[NORMAL]);
	this->zShader_Water->SetResource("tDepthMap", this->Dx_GBufferSRV[DEPTH]);

	this->zShader_Water->SetResource("tFinalScene", this->Dx_GBufferSRV[FINAL]);

	this->zShader_Water->SetFloat3("gCameraPos", this->zCamera->GetPositionD3D());

	D3DXMATRIX mInverseViewProj;
	D3DXMatrixInverse(&mInverseViewProj, 0, &this->zCamera->GetViewProjMatrix());
	this->zShader_Water->SetMatrix("mInverseViewProj", mInverseViewProj);

	for(auto it_Plane = this->zWaterPlanes.begin(); it_Plane != this->zWaterPlanes.end(); it_Plane++)
	{
		

		this->zShader_Water->Apply(0);

		this->zQuadRenderer->Draw();
	}
}


void DxManager::RenderFinalImage()
{
	//this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_GBufferRTV[FINAL], this->Dx_DepthStencilView);
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);

	this->zShader_Final->SetResource("tColorMap", this->Dx_GBufferSRV[COLOR]);
	this->zShader_Final->SetResource("tLightMap", this->Dx_GBufferSRV[LIGHT]);
	this->zShader_Final->SetResource("tDepthMap", this->Dx_GBufferSRV[DEPTH]);

	this->zShader_Final->Apply(0);

	this->zQuadRenderer->Draw();

	this->zShader_Final->SetResource("tColorMap", NULL);
	this->zShader_Final->SetResource("tLightMap", NULL);
	this->zShader_Final->SetResource("tDepthMap", NULL);

	this->zShader_Final->Apply(0);
}

void DxManager::RenderImages()
{
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	for (auto it_image = this->zImages.cbegin(); it_image != this->zImages.cend(); it_image++)
	{
		this->zShader_Image->SetFloat("posx", ((*it_image)->GetPosition().x / this->zParams.WindowWidth) * 2 - 1);
		this->zShader_Image->SetFloat("posy", 2 - ((*it_image)->GetPosition().y / this->zParams.WindowHeight) * 2 - 1);
		this->zShader_Image->SetFloat("dimx", ((*it_image)->GetDimensions().x / this->zParams.WindowWidth) * 2);
		this->zShader_Image->SetFloat("dimy", -((*it_image)->GetDimensions().y / this->zParams.WindowHeight) * 2);
		this->zShader_Image->SetFloat("opacity", (*it_image)->GetOpacity());
		this->zShader_Image->SetFloat("strata", (*it_image)->GetStrata01());

		if((*it_image)->GetTexture() != NULL)
		{
			this->zShader_Image->SetResource("tex2D", (*it_image)->GetTexture()->GetSRVPointer());
		}
		this->zShader_Image->Apply(0);
		this->Dx_DeviceContext->Draw(1, 0);
	}
	this->zShader_Image->SetResource("tex2D", NULL);
	this->zShader_Image->Apply(0);
}

void DxManager::RenderText()
{
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	for (auto it_Text = this->zTexts.cbegin(); it_Text != this->zTexts.cend(); it_Text++)
	{
		// if Convert from screen space is needed, which it is
		this->zShader_Text->SetFloat("posx", ((*it_Text)->GetPosition().x / this->zParams.WindowWidth) * 2 - 1);
		this->zShader_Text->SetFloat("posy", 2 - ((*it_Text)->GetPosition().y / this->zParams.WindowHeight) * 2 - 1);
		this->zShader_Text->SetFloat("strata", (*it_Text)->GetStrata01());

		this->zShader_Text->SetFloat("size", (*it_Text)->GetSize());
		this->zShader_Text->SetFloat("windowWidth", (float)this->zParams.WindowWidth);
		this->zShader_Text->SetFloat("windowHeight", (float)this->zParams.WindowHeight);

		// Im only using ASCI 30 - 100, to reduce data sent I only send those 70 as 0-70. Therefor the t = 30 and t - 30
		static bool once = true;
		if(once)
		{
			for(int t = 30; t < 100; t++)
			{
				this->zShader_Text->SetFloatAtIndex(t - 30, "charTex", (float)(int)(*it_Text)->GetFont()->charTexCoords[t]);
				this->zShader_Text->SetFloatAtIndex(t - 30, "charWidth", (float)(int)(*it_Text)->GetFont()->charWidth[t]);
			}
			once = false;
		}

		this->zShader_Text->SetResource("tex2D", (*it_Text)->GetFont()->textureResource->GetSRVPointer());

		std::string drawText = (*it_Text)->GetText();

		//
		if(drawText.size() > 40)
			drawText = drawText.substr(0, 40);

		this->zShader_Text->SetFloat3("overlayColor", (*it_Text)->GetColor());
		this->zShader_Text->SetFloat("NrOfChars", (float)drawText.size());
		for(int t = 0; t < (int)drawText.size(); t++)
		{
			// Im only using ASCI 30 - 100, to reduce data sent I only send those 70 as 0-70. Therefor the -30
			this->zShader_Text->SetFloatAtIndex(t, "text", (float)(int)drawText[t] - 30);
		}

		this->zShader_Text->Apply(0);

		this->Dx_DeviceContext->Draw(1, 0);
	}
	this->zShader_Text->SetResource("tex2D", NULL);
	this->zShader_Text->Apply(0);
}

void DxManager::DebugRenderTargets()
{
	ID3D11RenderTargetView* rtv[1] = {this->Dx_RenderTargetView};
	this->Dx_DeviceContext->OMSetRenderTargets(1, rtv, this->Dx_DepthStencilView);

	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->zShader_SRV_Debug->SetBool("bGrayScale", false);

	for (int i = 0; i < MAX_RENDER_TARGET_DEBUG; i++)
	{
		this->zShader_SRV_Debug->SetResource("tTextureMap", this->Dx_GBufferSRV[i]);
		UINT stride = sizeof(VertexTex);
		UINT offset = 0;
		this->Dx_DeviceContext->IASetVertexBuffers(0, 1, &this->zVertexBuffer[i], &stride, &offset);

		this->zShader_SRV_Debug->Apply(0);

		this->Dx_DeviceContext->Draw(6, 0);

		this->zShader_SRV_Debug->SetResource("tTextureMap", NULL);
		this->zShader_SRV_Debug->Apply(0);
	}

	bool debugRenderTargets = true;

	if(debugRenderTargets)
	{
		if(this->zPointLights.size() > 0)
		{
			ID3D11ShaderResourceView* shadowCubeMap = this->zPointLights.at(0)->GetSRV();

			this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			this->zShader_DebugCubeMapSRV->SetResource("gCube", shadowCubeMap);
			this->zShader_DebugCubeMapSRV->SetBool("bGray", true);
			for(int i = 0; i < 6; i++)
			{
				UINT stride = sizeof(VertexTex3);
				UINT offset = 0;

				this->Dx_DeviceContext->IASetVertexBuffers(0, 1, &this->zCubeMapVertexBuffer[i], &stride, &offset);

				this->zShader_DebugCubeMapSRV->Apply(0);

				this->Dx_DeviceContext->Draw(6, 0);
			}
			this->zShader_DebugCubeMapSRV->SetResource("gCube", NULL);
			this->zShader_DebugCubeMapSRV->Apply(0);
		}
	}

}