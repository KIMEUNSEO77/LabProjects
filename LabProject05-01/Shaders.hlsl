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


// 픽셀 셰이더를 정의
float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // 입력된 픽셀의 위치를 NDC(Normalized Device Coordinates)로 변환
    float2 f2NDC = float2(input.x / FRAME_BUFFER_WIDTH, input.y / FRAME_BUFFER_HEIGHT) - 0.5f;
    // 화면 비율 보정 (원이 찌그러지지 않도록)
    f2NDC.x *= (FRAME_BUFFER_WIDTH / FRAME_BUFFER_HEIGHT);
    
    float fLength = length(f2NDC);   // 중심 거리
    float fMin = 0.3f, fMax = 0.2f;
    
    // smoothstep : fMin보다 작으면 0, fMax보다 크면 1, 그 사이면 0과 1 사이의 값을 반환
    // 부드럽게 경계를 나타내줌
    cColor.rgb = smoothstep(fMin, fMax, fLength);
    
    return (cColor);
}