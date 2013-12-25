#include "GraphicsEngineImpl.h"
#include "DxManager_Events.h"

int GraphicsEngineParams::WindowWidth = 1024;
int GraphicsEngineParams::WindowHeight = 768;
bool GraphicsEngineParams::Maximized = false;
int GraphicsEngineParams::FOV = 75.0f;
float GraphicsEngineParams::NearClip = 0.2f;
float GraphicsEngineParams::FarClip = 200.0f;
int GraphicsEngineParams::RefreshRate = 60;
int GraphicsEngineParams::MaxFPS = 0;
int GraphicsEngineParams::MinFps = 0;
float GraphicsEngineParams::MouseSensativity = 1.0f;

GraphicsEngineImpl::GraphicsEngineImpl( const GraphicsEngineParams &params, HINSTANCE hInstance, int nCmdShow )
	: zParameters(params)
{
	this->zCamera = NULL;
	this->dx = NULL;
	this->hInstance = NULL;
	this->hWnd = NULL;
	this->zGameTimer = new GameTimer();
	this->zGameTimer->Init();

	this->zLoadingScreen = false;
	this->zRunning = true;
	this->zLoading = false;

	this->InitWindow(hInstance, nCmdShow);
	this->zKeyListener = new KeyListener(this->hWnd);

	this->InitObjects();

	this->Start();
}

GraphicsEngineImpl::~GraphicsEngineImpl()
{
	this->Close();
	this->WaitUntillDone();

	// Close DxManager thread.
	this->dx->Close();
	this->dx->WaitUntillDone();

	SAFE_DELETE(this->dx);
	SAFE_DELETE(this->zCamera);
	SAFE_DELETE(this->zGameTimer);
	SAFE_DELETE(this->zKeyListener);
}

iGraphicsEngineParams& GraphicsEngineImpl::GetEngineParameters()
{
	return this->GetEngineParams();
}

LRESULT CALLBACK GraphicsEngineImpl::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	GraphicsEngineImpl* gfx = NULL;
	PAINTSTRUCT ps;

	HDC hdc;

	if (message == WM_CREATE)
	{
		gfx = (GraphicsEngineImpl*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)gfx);
	}
	else
	{
		gfx = (GraphicsEngineImpl*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		if (gfx)
			gfx->GetKeyListener()->KeyDown(wParam);
		break;
	case WM_KEYUP:
		if (gfx)
			gfx->GetKeyListener()->KeyUp(wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		// Left Mouse Pressed
	case WM_LBUTTONDOWN:
		if (gfx)
			gfx->GetKeyListener()->MouseDown(1);
		break;

	case WM_LBUTTONUP:
		if (gfx)
			gfx->GetKeyListener()->MouseUp(1);
		break;

		// Right Mouse Pressed
	case WM_RBUTTONDOWN:
		if (gfx)
			gfx->GetKeyListener()->MouseDown(2);
		break;

	case WM_RBUTTONUP:
		if (gfx) gfx->GetKeyListener()->MouseUp(2);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

HRESULT GraphicsEngineImpl::InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	this->hInstance = hInstance;

	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style          = 0;//CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = this->WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = this->hInstance;
	wcex.hIcon          = LoadIcon(hInstance, "");
	//wcex.hCursor        = LoadCursor(hInstance, "cursor1.cur");
	//wcex.hCursor		= (HCURSOR)LoadImage(hInstance, "cursor1.cur", IMAGE_ICON, 32,32,LR_LOADFROMFILE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = "GraphicsEngine";
	wcex.hIconSm        = (HICON)LoadImage(hInstance, "", IMAGE_ICON, 32,32,LR_LOADFROMFILE);
	
	if( !RegisterClassEx(&wcex) )
		return E_FAIL;

	// Create window
	RECT rc = { 0, 0, this->zParameters.WindowWidth, this->zParameters.WindowHeight };
	if(this->zParameters.Maximized)
	{
		AdjustWindowRectEx(&rc, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP | WS_VISIBLE, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
		this->hWnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, "GraphicsEngine", 
			"GraphicsEngine - Direct3D 11.0", WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP | WS_VISIBLE, 
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, this->hInstance, this);

		// To make sure taskbar gets hidden.
		::SendMessage(this->hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		::SendMessage(this->hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	else
	{
		AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
		this->hWnd = CreateWindow("GraphicsEngine", "GraphicsEngine - Direct3D 11.0", 
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, this->hInstance, this);
	}
	if(!this->hWnd)
		return E_FAIL;

	ShowWindow(this->hWnd, nCmdShow);
	//MoveWindow(this->hWnd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, false);
	//DragAcceptFiles(this->hWnd, true);

	return S_OK;
}

void GraphicsEngineImpl::InitObjects()
{
	//FpsUtil* util = this->zGameTimer->GetFpsUtility();
	//util->SetFPSUpperLimit(this->zParameters.MaxFPS);
	//util->SetFPSLowerLimit(this->zParameters.MinFps);

	this->zCamera = new Camera(hWnd, this->zParameters);

	this->dx = new DxManager(this->hWnd, this->zParameters, this->zCamera);

	this->dx->Start();
	
	this->zLoadingScreenBG = NULL;
	this->zLoadingScreenPB = NULL;
	this->zLoadingScreenFade = NULL;
	this->zLoadingScreenPercentage = NULL;
	this->zLoadingScreenText = NULL;
	this->loadingScreenState = 0;
}

void GraphicsEngineImpl::Life()
{
	while(this->stayAlive)
	{
		if(this->GetEventQueueSize() == 0)
			this->zLoading = false;
		else
			this->zLoading = true;

		//Listen for events
		if (CJen::ProcessEvent* ev = this->WaitEvent())
		{
			if (LoadMeshEvent* LME = dynamic_cast<LoadMeshEvent*>(ev))
			{
				std::string filename = LME->GetFileName();
				if (StaticMesh* mesh = LME->GetStaticMesh())
				{
					bool success = mesh->LoadFromFile(filename);
					if (success)
					{
						this->dx->CreateStaticMesh(mesh);
					}
				}
			}

			delete ev;

			
		}
		Sleep(2);
	}
}

float GraphicsEngineImpl::Update()
{
	MSG msg = {0};
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if(msg.message == WM_QUIT)
			this->zRunning = false;
	}

	this->zGameTimer->Frame();

	std::stringstream ss;
	Vector3 position = this->zCamera->GetPosition();
	Vector3 forward = this->zCamera->GetForward();

	ss << "FPS: " << this->zGameTimer->GetFPS() 
		<< " (" << position.x << ", " << position.y << ", " << position.z
		<< ") (" << forward.x << ", " << forward.y << ", " << forward.z << ")";

	SetWindowText(this->hWnd, ss.str().c_str());

	return this->zGameTimer->GetDeltaTime();;
}

iMesh* GraphicsEngineImpl::CreateStaticMesh( const char* filename, const Vector3& pos )
{
	StaticMesh* mesh = new StaticMesh(pos, filename);

	LoadMeshEvent* lme = new LoadMeshEvent(filename, mesh);

	this->PutEvent(lme);

	return mesh;
}

void GraphicsEngineImpl::DeleteMesh( iMesh* delMesh )
{
	if (StaticMesh* mesh = dynamic_cast<StaticMesh*>(delMesh))
	{
		this->dx->DeleteStaticMesh(mesh);
	}
}

void GraphicsEngineImpl::ChangeSkyBox( const char* texture )
{
	this->dx->CreateSkyBox(std::string(texture));
}

iTerrain* GraphicsEngineImpl::CreateTerrain( const Vector3& pos, const Vector3& scale, const unsigned int& size, const char* heightMap )
{
	Terrain* terrain = new Terrain(pos, scale, size, heightMap);
	this->dx->CreateTerrain(terrain);

	return terrain;
}

void GraphicsEngineImpl::DeleteTerrain( iTerrain*& terrain )
{
	if (terrain != NULL)
	{
		this->dx->DeleteTerrain(dynamic_cast<Terrain*>(terrain));
		terrain = NULL;
	}
}

iText* GraphicsEngineImpl::CreateText( const char* text, Vector2 pos, float size, const char* fontTexturePath )
{
	return this->CreateText(std::string(text), D3DXVECTOR2(pos.x, pos.y), size, std::string(fontTexturePath));
}

Text* GraphicsEngineImpl::CreateText( std::string text, D3DXVECTOR2 position, float size, std::string fontTexturePath )
{
	Text* textobj = new Text(text, position, size);
	this->dx->CreateText(textobj, fontTexturePath);
	return textobj;
}

void GraphicsEngineImpl::DeleteText( iText* &deltxt )
{
	this->DeleteText(dynamic_cast<Text*>(deltxt));
	deltxt = NULL;
}

bool GraphicsEngineImpl::DeleteText( Text* delText )
{
	this->dx->DeleteText(delText);
	return true;
}

iImage* GraphicsEngineImpl::CreateImage( Vector2 pos, Vector2 dimensions, const char* texture )
{
	return this->CreateImage(D3DXVECTOR2(pos.x, pos.y), D3DXVECTOR2(dimensions.x, dimensions.y), std::string(texture));
}

Image* GraphicsEngineImpl::CreateImage( D3DXVECTOR2 position, D3DXVECTOR2 dimensions, std::string texture )
{
	Image* img = new Image(position, dimensions);
	this->dx->CreateImage(img, texture);
	return img;
}

void GraphicsEngineImpl::DeleteImage( iImage* &delImg )
{
	this->DeleteImage(dynamic_cast<Image*>(delImg));
	delImg = NULL;
}

bool GraphicsEngineImpl::DeleteImage( Image* delImage )
{
	this->dx->DeleteImage(delImage);
	return true;
}

void GraphicsEngineImpl::LoadingScreen( const char* BackgroundTexture /*= ""*/, const char* ProgressBarTexture /*= ""*/, float FadeBlackInInTime /*= 0.0f*/, float FadeBlackInOutTime /*= 0.0f*/, float FadeBlackOutInTime /*= 0.0f*/, float FadeBlackOutOutTime /*= 0.0f*/ )
{
	this->Update();
	std::string text = "";
	if (!this->zLoadingScreenBG)
	{
		if (strcmp(BackgroundTexture, "") != 0)
		{
			this->zLoadingScreenBG = this->CreateImage(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), BackgroundTexture);
			this->zLoadingScreenBG->SetStrata(10.0f);
		}
	}

	if (!this->zLoadingScreenPB)
	{
		if (strcmp(ProgressBarTexture, "") != 0)
		{
			this->zLoadingScreenPB = this->CreateImage(D3DXVECTOR2(((float)this->zParameters.WindowWidth / 4.0f), ((float)this->zParameters.WindowHeight * 3.0f) / 4.0f), 
				D3DXVECTOR2(0.0f, (float)this->zParameters.WindowHeight / 10.0f), ProgressBarTexture);
			this->zLoadingScreenPB->SetStrata(10.0f);
		}
	}

	if (!this->zLoadingScreenPercentage)
	{
		this->zLoadingScreenPercentage = this->CreateText("Hello", D3DXVECTOR2(((float)this->zParameters.WindowWidth / 2.0f), (float)this->zParameters.WindowHeight / 2.0f), 
			1.0f, "media/fonts/new");
		this->zLoadingScreenPercentage->SetStrata(10.0f);
	}

	int totalItems = this->GetEventQueueSize();

	if (totalItems == 0)
	{
		this->HideLoadingScreen();
		return;
	}
	float dx = (this->zParameters.WindowWidth * 0.5f) / totalItems;
	int y = this->zParameters.WindowHeight / 10.0f;

	if (!this->zLoadingScreenFade)
	{
		if(FadeBlackInInTime != 0.0f || FadeBlackInOutTime != 0.0f 
			|| FadeBlackOutInTime != 0.0f || FadeBlackOutOutTime != 0.0f)
		{
			this->zLoadingScreenFade = this->CreateImage(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2( (float)this->zParameters.WindowWidth, (float)this->zParameters.WindowHeight), "Media/LoadingScreen/FadeTexture.png");
			this->zLoadingScreenFade->SetStrata(10.0f);
		}
	}

	/*
	0 = fade in to black
	1 = fade out to loading screen
	2 = loading 
	3 = fade in to black
	4 = fade out to game
	*/
	this->StartRendering();
	float timer = 0.0f;
	bool loading = true;

	while (loading)
	{
		float deltaTime = this->Update();
		timer += deltaTime;

		if(this->loadingScreenState == 0)
		{
			if(FadeBlackInInTime > 0.0f)
			{
				float op = timer / FadeBlackInInTime;
				if(op > 1.0f)
					op = 1.0f;

				if(this->zLoadingScreenFade)
					this->zLoadingScreenFade->SetOpacity(op);
			}
			if(timer > FadeBlackInInTime)
			{
				this->loadingScreenState++;
				timer = 0;

				if ( this->zLoadingScreenBG ) 
					this->zLoadingScreenBG->SetDimensions(Vector2((float)this->zParameters.WindowWidth, (float)this->zParameters.WindowHeight));
			}
		}
		else if(this->loadingScreenState == 1)
		{
			if(FadeBlackInOutTime > 0.0f)
			{
				float op = 1 - (timer / FadeBlackInOutTime);
				if(op < 0.0f)
					op = 0.0f;
				if(this->zLoadingScreenFade)
					this->zLoadingScreenFade->SetOpacity(op);
			}

			if(timer > FadeBlackInOutTime)
				this->loadingScreenState++;
		}
		else if(this->loadingScreenState == 2)
		{
			if(!this->zLoading)
			{
				timer = 0;
				this->loadingScreenState++;
			}
		}
		else if(this->loadingScreenState == 3)
		{
			if(FadeBlackOutInTime > 0.0f)
			{
				float op = timer / FadeBlackOutInTime;
				if(op > 1.0f)
					op = 1.0f;
				if(this->zLoadingScreenFade)
					this->zLoadingScreenFade->SetOpacity(op);
			}

			if(timer > FadeBlackOutInTime)
			{
				this->loadingScreenState++;
				timer = 0;

				if(this->zLoadingScreenPB)
				{
					this->DeleteImage(this->zLoadingScreenPB);
					this->zLoadingScreenPB = NULL;
				}
				if(this->zLoadingScreenBG)
				{
					this->DeleteImage(this->zLoadingScreenBG);
					this->zLoadingScreenBG = NULL;
				}
			}
		}
		else
		{
			if(FadeBlackOutOutTime > 0.0f)
			{
				float op = 1 - (timer / FadeBlackOutOutTime);
				if(op < 0.0f)
					op = 0.0f;
				if(this->zLoadingScreenFade)
					this->zLoadingScreenFade->SetOpacity(op);
			}

			if(timer > FadeBlackOutOutTime)
			{
				this->loadingScreenState++;
				loading = false;
			}
		}

		int ItemsToGo = this->GetEventQueueSize();

		if(this->zLoading && this->loadingScreenState > 0)
		{
			if(this->zLoadingScreenPB)
			{
				if(ItemsToGo == 0)
					this->zLoadingScreenPB->SetDimensions(Vector2(dx * (totalItems - 0.5f), y));
				else if(ItemsToGo < totalItems - 1)
					this->zLoadingScreenPB->SetDimensions(Vector2(dx * (totalItems - ItemsToGo - 1), y));
				else
					this->zLoadingScreenPB->SetDimensions(Vector2(dx * 0.5f, y));
			}

			if (this->zLoadingScreenPercentage)
			{
				float perc = 1.0f - ((float)ItemsToGo / (float)totalItems);
				text = "";
				CJen::addNrToString(perc, text);
				//this->zLoadingScreenPercentage->SetText(text.c_str());
			}
		}

		// Sleep for a bit
		Sleep(15);
	}

	this->HideLoadingScreen();
}

void GraphicsEngineImpl::ShowLoadingScreen( const char* BackgroundTexture /*= ""*/, const char* ProgressBarTexture /*= ""*/, float FadeBlackInInTime /*= 0.0f*/, float FadeBlackInOutTime /*= 0.0f*/ )
{
	if(!this->zLoadingScreenBG)
	{
		if( strcmp(BackgroundTexture, "") != 0 )
		{
			this->zLoadingScreenBG = this->CreateImage(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), BackgroundTexture);
			this->zLoadingScreenBG->SetStrata(10.0f);
		}
	}

	if(!this->zLoadingScreenPB)
	{
		if( strcmp(ProgressBarTexture, "") != 0)
		{
			this->zLoadingScreenPB = this->CreateImage(D3DXVECTOR2((this->zParameters.WindowWidth / 4.0f), ((this->zParameters.WindowHeight * 3.0f) / 4.0f)), 
				D3DXVECTOR2(0, this->zParameters.WindowHeight / 10.0f), ProgressBarTexture);
			this->zLoadingScreenPB->SetStrata(10.0f);
		}
	}

	if(!this->zLoadingScreenFade)
	{
		if(FadeBlackInInTime != 0.0f || FadeBlackInOutTime != 0.0f)
		{
			this->zLoadingScreenFade = this->CreateImage(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2((float)this->zParameters.WindowWidth, (float)this->zParameters.WindowHeight), "Media/LoadingScreen/FadeTexture.png");
			this->zLoadingScreenFade->SetStrata(10.0f);
		}
	}

	if (!this->zLoadingScreenText)
	{
		this->zLoadingScreenText = this->CreateText("", D3DXVECTOR2((this->zParameters.WindowWidth / 4.0f), ((this->zParameters.WindowHeight * 3.0f) / 4.0f)), 1.0f, "media/fonts/new");
		this->zLoadingScreenText->SetStrata(10.0f);
	}

	this->StartRendering();
	float timer = 0.0f;
	bool loading = true;
	while(loading)
	{
		float diff = this->Update();
		timer += diff;

		if(this->loadingScreenState == 0)
		{
			if(FadeBlackInInTime > 0.0f)
			{
				float op = timer / FadeBlackInInTime;
				if(op > 1.0f)
					op = 1.0f;
				if(this->zLoadingScreenFade)
					this->zLoadingScreenFade->SetOpacity(op);
			}
			if(timer > FadeBlackInInTime)
			{
				this->loadingScreenState++;
				timer = 0;

				if ( this->zLoadingScreenBG ) 
					this->zLoadingScreenBG->SetDimensions(Vector2((float)this->zParameters.WindowWidth, (float)this->zParameters.WindowHeight));
			}
		}
		else if(this->loadingScreenState == 1)
		{
			if(FadeBlackInOutTime > 0.0f)
			{
				float op = 1 - (timer / FadeBlackInOutTime);
				if(op < 0.0f)
					op = 0.0f;
				if(this->zLoadingScreenFade)
					this->zLoadingScreenFade->SetOpacity(op);
			}

			if(timer > FadeBlackInOutTime)
				this->loadingScreenState++;
		}
		else
		{
			loading = false;
		}

		// Sleep for a bit
		Sleep(15);
	}
	this->zLoadingScreen = true;
}

void GraphicsEngineImpl::HideLoadingScreen()
{
	this->zLoadingScreen = false;
	this->loadingScreenState = 0;
	if(this->zLoadingScreenFade)
	{
		this->DeleteImage(this->zLoadingScreenFade);
		this->zLoadingScreenFade = NULL;
	}
	if(this->zLoadingScreenPB)
	{
		this->DeleteImage(this->zLoadingScreenPB);
		this->zLoadingScreenPB = NULL;
	}
	if(this->zLoadingScreenBG)
	{
		this->DeleteImage(this->zLoadingScreenBG);
		this->zLoadingScreenBG = NULL;
	}
	if (!this->zLoadingScreenText)
	{
		this->DeleteText(this->zLoadingScreenText);
		this->zLoadingScreenText = NULL;
	}
	if (!this->zLoadingScreenPercentage)
	{
		this->DeleteText(this->zLoadingScreenPercentage);
		this->zLoadingScreenPercentage = NULL;
	}
}

void GraphicsEngineImpl::ResetPerfLogging()
{
	this->dx->ResetPerfLogging();
}

void GraphicsEngineImpl::PrintPerfLogging()
{
	this->dx->PrintPerfLogging();
}

iPointLight* GraphicsEngineImpl::CreatePointLight( Vector3 pos, float radius, float intensity, bool enableShadows )
{
	return this->CreatePointLight(D3DXVECTOR3(pos.x, pos.y, pos.z), radius, intensity, enableShadows);
}

PointLight* GraphicsEngineImpl::CreatePointLight( D3DXVECTOR3 pos, float radius, float intensity, bool enableShadows )
{
	PointLight* light = new PointLight(pos, radius, intensity, enableShadows);
	this->dx->CreatePointLight(light);
	return light;
}

void GraphicsEngineImpl::DeletePointLight( iPointLight* &pLight )
{
	this->DeletePointLight(dynamic_cast<PointLight*>(pLight));
	pLight = NULL;
}

void GraphicsEngineImpl::DeletePointLight( PointLight* pLight )
{
	this->dx->DeletePointLight(pLight);
}