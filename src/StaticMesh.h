#ifndef STATIC_MESH_H
#define STATIC_MESH_H

#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>

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

protected:

    ID3D11Buffer* mVB;

    ID3D11ShaderResourceView* mTexture;

    int mVertexCount;

    XMFLOAT3 mPosition;
};

inline XMFLOAT3& StaticMesh::getPosition(){return mPosition;}

#endif