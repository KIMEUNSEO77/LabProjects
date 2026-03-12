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


// 픽셀 셰이더를 정의
float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // x축 중앙 초록색 선
    if ((int) input.x == (int) HALF_WIDTH)
        cColor.g = 1.0f;
    
    // y축 중앙 빨간색 선
    if ((int) input.y == (int) HALF_HEIGHT)
        cColor.r = 1.0f;
    
    // 중심에서 반지름이 100인 원의 파란색 선
    // 현재 픽셀과 중심점 사이의 거리가 100과 같으면, 파란색 선으로 표현
    float fDistance = distance((int2) input.xy, float2(HALF_WIDTH, HALF_HEIGHT));
    // 0.5f는 오차 범위로, 100과 99.5 ~ 100.5 사이의 거리를 가진 픽셀도 파란색 선으로 표현
    if (IsEqual(fDistance, 100.0f, 0.5f))
        cColor.b = 1.0f;
    
    return (cColor);
}