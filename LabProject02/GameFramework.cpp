// GameFramework.cpp

#include "stdafx.h"
#include "GameFramework.h"


CGameFramework::CGameFramework()
{
	m_pdxgiFactory = nullptr;
	m_pdxgiSwapChain = nullptr;
	m_pd3dDevice = nullptr;

	m_pd3dCommandAllocator = nullptr;
	m_pd3dCommandQueue = nullptr;
	m_pd3dPipeLineState = nullptr;
	m_pd3dCommandList = nullptr;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dSwapChainBackBuffers[i] = nullptr;
	m_pd3dRtvDescriptorHeap = nullptr;
	m_nRtvDescriptorIncrementSize = 0;

	m_pd3dDepthStencilBuffer = nullptr;
	m_pd3dDsvDescriptorHeap = nullptr;
	m_nDsvDescriptorIncrementSize = 0;

	m_nSwapChainBufferIndex = 0;

	m_hFenceEvent = nullptr;
	m_pd3dFence = nullptr;
	m_nFenceValue = 0;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;
}

CGameFramework::~CGameFramework()
{

}

// 응용 프로그램이 실행되어 주 윈도우가 생성되면 호출됨
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	// Direct3D 디바이스, 명령 큐와 명령 리스트, 스왑 체인 등을 생성하는 함수 호출
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetView();
	CreateDepthStencilView();

	BuildObjects(); // 렌더링할 게임 오브젝트 생성

	return(true);
}

void CGameFramework::OnDestroy()
{
	WaitForGpuComplete(); // GPU가 모든 명령을 완료할 때까지 대기

	ReleaseObjects(); // 게임 오브젝트 해제(소멸)

	::CloseHandle(m_hFenceEvent); // 펜스 이벤트 핸들 닫기

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i])
		m_ppd3dSwapChainBackBuffers[i]->Release(); // 스왑 체인 후면 버퍼 리소스 해제
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release(); // RTV 디스크립터 힙 해제

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release(); // 깊이-스텐실 버퍼 리소스 해제
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release(); // DSV 디스크립터 힙 해제

	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release(); // 명령 할당자 해제
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release(); // 명령 큐 해제
	if (m_pd3dPipeLineState) m_pd3dPipeLineState->Release(); // 파이프라인 상태 해제
	if (m_pd3dCommandList) m_pd3dCommandList->Release(); // 명령 리스트 해제

	if (m_pd3dFence) m_pd3dFence->Release(); // 펜스 해제

	m_pdxgiSwapChain->SetFullscreenState(FALSE, nullptr); // 전체 화면 모드 해제

	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release(); // 스왑 체인 해제
	if (m_pd3dDevice) m_pd3dDevice->Release(); // Direct3D 디바이스 해제
	if (m_pdxgiFactory) m_pdxgiFactory->Release(); // DXGI 팩토리 해제

#if defined(_DEBUG)
		IDXGIDebug1* pdxgiDebug = nullptr;
		DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
		HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, 
			DXGI_DEBUG_RLO_DETAIL);
		pdxgiDebug->Release();
#endif
}


// 스왑 체인 생성 코드 구현
void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient); // 윈도우 클라이언트 영역의 크기 얻기
	m_nWndClientWidth = rcClient.right - rcClient.left; // 클라이언트 영역의 너비 계산
	m_nWndClientHeight = rcClient.bottom - rcClient.top; // 클라이언트 영역의 높이 계산

	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1)); // 스왑 체인 설명 구조체 초기화
	dxgiSwapChainDesc.Width = m_nWndClientWidth; // 스왑 체인 버퍼의 너비 설정
	dxgiSwapChainDesc.Height = m_nWndClientHeight; // 스왑 체인 버퍼의 높이 설정
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 스왑 체인 버퍼의 픽셀 형식 설정
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1; // MSAA 샘플 수 설정
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0; // MSAA 품질 수준 설정

	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 스왑 체인 버퍼의 용도 설정
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers; // 스왑 체인 버퍼 수 설정
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE; // 스왑 체인 버퍼의 스케일링 모드 설정
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 스왑 체인 버퍼 교체 효과 설정
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // 스왑 체인 버퍼의 알파 모드 설정
	dxgiSwapChainDesc.Flags = 0; // 스왑 체인 플래그 설정

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC)); // 전체 화면 스왑 체인 설명 구조체 초기화
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60; // 전체 화면 모드의 새로 고침 빈도 분자 설정
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1; // 전체 화면 모드의 새로 고침 빈도 분모 설정
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 전체 화면 모드의 스캔라인 순서 설정
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // 전체 화면 모드의 스케일링 모드 설정
	dxgiSwapChainFullScreenDesc.Windowed = TRUE; // 전체 화면 모드 여부 설정

	m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, 
		&dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, nullptr, 
		(IDXGISwapChain1**)&m_pdxgiSwapChain); // 윈도우용 스왑 체인 생성

	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER); // Alt+Enter 키 조합으로 전체 화면 모드 전환 방지

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex(); // 현재 스왑 체인 버퍼 인덱스 얻기
}