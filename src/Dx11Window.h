#ifndef DX11WINDOW_H
#define DX11WINDOW_H

#include <d3dx11.h>
#include <string>

/*
    WMInputHandler: Inherit from this object then register it to handle input
*/
class WMInputHandler{
public:
    //Handle input from window's events
    virtual void handleInput( RAWINPUT* input ) = 0;
};

/*
    DX11Window: Creates a window as well as initializes a Direct3d11 device
*/
class DX11Window
{
public:

    //Constr/Destr

	DX11Window();
	~DX11Window();
	
	//Create a window with a given caption, width, and height. This is also the size of the back buffer Also pass in the object that will handle raw input
    bool init( LPCWSTR windowCaption, unsigned int windowWidth, unsigned int windowHeight, HINSTANCE hInstance, int nCmdShow, WMInputHandler* inputHandler );
            
    //Clear Back Depth and Stencil Buffers
    void clearBDS();

    //clear the window
    void clear( );

    //Callback given to window's event handler
	LRESULT CALLBACK mainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    //Accessor Functions

	inline float getAspectRatio();
    inline ID3D11Device* getDevice();
    inline ID3D11DeviceContext* getDeviceContext();
    inline IDXGISwapChain* getSwapChain();
    inline bool isPaused();
    inline bool isMinimized();
    inline bool isMaximized();
    inline bool isResizing();

    inline int getWindowWidth();
    inline int getWindowHeight();

protected:

	bool initWindow();
	bool initDirect3D();
    bool initInputDevices();

    bool onResize();

protected:

	HINSTANCE mhAppInst;
	HWND      mhMainWnd;
	bool      mIsPaused;
	bool      mIsMinimized;
	bool      mIsMaximized;
	bool      mIsResizing;
	UINT      m4xMsaaQuality;

	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* md3dImmediateContext;
	IDXGISwapChain* mSwapChain;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;

	D3D_DRIVER_TYPE md3dDriverType;
	std::wstring mWindowCaption;
	int mWindowWidth;
	int mWindowHeight;
    int mWindowRefreshRate;
	bool mEnable4xMsaa;

    WMInputHandler* mInputHandler;
};

inline float DX11Window::getAspectRatio(){return mWindowWidth / mWindowHeight;}
inline ID3D11Device* DX11Window::getDevice(){return md3dDevice;}
inline ID3D11DeviceContext* DX11Window::getDeviceContext(){return md3dImmediateContext;}
inline IDXGISwapChain* DX11Window::getSwapChain(){return mSwapChain;}
inline bool DX11Window::isPaused(){return mIsPaused;}
inline bool DX11Window::isMinimized(){return mIsMinimized;}
inline bool DX11Window::isMaximized(){return mIsMaximized;}
inline bool DX11Window::isResizing(){return mIsResizing;}
inline int DX11Window::getWindowWidth(){return mWindowWidth;}
inline int DX11Window::getWindowHeight(){return mWindowHeight;}

#endif // DX11WINDOW_H