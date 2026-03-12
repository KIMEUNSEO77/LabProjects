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

// 주어진 반지름(fRadius) 안의 정다각형(fSides) 그리기
float RegularPolygon(float2 f2NDC, float fSides, float fRadius)
{
    float fAngle = atan(f2NDC.y / f2NDC.x);   // 현재 픽셀 각도 (중심 기준 어느 방향에 있는지)
    float fSlices = (2.0f * 3.14159f) / fSides;   // 다각형 각도 간격
    
    // step(distance, radius) : distance가 radius보다 작으면 1.0f, 아니면 0.0f 반환
    float fShape = step(cos(floor((fAngle / fSlices) + 0.5f) * fSlices - fAngle) * length(f2NDC), fRadius);
    
    return (fShape);
}

// 픽셀 셰이더를 정의
float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // 픽셀 좌표를 NDC로 변환 (정규화)
    float2 f2NDC = input.xy / float2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT) - 0.5f;
    
    // NDC 좌표를 화면 비율에 맞게 조정 (원이 찌그러지지 않게)
    f2NDC.x *= (FRAME_BUFFER_WIDTH / FRAME_BUFFER_HEIGHT);
    // 좌표 10배 확대 (더 많은 패턴이 보이도록)
    f2NDC.xy *= 10.0f;
    
    // length(f2NDC) : 중심에서 현재 픽셀까지의 거리
    // cos(length(f2NDC) * 3.14159f) : 중심에서 멀어질수록 주기적으로 색상이 변화 (원형 패턴)
    // smoothstep(0.125f, 0.875f, value) : value가 0.125f보다 작으면 0.0f, 0.875f보다 크면 1.0f, 그 사이면 부드럽게 변화
    cColor.b = smoothstep(0.125f, 0.875f, abs(cos(length(f2NDC) * 3.14159f)));
    
    return (cColor);
}