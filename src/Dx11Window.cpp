#include "Dx11Window.h"
#include "Log.h"
#include "Utils.h"

#include <assert.h>

namespace
{
	DX11Window* gdx11Wndow = 0;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return gdx11Wndow->mainWndProc(hwnd, msg, wParam, lParam);
}

DX11Window::DX11Window() : 
    mhAppInst(NULL),
	mWindowCaption(L"D3D11 Application"),
	md3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	mWindowWidth(800),
	mWindowHeight(600),
    mWindowRefreshRate(60),
	mEnable4xMsaa(false),
	mhMainWnd(NULL),
	mIsPaused(false),
	mIsMinimized(false),
	mIsMaximized(false),
	mIsResizing(false),
	m4xMsaaQuality(0),
	md3dDevice(NULL),
	md3dImmediateContext(NULL),
	mSwapChain(NULL),
	mDepthStencilBuffer(NULL),
	mRenderTargetView(NULL),
	mDepthStencilView(NULL),
    mInputHandler(NULL)
{
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
	gdx11Wndow = this;
}

DX11Window::~DX11Window()
{

}

bool DX11Window::init( LPCWSTR windowCaption, unsigned int windowWidth, unsigned int windowHeight, HINSTANCE hInstance, int nCmdShow, WMInputHandler* inputHandler )
{
    mhAppInst = hInstance;
    mWindowCaption = windowCaption;

    mWindowWidth = windowWidth;
    mWindowHeight = windowHeight;

    assert( inputHandler );

    mInputHandler = inputHandler;

    if(!initWindow()){
        LOG_ERRO << "Unable to initialize DX11 Window" << LOG_ENDL;
		return false;
    }
        
	if(!initDirect3D()){
        LOG_ERRO << "Unable to initialize Direct3D 11" << LOG_ENDL;
		return false;
    }

    if(!initInputDevices()){
        LOG_ERRO << "Unable to intialize Input Devices" << LOG_ENDL;
        return false;
    }

	return true;
}

void DX11Window::clearBDS()
{
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, clearColor);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}
            
void DX11Window::clear( )
{
    ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDepthStencilBuffer);

	if( md3dImmediateContext )
		md3dImmediateContext->ClearState();

	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(md3dDevice);

    LOG_INFO << "Direct3D 11 device and Window Released." << LOG_ENDL;
}

bool DX11Window::initWindow()
{
    WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = mhAppInst;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"DX11WindowClassName";

	if( !RegisterClass(&wc) ){
		LOG_ERRO << "RegisterClass for Window Failed." << LOG_ENDL;
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mWindowWidth, mWindowHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"DX11WindowClassName", mWindowCaption.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0); 

	if( !mhMainWnd ){
        LOG_ERRO << "CreateWindow Failed." << LOG_ENDL;
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

    LOG_INFO << "Window Successfully Initialized." << LOG_ENDL;
    return true;
}

bool DX11Window::initDirect3D()
{
    // Create the device and device context.
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
			0,                 // default adapter
			md3dDriverType,
			0,                 // no software device
			createDeviceFlags, 
			0, 0,              // default feature level array
			D3D11_SDK_VERSION,
			&md3dDevice,
			&featureLevel,
			&md3dImmediateContext);

	if( FAILED(hr) ){
		LOG_ERRO << "D3D11CreateDevice Failed." << LOG_ENDL;
		return false;
	}

	if( featureLevel != D3D_FEATURE_LEVEL_11_0 ){
		LOG_ERRO << "Direct3D Feature Level 11 unsupported by this system." << LOG_ENDL;
		return false;
	}

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width  = mWindowWidth;
	sd.BufferDesc.Height = mWindowHeight;
	sd.BufferDesc.RefreshRate.Numerator = mWindowRefreshRate;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    //Disable multisampling for now
	sd.SampleDesc.Count   = 1; 
	sd.SampleDesc.Quality = 0;

	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = mhMainWnd;
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;

	IDXGIDevice* dxgiDevice = 0;
	if(FAILED((md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)))){
        return false;
    }
	      
	IDXGIAdapter* dxgiAdapter = 0;
	if(FAILED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter))){
        return false;
    }

	IDXGIFactory* dxgiFactory = 0;
	if(FAILED((dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory)))){
        return false;
    }

    //Create the swap chain!
	if(FAILED((dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain)))){
        LOG_ERRO << "Failed to Create SwapChain from device" << LOG_ENDL;
        return false;
    }
	
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);
	
	if( !onResize() ){
        return false;
    }

    LOG_INFO << "Direct3D11 Successfully Initialized" << LOG_ENDL;
    return true;
}

bool DX11Window::initInputDevices()
{
    RAWINPUTDEVICE rid[2];

    // Keyboard
    rid[0].usUsagePage = 1;
    rid[0].usUsage = 6;
    rid[0].dwFlags = 0;
    rid[0].hwndTarget=NULL;

    // Mouse
    rid[1].usUsagePage = 1;
    rid[1].usUsage = 2;
    rid[1].dwFlags = 0;
    rid[1].hwndTarget=NULL;

    if( !RegisterRawInputDevices( rid, 2, sizeof(RAWINPUTDEVICE) ) ){
        LOG_ERRO << "RegisterRawInputDevices() failed" << LOG_ENDL;
        return false;
    }

    LOG_INFO << "Registered Mouse and Keyboard Successfully." << LOG_ENDL;
    return true;
}

bool DX11Window::onResize()
{
    assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);

	// Resize the swap chain and recreate the render target view.
	if(FAILED(mSwapChain->ResizeBuffers(1, mWindowWidth, mWindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0))){
        return false;
    }

	ID3D11Texture2D* backBuffer;

	if(FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)))){
        LOG_ERRO << "Failed to GetBuffer() from SwapChain" << LOG_ENDL;
        return false;
    }

	if(FAILED(md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView))){
        LOG_ERRO << "Failed to CreateRenderTargetView() from Device" << LOG_ENDL;
        return false;
    }

	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	
	depthStencilDesc.Width     = mWindowWidth;
	depthStencilDesc.Height    = mWindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;

    //Disable multisampling for now
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	if(FAILED(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer))){
        LOG_ERRO << "Failed to CreateTexture2D() of DepthStencil" << LOG_ENDL;
        return false;
    }

	if(FAILED(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView))){
        LOG_ERRO << "Failed to CreateDepthStencilView from device" << LOG_ENDL;
        return false;
    }

	// Bind the render target view and depth/stencil view to the pipeline.
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// Set the viewport transform.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width    = static_cast<float>(mWindowWidth);
	mScreenViewport.Height   = static_cast<float>(mWindowHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

    LOG_INFO << "Created Render Target: " << mWindowWidth << ", " << mWindowHeight << LOG_ENDL;
    return true;
}

LRESULT CALLBACK DX11Window::mainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch( msg )
	{
	// WM_ACTIVATE is sent when the window is activated or deactivated.  
	// We pause the game when the window is deactivated and unpause it 
	// when it becomes active.  
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			mIsPaused = true;
		}
		else
		{
			mIsPaused = false;
		}
		return 0;
    case WM_KEYDOWN:
        if( LOWORD(wParam) == VK_ESCAPE ){
            PostQuitMessage(0);
        }
        return 0;

    case WM_INPUT:
        {
            UINT bufferSize;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof (RAWINPUTHEADER));

            // Create a buffer of the correct size - but see note below
            BYTE *buffer=new BYTE[bufferSize];

            // Call the function again, this time with the buffer to get the data
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (LPVOID)buffer, &bufferSize, sizeof (RAWINPUTHEADER));

            if( mInputHandler ){
                mInputHandler->handleInput( (RAWINPUT*)buffer );
            }

            delete[] buffer;
        }
		return 0;

	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		mWindowWidth  = LOWORD(lParam);
		mWindowHeight = HIWORD(lParam);

		if( md3dDevice )
		{
			if( wParam == SIZE_MINIMIZED )
			{
				mIsPaused = true;
				mIsMinimized = true;
				mIsMaximized = false;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				mIsPaused = false;
				mIsMinimized = false;
				mIsMaximized = true;
				if( !onResize() ){
                    PostQuitMessage(1);
                }
			}
			else if( wParam == SIZE_RESTORED )
			{
				
				// Restoring from minimized state?
				if( mIsMinimized )
				{
					mIsPaused = false;
					mIsMinimized = false;
					if( !onResize() ){
                        PostQuitMessage(1);
                    }
				}

				// Restoring from maximized state?
				else if( mIsMaximized )
				{
					mIsPaused = false;
					mIsMaximized = false;
					if( !onResize() ){
                        PostQuitMessage(1);
                    }
				}
				else if( mIsResizing )
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					if( !onResize() ){
                        PostQuitMessage(1);
                    }
				}
			}
		}
		return 0;

	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		mIsPaused = true;
		mIsResizing  = true;
		return 0;

	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mIsPaused = false;
		mIsResizing  = false;
		if( !onResize() ){
            PostQuitMessage(1);
        }
		return 0;
 
	// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
    case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);

	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;
        /*
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
        */
    default:
        break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}