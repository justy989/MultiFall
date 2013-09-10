#include "UIDisplay.h"
#include "Log.h"

#include "D3DUtils.h"

UIDisplay::UIDisplay() :
    mVB(NULL),
    mIB(NULL),
    mTexture(NULL),
    mPixelShader(NULL),
    mVertexShader(NULL),
    mSampler(NULL),
    mInputLayout(NULL)
{

}

UIDisplay::~UIDisplay()
{
    
}

bool UIDisplay::init( ID3D11Device* device, LPCWSTR uiTexturePath, LPCWSTR uiShaderPath )
{
    if( FAILED( D3DX11CreateShaderResourceViewFromFile( device,
        uiTexturePath, 0, 0, &mTexture, 0 ) ) ){
        LOG_ERRO << "Failed to create UI Texture out of: " << LOG_WC_TO_C(uiTexturePath) << LOG_ENDL;
        return false;
    }

    //Create quad for each object, then reference them when drawing as needed
    FontVertex verts[] = 
    {
        //Position                         Color                             Texture

        //Left Window Title Corner
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.0f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.125f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.125f)},

        //Middle Window Title
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.0f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.0f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.125f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.125f)},

        //Right Window Title Corner
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.0f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.0f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.125f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.125f)},

        
        //Top Left Window Body
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.125f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.125f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.25f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.25f)},

        //Top Middle Window Body
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.125f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.125f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.25f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.25f)},

        //Top Right Window Body
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.125f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.125f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.25f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.25f)},

        //Below Tab Left
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.125f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.125f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.25f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.25f)},

        //Below Tab Right
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.25f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.25f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.375f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.375f)},

        //Middle Left Window Body
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.25f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.25f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.375f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.375f)},

        //Middle Middle Window Body
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.25f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.25f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.375f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.375f)},

        //Middle Right Window Body
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.25f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.25f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.375f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.375f)},

        
        //Bottom Left Window Body
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.375f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.375f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.5f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.5f)},

        //Bottom Middle Window Body
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.375f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.375f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.5f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.5f)},

        //Bottom Right Window Body
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.375f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.375f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.5f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.5f)},


        //Checkbox unchecked
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.5f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.5f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.625f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.625f)},

        //Checkbox checked
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.5f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.5f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.625f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.625f)},


        //Left Arrow Slider
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.625f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.625f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.75f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.75f)},

         //Right Arrow Slider
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.625f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.625f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.75f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.75f)},

        //Filled Bar Slider
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.625f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.625f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.75f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.75f)},

         //Empty Bar Slider
        {XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.625f)},
        {XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.625f)},
        {XMFLOAT4(0.1f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.75f)},
        {XMFLOAT4(0.0f, -0.1f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.75f, 0.75f)},
    };

    ushort inds[ UI_INDEX_COUNT ];
    ushort v = 0;

    for(int i = 0; i < UI_INDEX_COUNT; ){
        inds[i] = v; i++;
        inds[i] = v + 1; i++;
        inds[i] = v + 2; i++;

        inds[i] = v; i++;
        inds[i] = v + 2; i++;
        inds[i] = v + 3; i++;

        v += 4;
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(FontVertex) * UI_INDEX_COUNT;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = verts;

    if( FAILED(device->CreateBuffer(&vbd, &vinitData, &mVB) ) ){
        LOG_ERRO << "Unable to allocate Vertex Buffer for UI" << LOG_ENDL;
        return false;
    }

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(ushort) * sizeof inds;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = inds;

    //Create IB
    if( FAILED(device->CreateBuffer(&ibd, &iinitData, &mIB) ) ){
        LOG_ERRO << "Unable to allocate Index Buffer for UI" << LOG_ENDL;
        return false;
    }

    //shader variables to be updated
	D3D11_BUFFER_DESC constDesc;
    ZeroMemory( &constDesc, sizeof( constDesc ) );
    constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constDesc.ByteWidth = sizeof( XMMATRIX );
    constDesc.Usage = D3D11_USAGE_DEFAULT;
    
	if( FAILED( device->CreateBuffer( &constDesc, 0, &mWorldCB ) ) ){
        LOG_ERRO << "Failed to create constant buffer for UI" << LOG_ENDL;
        return false;
    }

    //Compile the shaders
    ID3DBlob* vsBuffer = 0;

	if( !CompileD3DShader( uiShaderPath, "VS_Main", "vs_5_0", &vsBuffer ) ){
        LOG_ERRO << "Failed to compile: " << LOG_WC_TO_C(uiShaderPath) << " vertex shader" << LOG_ENDL;
        return false;
    }

	device->CreateVertexShader( vsBuffer->GetBufferPointer( ),
	vsBuffer->GetBufferSize( ), 0, &mVertexShader );	 

	ID3DBlob* psBuffer = 0;

	if( !CompileD3DShader( uiShaderPath, "PS_Main", "ps_5_0", &psBuffer ) ){
        LOG_ERRO << "Failed to compile: " << LOG_WC_TO_C(uiShaderPath) << " pixel shader" << LOG_ENDL;
        return false;
    }

	device->CreatePixelShader( psBuffer->GetBufferPointer( ),
	psBuffer->GetBufferSize( ), 0, &mPixelShader );

	psBuffer->Release( );

    //Create the sampler
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT; //Point filtering!
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    
    device->CreateSamplerState( &samplerDesc, &mSampler );

    //Input Description
    D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	unsigned int totalLayoutElements = ARRAYSIZE( vertexDesc );

	if( FAILED( device->CreateInputLayout(vertexDesc, totalLayoutElements, 
								vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(),
								&mInputLayout) ) ){
        LOG_ERRO << "Failed to create Input Layout for Text" << LOG_ENDL;
        return false;
    }

    vsBuffer->Release();

    return true;
}

void UIDisplay::clear()
{
    ReleaseCOM( mVB );
    ReleaseCOM( mIB );
    ReleaseCOM( mTexture );
    ReleaseCOM( mPixelShader );
    ReleaseCOM( mVertexShader );
    ReleaseCOM( mSampler );
    ReleaseCOM( mInputLayout );

    LOG_INFO << "Released UI Buffers, textures, shaders, sampler, and input layout" << LOG_ENDL;
}

void UIDisplay::prepareUIRendering( ID3D11DeviceContext* device )
{
    //Setup input layout and topology
    device->IASetInputLayout( mInputLayout );
    device->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    //Setup vertex and pixel shaders
	device->VSSetShader( mVertexShader, 0, 0 );
	device->PSSetShader( mPixelShader, 0, 0 );

    //Setup sampler
	device->PSSetSamplers( 0, 1, &mSampler );
}

void UIDisplay::drawWindow( ID3D11DeviceContext* device, UIWindow& window, TextManager& tm )
{
    uint stride = sizeof(FontVertex);
    uint offset = 0;

    //Set vertex and index buffers
    device->IASetVertexBuffers( 0, 1, &mVB, &stride, &offset );
    device->IASetIndexBuffer( mIB, DXGI_FORMAT_R16_UINT, offset );

    device->PSSetShaderResources( 0, 1, &mTexture );

    //Draw stuffs
    for(int i = 0; i < 20; i++){
        XMMATRIX world = XMMatrixTranslation( -1.0f + ( static_cast<float>(i) * 0.1f ), 0.0f, 0.0f);
        XMMATRIX worldMat = world;
        worldMat = XMMatrixTranspose( worldMat );

        device->UpdateSubresource( mWorldCB, 0, 0, &worldMat, 0, 0 );
	    device->VSSetConstantBuffers( 0, 1, &mWorldCB );

	    device->DrawIndexed(6, i * 6, 0);
    }
}