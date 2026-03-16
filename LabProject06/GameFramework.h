#pragma once
// GameFramework.h

#include "Timer.h"
#include "Scene.h"

// 게임 프로그램 뼈대
// Direct3D 디바이스를 생성, 사용자 입력 처리, 애니메이션 등 화면 출력을 위한 여러 가지 처리 
// 윈도우 만들기 + D3D12 장치 준비 + 버퍼 준비 + 명령 전송 + 입력 처리 + 렌더링 + 종료 처리
// 이 전부를 관리하는 게임 프레임워크의 본체
class CGameFramework
{
private:
	HINSTANCE m_hInstance; // 애플리케이션 인스턴스 핸들
	HWND m_hWnd;           // 메인 윈도우 핸들

	int m_nWndClientWidth;  // 윈도우 클라이언트 영역의 너비
	int m_nWndClientHeight; // 윈도우 클라이언트 영역의 높이

	IDXGIFactory4* m_pdxgiFactory; // DXGI 팩토리 인터페이스 포인터 (DXGI는 Direct3D와 함께 사용되는 그래픽 인터페이스로, 디바이스와 스왑 체인 등을 관리)
	IDXGISwapChain3* m_pdxgiSwapChain;   // DXGI 스왑 체인 인터페이스 포인터 (화면에 렌더링된 이미지를 표시하기 위한 버퍼 체인 관리) 
	ID3D12Device* m_pd3dDevice;         // Direct3D 12 디바이스 인터페이스 포인터 (그래픽 하드웨어와 상호 작용하여 리소스 생성, 명령 실행 등을 담당)

	bool m_bMsaa4xEnable = false; // 4x MSAA (Multi-Sample Anti-Aliasing) 사용 여부 - 화면의 계단 현상을 줄이기 위한 기술
	UINT m_nMsaa4xQualityLevels = 0; // 4x MSAA 품질 수준 수

	static const UINT m_nSwapChainBuffers = 2; // 스왑 체인 후면 버퍼 수 - 일반적으로 2 또는 3 사용 (더 많은 버퍼는 더 부드러운 렌더링을 제공하지만 더 많은 메모리를 사용)
	// 2면 더블 버퍼링
	UINT m_nSwapChainBufferIndex; // 현재 스왑 체인 버퍼 인덱스

	// RTV: 렌더 타겟 뷰는 렌더링 결과를 저장하는 버퍼에 대한 뷰. 스왑 체인의 후면 버퍼에 대한 RTV가 필요
	ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers]; // 스왑 체인 후면 버퍼 리소스 포인터 배열
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap; // 렌더 타겟 뷰 디스크립터 힙 인터페이스 포인터 (렌더 타겟 정보를 저장)
	UINT m_nDsvDescriptorIncrementSize; // 깊이-스텐실 뷰 디스크립터 크기 증가량

	// DSV: 깊이-스텐실 뷰는 깊이 버퍼와 스텐실 버퍼에 대한 뷰. 누가 앞에 있는지 결정
	// 깊이 버퍼는 픽셀의 깊이 정보를 저장하여 올바른 렌더링 순서를 보장
	// 스텐실 버퍼는 특정 픽셀에 대한 추가적인 테스트를 수행하는 데 사용
	ID3D12Resource* m_pd3dDepthStencilBuffer; // 깊이-스텐실 버퍼 리소스 포인터
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap; // 깊이-스텐실 뷰 디스크립터 힙 인터페이스 포인터 
	UINT m_nRtvDescriptorIncrementSize; // 렌더 타겟 뷰 디스크립터 크기 증가량

	// 명령 큐는 GPU에 명령을 제출하는 인터페이스 (명령 리스트를 실행하기 위한 큐)
	// 명령 할당자는 명령 리스트를 기록하기 위한 메모리를 관리
	ID3D12CommandQueue* m_pd3dCommandQueue; // 명령 큐 인터페이스 포인터
	ID3D12CommandAllocator* m_pd3dCommandAllocator; // 명령 할당자 인터페이스 포인터
	ID3D12GraphicsCommandList* m_pd3dCommandList; // 그래픽 명령 리스트 인터페이스 포인터

	ID3D12PipelineState* m_pd3dPipeLineState; // 파이프라인 상태 인터페이스 포인터 (렌더링 설정과 셰이더 프로그램을 포함하는 객체)

	// CPU와 GPU 간의 동기화를 위한 메커니즘. CPU가 GPU가 특정 작업을 완료할 때까지 기다리도록 할 수 있음
	ID3D12Fence* m_pd3dFence; // 펜스 인터페이스 포인터
	UINT64 m_nFenceValues[m_nSwapChainBuffers]; // 펜스 값 (후면 버퍼마다 현재 펜스 값을 관리하기 위해 수정
	HANDLE m_hFenceEvent; // 펜스 이벤트 핸들

	D3D12_VIEWPORT m_d3dViewport; // 뷰포트 구조체
	D3D12_RECT m_d3dScissorRect; // 가위 사각형 구조체

	// 게임 프레임워크에서 사용할 타이머
	CGameTimer m_GameTimer;
	// 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열
	_TCHAR m_pszFrameRate[50];

	CScene* m_pScene; // 게임 씬 포인터 (게임의 모든 오브젝트와 로직을 관리하는 클래스)

public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);   // 게임 프레임워크 초기화
	void OnDestroy(); // 게임 프레임워크 종료

	void CreateSwapChain(); // 스왑 체인 생성
	void CreateRtvAndDsvDescriptorHeaps(); // RTV와 DSV 디스크립터 힙 생성
	void CreateDirect3DDevice(); // Direct3D 디바이스 생성 (GPU 사용 준비)
	void CreateCommandQueueAndList(); // 명령 큐와 명령 리스트 생성

	void CreateRenderTargetView(); // 렌더 타겟 뷰 생성 
	void CreateDepthStencilView(); // 깊이-스텐실 뷰 생성
	void BuildObjects(); // 게임 오브젝트 생성
	void ReleaseObjects(); // 게임 오브젝트 해제

	void ProcessInput(); // 사용자 입력 처리
	void AnimateObjects(); // 애니메이션 처리
	void FrameAdvance(); // 프레임 진행(렌더링)

	void WaitForGpuComplete(); // CPU와 GPU 동기화

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // 마우스 메시지 처리
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // 키보드 메시지 처리
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // 윈도우 메시지 처리

	void ChangeSwapChainState(); // 스왑 체인 상태 변경 (예: 전체 화면과 창 모드 간 전환)
	void MoveToNextFrame(); // 다음 프레임으로 이동 (GPU와의 동기화 포함)
};

