Texture2D texture_ : register( t0 );
Texture2D colorBuffer : register( t1 );
Texture2D normalBuffer : register( t2 );
Texture2D depthBuffer : register ( t3 );

SamplerState colorSampler_ : register( s0 );

cbuffer cbPerFrame : register( cb0 )
{
	float4x4 gViewProj;
	float4x4 gInvViewProj;
	float4x4 gWorld;
	//float4 gCameraPos;
};

cbuffer cbPerObject : register( cb1 )
{
	float4x4 omfg;
};

cbuffer cbPerLightPS : register( cb2 )
{
	float4 gLightPosition;	//16 bytes
	float4 gLightRadIntensity; //16 bytes
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
	
    float4x4 worldViewProj = gWorld * gViewProj;
    
    output.pos = mul(float4(input.pos, 1.0f), worldViewProj);
    output.tex = input.tex; //pass the texture coordinates further
    output.normal = input.normal;

    return output;
}

PointVertexOut vs_point(VertexIn input)
{
    PointVertexOut output;
	
    float4x4 worldViewProj = gWorld * gViewProj;
    
    output.pos = mul(float4(input.pos, 1.0f), gViewProj);
    output.screenPos = output.pos;
	
    return output;
}

/*
float4 ps_main(VertexOut pin) : SV_Target
{
	//Sample the texture
    float4 color = texture_.Sample( colorSampler_, pin.tex );

    return color;
}*/


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

float4 ps_directional(VertexOut pin) : SV_TARGET0
{
    //float4 n = normalBuffer.Sample( colorSampler_, pin.tex );
	//float3 normal = 2.0f * n.xyz - 1.0f; //transform back into (-1, 1)
	//normal = normalize(normal);
	
	//float lightIntensity;
	//float3 lightDir;

	float4 color = colorBuffer.Sample( colorSampler_, pin.tex );
	float4 ambient = color * 0.12f;
	
	//lightDir = float3(0.1f,0.02f,-0.05f);
	//lightDir = normalize(lightDir);
	
	// Calculate the amount of light on this pixel.
    //lightIntensity = saturate(dot(normal, lightDir));
	
	//if(lightIntensity > 0)
	//{
		color = ambient;//0.1f *saturate(lightIntensity * color + ambient);
	//}
	
    return color;
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
	float3 color = colorBuffer.Sample( colorSampler_, texCoord );
    float3 diffuseLight = color;
	
	float4 colorfinal = float4(attenuation * gLightRadIntensity.y * diffuseLight, 1.0f);
	
    return colorfinal;
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

technique11 DirLight
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, vs_fsquad() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, ps_directional() ) );
    }
}

technique11 PointLight
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, vs_point() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, ps_point() ) );
    }
}