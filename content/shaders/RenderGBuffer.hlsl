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

cbuffer cbPerFrame : register( b0 )
{
	float4x4 gViewProj;
	float4x4 gInvViewProj;
	float4 gCameraPos;
};

cbuffer cbPerObject : register( b1 )
{
	float4x4 gWorld;
};

cbuffer cbPerLightPS : register( b2 )
{
	float4 gLightPosition;	//16 bytes
	float4 gLightRadIntensity; //16 bytes
};

cbuffer cbFogPS : register( b3 )
{
	float4 gFogData;	//16 bytes
};

cbuffer cbParticles : register( b4 )
{
	ParticleInstanceData gParticles[100];
};

struct VertexIn
{
	float3 pos  : POSITION;
	float3 normal : NORMAL;
	float2 tex :	TEXCOORD;
};

struct VertexOut
{
	float4 pos  : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex :	TEXCOORD;
};

struct LightParticleIn
{
	float3 pos  : POSITION;
};

struct LightParticleGSIn
{
	float4 pos  : POSITION;
	float2 tex :	TEXCOORD;
};

struct LightParticleOut
{
	float4 pos  : SV_POSITION;
	float4 screenPos : TEXCOORD0;
	float2 tex :	TEXCOORD1;
};

struct PointVertexOut
{
	float4 pos  : SV_POSITION;
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
	float4 color = colorBuffer.Sample( colorSampler_, pin.tex );
	float depth = depthBuffer.Sample( colorSampler_, pin.tex ).r;
	
    return ( float4(gFogData.xyz, 0) * saturate((1.0 - depth) * gFogData.w) ) * color * 0.5f;
}

float4 ps_point(PointVertexOut pin) : SV_TARGET0
{
	pin.screenPos.xy /= pin.screenPos.w;
	float2 texCoord = 0.5f * (float2(pin.screenPos.x, -pin.screenPos.y) + 1);
	
    float4 n = normalBuffer.Sample( colorSampler_, texCoord );
	float3 normal = 2.0f * n.xyz - 1.0f; //transform back into (-1, 1)
	normal = normalize(normal);
	
	float depth = depthBuffer.Sample( colorSampler_, texCoord ).r;
	
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
	float3 ambientLight = colorBuffer.Sample( colorSampler_, texCoord );

    return float4(float4(normalize(gFogData.xyz), 0) * (saturate((1.0f - depth) * 2.0f * gFogData.w)) * attenuation * gLightRadIntensity.y * ambientLight, 1.0f);
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
		float3 rightVector = 0.02f * normalize(cross(planeNormal, upVector));
		upVector = 0.02f * normalize(cross(rightVector, planeNormal));

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
	if(particleDepth > geoDepth)
	{
		clip(-1);		
	}

	return texture_.Sample( colorSampler_, input.tex );	
	
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