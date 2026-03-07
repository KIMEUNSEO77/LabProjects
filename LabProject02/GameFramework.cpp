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