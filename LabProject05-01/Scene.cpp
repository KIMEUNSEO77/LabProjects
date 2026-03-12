#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

// 루트 시그너쳐(CPU -> GPU 자원 연결 규칙)를 생성
void CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	// 아무 리소스도 없는 가장 기본적인 루트 시그너쳐를 생성

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = 0;  // 아무 자원도 전달하지 않음
	d3dRootSignatureDesc.pParameters = NULL;
	d3dRootSignatureDesc.NumStaticSamplers = 0;  // texture 샘플러도 전달하지 않음
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	// 정점 레이아웃 사용 허용 (Vertex Buffer에서 데이터를 읽어올 때 필요)
	d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// 루트 시그니처 바이너리 저장용 Blob, 에러 메시지 저장용 Blob을 생성
	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	// 루트 시그니처 직렬화 (GPU가 이해할 수 있는 형태로 변환)
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	// GPU에 루트 시그니처 생성
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void
			**)&m_pd3dGraphicsRootSignature);
	// 블롭 메모리 해제
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
}

// 그래픽 파이프라인 상태 객체를 생성 (GPU 렌더링 파이프라인의 전체 설정을 담고 있음)
// ex) 어떤 셰이더를 사용할지, 래스터라이저 상태, 블렌드 상태 등 렌더링 설정을 GPU에게 전달
void CScene::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
{
	// 정점 셰이더와 픽셀 셰이더를 생성 (블롭은 컴파일된 바이트 셰이더 코드 저장용)
	ID3DBlob* pd3dVertexShaderBlob = NULL;
	ID3DBlob* pd3dPixelShaderBlob = NULL;
	UINT nCompileFlags = 0;

	// 디버그 모드에서는 셰이더 디버깅과 최적화 생략을 위해 컴파일 플래그 설정
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// 셰이더 컴파일	(HLSL 파일에서 "VSMain"과 "PSMain" 함수를 각각 정점 셰이더와 픽셀 셰이더로 컴파일)
	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "VSMain", "vs_5_1", nCompileFlags, 0,
		&pd3dVertexShaderBlob, NULL);
	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "PSMain", "ps_5_1", nCompileFlags, 0,
		&pd3dPixelShaderBlob, NULL);

	// 래스터라이저 상태를 설정
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;   // 폴리곤을 채우는 방식 (솔리드)
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;    // 뒷면 컬링 (카메라에서 보이지 않는 면은 그리지 않음)
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;      // 정점이 시계 방향으로 정의된 경우 앞면으로 간주
	// 깊이 바이어스 설정 (깊이 버퍼에서 픽셀의 깊이 값을 조정하여 Z-파이팅 방지) - 사용 안함
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;        // 깊이 클리핑 활성화 (깊이 범위를 벗어난 픽셀은 그리지 않음)
	d3dRasterizerDesc.MultisampleEnable = FALSE;     // 다중 샘플링 비활성화 (MSAA 사용 안함)
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE; // 안티앨리어싱 라인 비활성화 (선 렌더링에서 안티앨리어싱 사용 안함)
	d3dRasterizerDesc.ForcedSampleCount = 0;         // 강제 샘플 수 (0은 기본값 사용)
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF; // 보수적 래스터라이제이션 비활성화 (기본 래스터라이제이션 사용)

	// 블렌드 상태를 설정 (픽셀 색상과 렌더 타겟의 기존 색상을 어떻게 혼합할지 정의)
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;    // 알파 투 커버리지 비활성화 (MSAA에서 알파 채널을 사용하여 샘플 커버리지 결정하는 기능 사용 안함)
	d3dBlendDesc.IndependentBlendEnable = FALSE;   // 독립 블렌딩 비활성화 (모든 렌더 타겟에 동일한 블렌드 설정 적용)
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;  // 블렌딩 비활성화 (픽셀 색상이 렌더 타겟의 기존 색상을 덮어쓰도록 설정)
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE; // 논리 연산 비활성화 (픽셀 색상과 렌더 타겟의 기존 색상에 대해 논리 연산 사용 안함)
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE; // 소스 블렌드 팩터 설정 (픽셀 색상 그대로 사용)
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO; // 대상 블렌드 팩터 설정 (렌더 타겟의 기존 색상 무시)
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // 블렌드 연산 설정 (소스와 대상 색상을 더하는 방식)
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;  // 소스 알파 블렌드 팩터 설정 (픽셀 알파 그대로 사용)
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO; // 대상 알파 블렌드 팩터 설정 (렌더 타겟의 기존 알파 무시)
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD; // 알파 블렌드 연산 설정 (소스와 대상 알파를 더하는 방식)
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;  // 논리 연산 설정 (논리 연산 사용 안함)
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // 렌더 타겟에 모든 색상 채널(RGBA) 쓰기 허용

	// 그래픽 파이프라인 상태를 설정 (이 구조체에 셰이더, 래스터라이저 상태, 블렌드 상태 등 렌더링 설정을 담아서 GPU에 전달)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = m_pd3dGraphicsRootSignature;
	// 컴파일된 셰이더 코드의 포인터와 크기를 설정하여 GPU에 전달
	d3dPipelineStateDesc.VS.pShaderBytecode = pd3dVertexShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.VS.BytecodeLength = pd3dVertexShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.PS.pShaderBytecode = pd3dPixelShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.PS.BytecodeLength = pd3dPixelShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.RasterizerState = d3dRasterizerDesc;
	d3dPipelineStateDesc.BlendState = d3dBlendDesc;
	d3dPipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	d3dPipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	d3dPipelineStateDesc.InputLayout.pInputElementDescs = NULL;
	d3dPipelineStateDesc.InputLayout.NumElements = 0;
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.SampleDesc.Quality = 0;
	// 지금까지 채운 설정으로 그래픽 파이프라인 상태 객체를 GPU에 생성
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
}

// 렌더링 준비에 필요한 객체들을 생성 (루트 시그너쳐, 그래픽 파이프라인 상태 객체 등)
void CScene::BuildObjects(ID3D12Device* pd3dDevice)
{
	CreateGraphicsRootSignature(pd3dDevice);
	CreateGraphicsPipelineState(pd3dDevice);
}
void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
	LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput()
{
	return(false);
}
void CScene::AnimateObjects(float fTimeElapsed)
{
}

// 렌더링을 하기 위해 GPU에 렌더링 설정을 전달하는 함수
// 그 프레임에서 그릴 준비를 함.
void CScene::PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 그래픽 루트 시그너쳐를 설정
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	// 파이프라인 상태를 설정
	pd3dCommandList->SetPipelineState(m_pd3dPipelineState);
	// 프리미티브 토폴로지(삼각형 리스트)를 설정 (어떤 방식으로 정점들을 연결하여 도형을 그릴지 설정)
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	PrepareRender(pd3dCommandList);
	// 정점 3개를 사용하여 렌더링
	pd3dCommandList->DrawInstanced(3, 1, 0, 0);
}

