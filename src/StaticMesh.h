#ifndef STATIC_MESH_H
#define STATIC_MESH_H

#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <d3dx11effect.h>

#include "Utils.h"

class StaticMesh{
public:

    StaticMesh();
    ~StaticMesh();

    //Load a mesh from a .obj file and a texture
    bool loadFromObj( ID3D11Device* device, char* objPath, LPCWSTR texturePath );

    //Draw the mesh
    void draw( ID3D11DeviceContext* device );

    //Clear allocated buffers and texture
    void clear();

    //Get and set the position
    inline XMFLOAT3& getPosition();

    //get the bound box of the loaded verts
    inline XMFLOAT3& getBoundingBoxMin();
    inline XMFLOAT3& getBoundingBoxMax();

protected:

    void calcBoundingBox( DungeonVertex* verts, uint vertCount );

protected:

    ID3D11Buffer* mVB;

    ID3D11ShaderResourceView* mTexture;

    int mVertexCount;

    XMFLOAT3 mPosition;

    XMFLOAT3 mBoundingBoxMin;
    XMFLOAT3 mBoundingBoxMax;
};

inline XMFLOAT3& StaticMesh::getPosition(){return mPosition;}
inline XMFLOAT3& StaticMesh::getBoundingBoxMin(){return mBoundingBoxMin;}
inline XMFLOAT3& StaticMesh::getBoundingBoxMax(){return mBoundingBoxMax;}

#endif