#pragma once
// GameFramework.h

// 게임 프로그램 뼈대
// Direct3D 디바이스를 생성, 사용자 입력 처리, 애니메이션 등 화면 출력을 위한 여러 가지 처리 
class CGameFramework
{
private:
	HINSTANCE m_hInstance; // 애플리케이션 인스턴스 핸들
	HWND m_hWnd;           // 메인 윈도우 핸들

	int m_nWndClientWidth;  // 윈도우 클라이언트 영역의 너비
	int m_nWndClientHeight; // 윈도우 클라이언트 영역의 높이

	IDXGIFactory4* m_pdxgiFactory; // DXGI 팩토리 인터페이스 포인터
	IDXGISwapChain3* m_pdxgiSwapChain;   // DXGI 스왑 체인 인터페이스 포인터
	ID3D12Device* m_pd3dDevice;         // Direct3D 12 디바이스 인터페이스 포인터

	bool m_bMsaa4xEnable = false; // 4x MSAA 사용 여부

	UINT m_nMsaa4xQualityLevels = 0; // 4x MSAA 품질 수준 수
	static const UINT m_nSwapChainBuffers = 2; // 스왑 체인 후면 버퍼 수
	UINT m_nSwapChainBufferIndex; // 현재 스왑 체인 버퍼 인덱스

	ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers]; // 스왑 체인 후면 버퍼 리소스 포인터 배열
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap; // 렌더 타겟 뷰 디스크립터 힙 인터페이스 포인터
	UINT m_nDsvDescriptorIncrementSize; // 깊이-스텐실 뷰 디스크립터 크기 증가량

	ID3D12CommandQueue* m_pd3dCommandQueue; // 명령 큐 인터페이스 포인터
	ID3D12CommandAllocator* m_pd3dCommandAllocator; // 명령 할당자 인터페이스 포인터
	ID3D12GraphicsCommandList* m_pd3dCommandList; // 그래픽 명령 리스트 인터페이스 포인터

	ID3D12Fence* m_pd3dFence; // 펜스 인터페이스 포인터
	UINT64 m_nFenceValue; // 펜스 값
	HANDLE m_hFenceEvent; // 펜스 이벤트 핸들

	D3D12_VIEWPORT m_d3dViewport; // 뷰포트 구조체
	D3D12_RECT m_d3dScissorRect; // 가위 사각형 구조체

public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);   // 게임 프레임워크 초기화
	void OnDestroy(); // 게임 프레임워크 종료

	void CreateSwapChain(); // 스왑 체인 생성
	void CreateRtvAndDsvDescriptorHeaps(); // RTV와 DSV 디스크립터 힙 생성
	void CreateDirect3DDevice(); // Direct3D 디바이스 생성
	void CreateCommandQueueAndList(); // 명령 큐와 명령 리스트 생성

	void CreateRenderTargetView(); // 렌더 타겟 뷰 생성
	void CreateDepthStencilView(); // 깊이-스텐실 뷰 생성

	void ProcessInput(); // 사용자 입력 처리
	void AnimateObjects(); // 애니메이션 처리
	void FrameAdvance(); // 프레임 진행(렌더링)

	void WaitForGpuComplete(); // CPU와 GPU 동기화

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // 마우스 메시지 처리
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // 키보드 메시지 처리
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // 윈도우 메시지 처리
};

