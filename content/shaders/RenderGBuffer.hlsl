Texture2D texture_ : register( t0 );
Texture2D colorBuffer : register( t1 );
Texture2D normalBuffer : register( t2 );
Texture2D depthBuffer : register ( t3 );

struct ParticleInstanceData
{
	float3 pos;
	float rot;
	float4 isAlive;
};

DepthStencilState depthState
{
	DepthEnable = TRUE;
	DepthWriteMask = 1;
	DepthFunc = ALWAYS;
};

SamplerState colorSampler_
{
	Filter = MIN_MAG_MIP_POINT;
	ComparisonFunc = NEVER;
	AddressU = WRAP;
	AddressV = WRAP;
};

BlendState blendState
{
	BlendEnable[0] = TRUE;
	BlendOp[0] = ADD;
	SrcBlend[0] = ONE;
	DestBlend[0] = ONE;
	RenderTargetWriteMask[0] = 0x0F;
	SrcBlendAlpha[0] = ONE;
	DestBlendAlpha[0] = ONE;
	BlendOpAlpha[0] = ADD;
};

BlendState alphaToCoverageBlendState
{
	BlendEnable[0] = TRUE;
	BlendOp[0] = ADD;
	SrcBlend[0] = ONE;
	DestBlend[0] = ONE;
	RenderTargetWriteMask[0] = 0x0F;
	SrcBlendAlpha[0] = ONE;
	DestBlendAlpha[0] = ONE;
	BlendOpAlpha[0] = ADD;
    AlphaToCoverageEnable = TRUE;
};

cbuffer cbPerFrame : register( b0 )
{
	float4x4 gViewProj;
	float4x4 gInvViewProj;
	float4 gCameraPos;
    float gCameraNear;
    float gCameraFar;
    float2 gPerFramePadding;
};

cbuffer cbPerObject : register( b1 )
{
	float4x4 gWorld;
};

cbuffer cbPerLightPS : register( b2 )
{
	float4 gLightPosition;	//32 bytes
	float4 gLightRadIntensity; //16 bytes
    float4 gLightColor; // 32 bytes
};

cbuffer cbFogPS : register( b3 )
{
    float gFogStart;
    float gFogEnd;
    float gFogDiff;
    float gFogPadding;
	float4 gFogColor;//16 bytes
};

cbuffer cbParticles : register( b4 )
{
	ParticleInstanceData gParticles[100];
};

cbuffer cbBillboardFixed : register( b5 )
{
    float2 gBillboardTexCoord[6] =
    {
        float2(0, 1),
	    float2(1, 1),
	    float2(0, 0),
        float2(1, 1),
	    float2(0, 0),
	    float2(1, 0)
    };
};

struct VertexIn
{
	float3 pos    : POSITION;
	float3 normal : NORMAL;
	float2 tex    :	TEXCOORD;
};

struct VertexOut
{
	float4 pos    : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex    :	TEXCOORD;
};

struct LightParticleIn
{
	float3 pos  : POSITION;
};

struct LightParticleGSIn
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD;
};

struct LightParticleOut
{
	float4 pos       : SV_POSITION;
	float4 screenPos : TEXCOORD0;
	float2 tex       : TEXCOORD1;
};

struct BillboardIn
{
    float4 pos     : POSITION;
    float4 dim     : DIMENSION;
    //float2 tex     : TEXCOORD;
    //float2 texDim  : TEXDIM;
};

struct BillboardOut
{
	float4 pos       : SV_POSITION;
	float4 screenPos : TEXCOORD0;
	float2 tex       : TEXCOORD1;
};

struct PointVertexOut
{
	float4 pos       : SV_POSITION;
	float4 screenPos :	TEXCOORD;
};

struct GBufferOut
{
	half4 color : SV_TARGET0;
	half4 normal : SV_TARGET1;
};

VertexOut vs_main(VertexIn input)
{
    VertexOut output;
	
    float4x4 worldViewProj = mul(gWorld, gViewProj);
    
    output.pos = mul(float4(input.pos, 1.0f), worldViewProj);
    output.tex = input.tex; //pass the texture coordinates further
    output.normal = input.normal;

    return output;
}

PointVertexOut vs_point(VertexIn input)
{
    PointVertexOut output;
	
    float4x4 worldViewProj = mul(gWorld, gViewProj);
    
    output.pos = mul(float4(input.pos, 1.0f), worldViewProj);
    output.screenPos = output.pos;
	
    return output;
}

GBufferOut ps_main(VertexOut input)
{
    GBufferOut output;
	
    output.color = texture_.Sample( colorSampler_, input.tex ); //output Color
    output.normal.rgb = 0.5f * (normalize(input.normal) + 1.0f);      //transform normal domain
	output.normal.a = 1.0f;
	
    return output;
}

VertexOut vs_fsquad(VertexIn input)
{
    VertexOut output;
    
    output.pos = float4(input.pos, 1.0f);
    output.tex = input.tex;                            //pass the texture coordinates further
    output.normal = input.normal;
	
    return output;
}

float4 ps_ambient(VertexOut pin) : SV_TARGET0
{
	float4 color = colorBuffer.Sample( colorSampler_, pin.tex ) * 0.5f;
    //return color * 0.5f;
    float depth = depthBuffer.Sample( colorSampler_, pin.tex ).r;

    float scaledDepth = (2.0f * depth) - 1.0f; //Convert to -1 to 1
    depth = 2.0 * gCameraNear * gCameraFar / (gCameraFar + gCameraNear - scaledDepth * (gCameraFar - gCameraNear)); //Calculate world depths
    //depth = 2.0 * 0.05f * 15.0f / (15.0f + 0.05f - scaledDepth * (15.0f - 0.05f));
    float fogBlend = clamp( depth, gFogStart, gFogEnd );
    fogBlend = ( fogBlend - gFogStart ) / gFogDiff;

    return ( ( 1.0f - fogBlend ) * color ) + ( fogBlend * gFogColor );
}

float4 ps_point(PointVertexOut pin) : SV_TARGET0
{
	pin.screenPos.xy /= pin.screenPos.w;
	float2 texCoord = 0.5f * (float2(pin.screenPos.x, -pin.screenPos.y) + 1);
	float depth = depthBuffer.Sample( colorSampler_, texCoord ).r;

    //float lDepth = pin.screenPos.z /= pin.screenPos.w;

    //if( lDepth > depth )
	//{
		//clip(-1);		
	//}

    float4 n = normalBuffer.Sample( colorSampler_, texCoord );
	float3 normal = 2.0f * n.xyz - 1.0f; //transform back into (-1, 1)
	normal = normalize(normal);
	
	
	float4 position;
    position.xy = pin.screenPos.xy;
    position.z = depth;
    position.w = 1.0f;
	
	//transform to world space
    position = mul(position, gInvViewProj);
    position /= position.w;
	
	//surface-to-light vector
    float3 lightVector = gLightPosition - position;
	
    //compute attenuation based on distance - linear attenuation
    float attenuation = saturate( 1.0f - length(lightVector) / gLightRadIntensity.x ); 
	
    //normalize light vector
    lightVector = normalize(lightVector); 
	
    //compute diffuse light
    float NdL = saturate( dot( normal,lightVector ) );
	float3 ambientLight = colorBuffer.Sample( colorSampler_, texCoord ) * 0.5f;

    float4 litColor = float4( NdL * attenuation * gLightRadIntensity.y * gLightColor.xyz * ambientLight, 1.0f );

    return litColor;

    //I tried fogging up the lights but on any other color than black it looked horrible
    //Calculate fog based on scaled depth
    //float fogBlend = clamp( lDepth, gFogStart, gFogEnd );
    //fogBlend = ( fogBlend - gFogStart ) / gFogDiff;

    //return ( ( 1.0f - fogBlend ) * litColor ) + ( fogBlend * gFogColor );
}

LightParticleGSIn vs_lightparticle(LightParticleIn input, uint instanceID : SV_InstanceID)
{
	LightParticleGSIn output;
	//float4x4 worldViewProj = mul(gWorld, gViewProj);    
    output.pos = float4(gParticles[instanceID].pos, 1.0f);
	output.tex = float2(gParticles[instanceID].isAlive.x, gParticles[instanceID].rot);

	return output;
}

[maxvertexcount(4)]
void gs_lightparticle(point LightParticleGSIn input[1], inout TriangleStream<LightParticleOut> output)
{
	//if the particle instance is alive
	if(input[0].tex.x > 0)
	{
		float3 planeNormal = input[0].pos.xyz - gCameraPos.xyz;
		planeNormal = normalize(planeNormal);

		float3 upVector = float3(0,1,0);//float3(cos(input[0].tex.y), sin(input[0].tex.y), 0.0f);
		float3 rightVector = 0.01f * normalize(cross(planeNormal, upVector));
		upVector = 0.01f * normalize(cross(rightVector, planeNormal));

		// Create the billboards quad
		float3 vert[4];

		// We get the points by using the billboards right vector and the billboards height
		vert[0] = input[0].pos.xyz - rightVector; // Get bottom left vertex
		vert[1] = input[0].pos.xyz + rightVector; // Get bottom right vertex
		vert[2] = input[0].pos.xyz - rightVector + upVector; // Get top left vertex
		vert[3] = input[0].pos.xyz + rightVector + upVector; // Get top right vertex

		// Get billboards texture coordinates
		float2 texCoord[4];
		texCoord[0] = float2(0, 1);
		texCoord[1] = float2(1, 1);
		texCoord[2] = float2(0, 0);
		texCoord[3] = float2(1, 0);

		// Now we "append" or add the vertices to the outgoing stream list
		LightParticleOut outputVert;
		for(int i = 0; i < 4; i++)
		{
			outputVert.pos = mul(float4(vert[i], 1.0f), gViewProj);
			outputVert.screenPos = outputVert.pos;
			outputVert.tex = texCoord[i];

			output.Append(outputVert);
		}
	}
}

float4 ps_lightparticle(LightParticleOut input) : SV_Target0
{
	input.screenPos.xy /= input.screenPos.w;
	float2 texCoord = 0.5f * (float2(input.screenPos.x, -input.screenPos.y) + 1);

	float geoDepth = depthBuffer.Sample( colorSampler_, texCoord ).r;
	float particleDepth = input.screenPos.z / input.screenPos.w;

	if( particleDepth > geoDepth )
	{
		clip(-1);		
	}

	return texture_.Sample( colorSampler_, input.tex );	
	
}

BillboardIn vs_billboard( BillboardIn bb )
{
    return bb;
}

[maxvertexcount(6)]
void gs_billboard( point BillboardIn bb[1], inout TriangleStream<BillboardOut> output )
{
    float3 planeNormal = bb[0].pos.xyz - gCameraPos.xyz;
	planeNormal = normalize(planeNormal);

	float3 upVector = float3(0,1,0);
	float3 rightVector = normalize(cross(planeNormal, upVector));

	float4 vert[6];

    float3 hOffset = rightVector * bb[0].dim.x;
    float3 vOffset = upVector * bb[0].dim.y;

	// We get the points by using the billboards right vector and the billboards height
	vert[0] = float4(bb[0].pos.xyz - hOffset - vOffset, 1.0f); // Get bottom left vertex
	vert[1] = float4(bb[0].pos.xyz + hOffset - vOffset, 1.0f); // Get bottom right vertex
	vert[2] = float4(bb[0].pos.xyz - hOffset + vOffset, 1.0f); // Get top left vertex

    vert[3] = vert[1];
    vert[4] = vert[2];
	vert[5] = float4(bb[0].pos.xyz + hOffset + vOffset, 1.0f); // Get top right vertex

	// Now we "append" or add the vertices to the outgoing stream list
	BillboardOut outputVert;
	for(int i = 0; i < 6; i++)
	{
		outputVert.pos = mul(vert[i], gViewProj);
		outputVert.screenPos = outputVert.pos;
		outputVert.tex = gBillboardTexCoord[i];

		output.Append(outputVert);
	}
}

float4 ps_billboard(BillboardOut input) : SV_Target0
{
	input.screenPos.xy /= input.screenPos.w;
	float2 texCoord = 0.5f * (float2(input.screenPos.x, -input.screenPos.y) + 1);

	float geoDepth = depthBuffer.Sample( colorSampler_, texCoord ).r;
	float particleDepth = input.screenPos.z / input.screenPos.w;

	if( particleDepth > geoDepth )
	{
		clip(-1);		
	}

    float4 color = texture_.Sample( colorSampler_, input.tex );

    clip( color.a < 0.1f ? -1 : 1 );

	return color;	
}

technique11 GeoPass
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, vs_main() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, ps_main() ) );
    }
}

technique11 AmbientLight
{
    pass P0
    {
		SetDepthStencilState(depthState, 0);
		SetBlendState(blendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF );
        SetVertexShader( CompileShader( vs_5_0, vs_fsquad() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, ps_ambient() ) );
    }
}

technique11 PointLight
{
    pass P0
    {
		SetDepthStencilState(depthState, 0);
		SetBlendState(blendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF );
        SetVertexShader( CompileShader( vs_5_0, vs_point() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, ps_point() ) );
    }
}

technique11 Billboard
{
    pass P0
    {
		SetDepthStencilState(depthState, 0);
		SetBlendState(alphaToCoverageBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF );
        //SetBlendState(blendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF );
        SetVertexShader( CompileShader( vs_5_0, vs_billboard() ) );
		SetGeometryShader( CompileShader( gs_5_0, gs_billboard() ) );
        SetPixelShader( CompileShader( ps_5_0, ps_billboard() ) );
    }
}

technique11 LightParticle
{
    pass P0
    {
		SetDepthStencilState(depthState, 0);
		SetBlendState(blendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF );
        SetVertexShader( CompileShader( vs_5_0, vs_lightparticle() ) );
		SetGeometryShader( CompileShader( gs_5_0, gs_lightparticle() ) );
        SetPixelShader( CompileShader( ps_5_0, ps_lightparticle() ) );
    }
}