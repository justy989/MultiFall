#include "StaticMesh.h"
#include "Log.h"

#include <fstream>
#include <vector>
#include <assert.h>

StaticMesh::StaticMesh() :
    mVB(NULL),
    mTexture(NULL),
    mVertexCount(0)
{

}

StaticMesh::~StaticMesh()
{
    
}

bool StaticMesh::loadFromObj( ID3D11Device* device, char* objPath, LPCWSTR texturePath )
{
    if( FAILED( D3DX11CreateShaderResourceViewFromFile( device,
        texturePath, 0, 0, &mTexture, 0 ) ) ){
        LOG_ERRO << "Failed to create Shader resource out of: " << LOG_WC_TO_C(texturePath) << LOG_ENDL;
        return false;
    }

    std::ifstream file(objPath);

    if( !file.is_open() ){
        LOG_ERRO << "Unable to open static mesh .obj file: " << objPath << LOG_ENDL;
        return false;
    }

    char buffer[128];
    std::vector<XMFLOAT3> verts;
    std::vector<XMFLOAT2> uvs;
    std::vector<XMFLOAT3> normals;

    std::vector<DungeonVertex> vb;

    while( !file.eof() ){
        file >> buffer;

        if( buffer[0] == '#' ){
            file.getline(buffer, 128);
            continue;
        }

        if( strcmp( buffer, "v" ) == 0 ){
            //Verts
            XMFLOAT3 v;
            file >> buffer; v.x = static_cast<float>(atof(buffer));
            file >> buffer; v.y = static_cast<float>(atof(buffer));
            file >> buffer; v.z = static_cast<float>(atof(buffer));
            verts.push_back(v);
        }else if( strcmp( buffer, "vt" ) == 0 ){
            //Texture UVs            
            XMFLOAT2 v; //Flip the text coords, idk why...
            file >> buffer; v.x = 1.0f - static_cast<float>(atof(buffer));
            file >> buffer; v.y = 1.0f - static_cast<float>(atof(buffer));
            uvs.push_back(v);
        }else if( strcmp( buffer, "vn" ) == 0 ){
            //Normals
            XMFLOAT3 n;
            file >> buffer; n.x = static_cast<float>(atof(buffer));
            file >> buffer; n.y = static_cast<float>(atof(buffer));
            file >> buffer; n.z = static_cast<float>(atof(buffer));
            normals.push_back(n);
        }else if( strcmp( buffer, "f" ) == 0 ){
            //Weird Indicies
            for(int i = 0; i < 3; i++){
                file >> buffer;

                char* v = strtok(buffer, "/");
                char* t = strtok(NULL, "/");
                char* n = strtok(NULL, "/");

                DungeonVertex vertex;
                vertex.position = verts[ atoi( v ) - 1 ];
                vertex.tex = uvs[ atoi( t ) - 1 ];
                vertex.normal = normals[ atoi( n ) - 1 ];

                vb.push_back( vertex );
            }
        }else{
            file.getline(buffer, 128);
            continue;
        }
    }

    mVertexCount = vb.size();

    DungeonVertex* vertData = new DungeonVertex[ mVertexCount ];

    for(int i = 0; i < mVertexCount; i++){
        vertData[i] = vb[i];
    }

    calcBoundingBox( vertData, mVertexCount );

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(DungeonVertex) * mVertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertData;

    if( FAILED( device->CreateBuffer(&vbd, &vinitData, &mVB) ) ){
        LOG_ERRO << "Unable to allocate Vertex Buffer for Static Mesh: " << objPath << LOG_ENDL;
        delete[] vertData;
        return false;
    }

    delete[] vertData;
    return true;
}

void StaticMesh::calcBoundingBox( DungeonVertex* verts, uint vertCount )
{
    assert( vertCount > 0 );

    //Init data to start, assumes vertexCount > 0!
    mBoundingBoxMin.x = verts[0].position.x;
    mBoundingBoxMax.x = verts[0].position.x;
    mBoundingBoxMin.y = verts[0].position.y;
    mBoundingBoxMax.y = verts[0].position.y;
    mBoundingBoxMin.z = verts[0].position.z;
    mBoundingBoxMax.z = verts[0].position.z;

    //Loop through each vertex
    for(int i = 0; i < mVertexCount; i++){

        //Calc X min and max
        if( verts[i].position.x < mBoundingBoxMin.x ){
            mBoundingBoxMin.x = verts[i].position.x;
        }

        if( verts[i].position.x > mBoundingBoxMax.x ){
            mBoundingBoxMax.x = verts[i].position.x;
        }

        //Calc Y min and max
        if( verts[i].position.y < mBoundingBoxMin.y ){
            mBoundingBoxMin.y = verts[i].position.y;
        }

        if( verts[i].position.y > mBoundingBoxMax.y ){
            mBoundingBoxMax.y = verts[i].position.y;
        }

        //Calc Z min and max
        if( verts[i].position.z < mBoundingBoxMin.z ){
            mBoundingBoxMin.z = verts[i].position.z;
        }

        if( verts[i].position.z > mBoundingBoxMax.z ){
            mBoundingBoxMax.z = verts[i].position.z;
        }
    }
}

void StaticMesh::draw( ID3D11DeviceContext* device )
{
    uint stride = sizeof DungeonVertex;
    uint offset = 0;

    device->PSSetShaderResources(0, 1, &mTexture );
    device->IASetVertexBuffers( 0, 1, &mVB, &stride, &offset );
    device->Draw( mVertexCount, 0 );
}

void StaticMesh::clear()
{
    ReleaseCOM( mVB );
    ReleaseCOM( mTexture );

    mVertexCount = 0;
}