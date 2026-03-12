// Shader.hlsl

// HLSL(High-Level Shading Language)로, DirectX에서 GPU를 작성하는 언어
// 

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}

// 정점 셰이더 정의
// 렌더 타겟 사각형 전체를 삼각형 2개로 표현한다.
float4 VSMain(uint nVertexID : SV_VertexID) : SV_POSITION
{
    float4 output = (float4) 0;
    if (nVertexID == 0)
        output = float4(-1.0f, +1.0f, 0.0f, 1.0f);
    else if (nVertexID == 1)
        output = float4(+1.0f, +1.0f, 0.0f, 1.0f);
    else if (nVertexID == 2)
        output = float4(+1.0f, -1.0f, 0.0f, 1.0f);
    else if (nVertexID == 3)
        output = float4(-1.0f, +1.0f, 0.0f, 1.0f);
    else if (nVertexID == 4)
        output = float4(+1.0f, -1.0f, 0.0f, 1.0f);
    else if (nVertexID == 5)
        output = float4(-1.0f, -1.0f, 0.0f, 1.0f);
    return (output);
}

#define FRAME_BUFFER_WIDTH 1280.0f
#define FRAME_BUFFER_HEIGHT 720.0f

#define HALF_WIDTH   (FRAME_BUFFER_WIDTH * 0.5f)
#define HALF_HEIGHT  (FRAME_BUFFER_HEIGHT * 0.5f)
#define EPSILON       1.0e-5f

inline bool IsZero(float fValue)
{
    return ((abs(fValue) <=  EPSILON));
}
inline bool IsZero(float fValue, float fEpsilon)
{
    return ((abs(fValue) <= fEpsilon));
}
inline bool IsEqual(float fA, float fB, float fEpsilon)
{
    return ((abs(fA - fB) <= fEpsilon));
}

float Rectangle(float2 f2NDC, float fLeft, float fRight, float fTop, float fBottom)
{
    float2 f2Shape = float2(step(fLeft, f2NDC.x), step(f2NDC.x, fRight));
    f2Shape *= float2(step(fTop, f2NDC.y), step(f2NDC.y, fBottom));
    
    return (f2Shape.x * f2Shape.y);
}

// 픽셀 셰이더를 정의
float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    float2 f2NDC = float2(input.x / FRAME_BUFFER_WIDTH, input.y / FRAME_BUFFER_HEIGHT) - 0.5f; // (0, 1) : (-0.5, 0.5)
    f2NDC.x *= (FRAME_BUFFER_WIDTH / FRAME_BUFFER_HEIGHT);
    
    // 점들이 놓일 원의 반지름
    float fRadius = 0.3f;
    // 각도 간격 = 12도? 점 하나 찍고, 12도 회전하며 점을 찍는 것을 반복
    float fRadian = radians(360.0f / 30.0f);
    
    for (float f = 0; f < 30.0f; f += 1.0f)
    {
        // 원 둘레에 12도 간격으로 점을 고르게 찍음
        float fAngle = fRadian * f;
        // 0.0025f는 점의 밝기, 점과 픽셀 사이의 거리가 가까울수록 밝기가 증가하도록 함
        cColor.rgb += (0.0025f / length(f2NDC + float2(fRadius * cos(fAngle), fRadius * sin(fAngle))));
    }
    
    return (cColor);
}