// Shader.hlsl

// HLSL(High-Level Shading Language)로, DirectX에서 GPU를 작성하는 언어
// 

//정점 데이터와 인스턴싱 데이터를 위한 구조체이다.
struct VS_INSTANCING_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float4x4 mtxTransform : WORLDMATRIX;
    float4 instanceColor : INSTANCECOLOR;
};
struct VS_INSTANCING_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// CPU에서 만든 값 → GPU 셰이더로 전달하기 위해 상수 버퍼(Constant Buffer)를 선언
// CPU에서 계산한 행렬을 셰이더가 받아서 사용, 즉 실제 3D 렌더링이 시작된 것

// 게임 객체의 정보를 위한 상수 버퍼를 선언
cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
};
// 카메라의 정보를 위한 상수 버퍼를 선언
cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
};

// 정점 셰이더의 입력을 위한 구조체를 선언
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

// 정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// 일반 렌더링
// 정점 셰이더
VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;
    
// 정점을 변환(월드 변환, 카메라 변환, 투영 변환) - GPU에서 직접 계산을 하기 때문에 매우 빠르게 처리됨
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
    output.color = input.color;
    
    return (output);
}
//픽셀 셰이더
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    return (input.color);
}


// 인스턴싱 렌더링
// 정점 셰이더
VS_INSTANCING_OUTPUT VSInstancing(VS_INSTANCING_INPUT input)
{
    VS_INSTANCING_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), input.mtxTransform),
gmtxView), gmtxProjection);
    output.color = input.color + input.instanceColor;
    return (output);
}
// 픽셀 셰이더
float4 PSInstancing(VS_INSTANCING_OUTPUT input) : SV_TARGET
{
    return (input.color);
}