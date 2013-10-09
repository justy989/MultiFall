#ifndef DUNGENAPP_H
#define DUNGENAPP_H

#include "NetServer.h"
#include "NetClient.h"

#include "Dx11Window.h"
#include "Timer.h"
#include "Camera.h"

#include "WorldDisplay.h"
#include "World.h"

#include "Configuration.h"
#include "UserBinds.h"

#include "TextManager.h"

#include "WorldEntity.h"
#include "WorldGenerator.h"

#include "StaticMesh.h"

#include "UIDisplay.h"

#include "EmitterManager.h"

#include "Console.h"

#include "EventManager.h"

#include "Party.h"

#include "ScreenManager.h"

#include <d3dx11effect.h>

class App : public WMInputHandler{
public:

    App();

    //handle input from the dx11 window
    virtual void handleInput( RAWINPUT* input );

    //run the application
    int run( HINSTANCE hInstance, int nCmdShow );

	struct EnvVertex{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 tex;
	};

protected:

    bool init( );
	bool initShaders( );
	bool initRTVs( );	

    void update( float dt );
    void draw( );

	void drawFSQuad( );

    void clear( );

protected:

    void genLevel( uint seed );

protected:

    friend class Console;

    Configuration mConfig;

    UserBinds mBinds;

    DX11Window mWindow;

    Timer mTimer;

    Camera mCamera;

    WorldDisplay mWorldDisplay;

    World mWorld;

    Party mParty;

	TextManager mTextManager;

    Console mConsole;

    EventManager mEventManager;

    ScreenManager mScreenManager;

    UIDisplay mUIDisplay;

    //Tmp
    bool camKeyDown[4];

    bool collisionMode;
    bool drawStats;
    bool freeLook;
    bool consoleMode;

    char FPSString[128];
    char CameraPosString[128];
    char MousePosString[128];

    WorldEntity mEntity;

    WorldGenerator::LevelGenerationRanges mLevelGenRanges;
    WorldGenerator::PopulationGenerationRanges mPopGenRanges;
    WorldGenerator mWorldGen;

	EmitterManager mEmitterManager;

    bool mLeftClick;

    bool mKeyPress;
    USHORT mKeyChar;

    float mBlockDimenions;

protected:	

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mRenderGBufferTech;
	ID3DX11EffectTechnique* mAmbientLightTech;
	ID3DX11EffectTechnique* mPointLightTech;
	ID3DX11EffectTechnique* mLightParticleTech;
    ID3DX11EffectTechnique* mCharacterBillboardTech;

	ID3D11InputLayout* mInputLayout;
	ID3D11Buffer* mFSQuadVB;
    ID3D11Buffer* mFSQuadIB;
    ID3D11Buffer* mPerObjectCB;

	XMFLOAT2 mHalfPixel;

	//G-buffer render targets for deferred shading
	//[0] = color
	//[1] = normal
	ID3D11RenderTargetView* mGBufferRTVs[2];
	ID3D11DepthStencilView* mDepthStencilView;
	ID3D11Texture2D* mGBufferTextures[3];
	ID3D11ShaderResourceView* mGBufferSRVs[3];
};

#endif